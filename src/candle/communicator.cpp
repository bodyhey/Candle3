#include "communicator.h"
#include "globals.h"
#include <QVector3D>
#include <QDebug>
#include <QCheckBox>
#include <QMessageBox>
#include <QTextCursor>
#include <QRegularExpression>
#include <parser/gcodeviewparse.h>

Communicator::Communicator(
    QObject *parent,
    Connection *connection,
    Configuration *configuration
) : QObject(parent),
    m_connection(connection),
    m_configuration(configuration),
    m_timerStateQuery(this),
    m_deviceStatesDictionary({
        {DeviceUnknown, "Unknown"},
        {DeviceIdle, "Idle"},
        {DeviceAlarm, "Alarm"},
        {DeviceRun, "Run"},
        {DeviceHome, "Home"},
        {DeviceHold0, "Hold:0"},
        {DeviceHold1, "Hold:1"},
        {DeviceQueue, "Queue"},
        {DeviceCheck, "Check"},
        {DeviceDoor0, "Door:0"},
        {DeviceDoor1, "Door:1"},
        {DeviceDoor2, "Door:2"},
        {DeviceDoor3, "Door:3"},
        {DeviceJog, "Jog"},
        {DeviceSleep, "Sleep"}
    })
{
    m_reseting = false;
    m_resetCompleted = true;
    m_aborting = false;
    m_statusReceived = false;
    m_spindleCW = true;

    resetStateVariables();

    // this->connect(m_connection, &Connection::error, this, &Communicator::onConnectionError(QString));
    if (m_connection) {
        connect(m_connection, &Connection::lineReceived, this, &Communicator::onConnectionLineReceived);
    }

    setSenderStateAndEmitSignal(SenderStopped);

    // Update state timer
    connect(&m_timerStateQuery, &QTimer::timeout, this, &Communicator::onTimerStateQuery);
    m_timerStateQuery.start();
}

void Communicator::resetStateVariables()
{
    m_deviceState = DeviceUnknown;
    m_senderState = SenderUnknown;
    m_machinePos = QVector3D(0, 0, 0);
    m_workPos = QVector3D(0, 0, 0);
    m_machineConfiguration = nullptr;
}

/**
 * @param tableIndex -1 - ui commands, -2 - utility commands, -3 - utility commands
 */
SendCommandResult Communicator::sendCommand(
    CommandSource source,
    QString commandLine,
    int tableIndex,
    bool wait,
    CommandCallback callback
) {
    QRegularExpressionMatch match;

    // tableIndex:
    // 0...n - commands from g-code program
    // -1 - ui commands
    // -2 - utility commands
    // -3 - utility commands

    if (!m_connection->isConnected() || !m_resetCompleted) return SendDone;

    // Check command
    if (commandLine.isEmpty()) return SendEmpty;

    // Place to queue on 'wait' flag
    if (wait) {
        m_queue.append(CommandQueue(source, commandLine, tableIndex));

        return SendQueue;
    }

    // Evaluate scripts in command
    // @todo scripting??
    //if (tableIndex < 0) command = evaluateCommand(command);

    // Check evaluated command
    if (commandLine.isEmpty()) return SendEmpty;

    // Place to queue if command buffer is full
    if ((bufferLength() + commandLine.length() + 1) > BUFFERLENGTH) {
        m_queue.append(CommandQueue(source, commandLine, tableIndex, callback));

        return SendQueue;
    }

    commandLine = commandLine.toUpper();

    CommandAttributes commandAttributes(
        source,
        m_commandIndex++,
        tableIndex,
        commandLine,
        callback
    );

    m_commands.append(commandAttributes);

    QString command = GcodePreprocessorUtils::removeComment(commandLine);

    // Processing spindle speed only from g-code program
    static QRegularExpression s("[Ss]0*(\\d+)");
    match = s.match(command);
    if (match.hasMatch() && commandAttributes.tableIndex > -2) {
        int speed = match.captured(1).toInt();
        // @TODO we are about to send new spindle speed, should we update UI now or wait for response?? or
        // maybe in onFeedSpindleSpeedReceived ??
        // if (ui->slbSpindle->value() != speed) {
        //     ui->slbSpindle->setValue(speed);
        // }
    }

    // Set M2 & M30 commands sent flag
    static QRegularExpression M230("(M0*2|M30|M0*6|M25)(?!\\d)");
    static QRegularExpression M6("(M0*6)(?!\\d)");
    if ((m_senderState == SenderTransferring) && command.contains(M230)) {
        if (
            !command.contains(M6) ||
            m_configuration->senderModule().useToolChangeCommands() ||
            m_configuration->senderModule().pauseSenderOnToolChange()
        ) {
            setSenderStateAndEmitSignal(SenderPausing);
        }
    }

    // Queue offsets request on G92, G10 commands
    static QRegularExpression G92("(G92|G10)(?!\\d)");
    if (command.contains(G92)) sendCommand(source, "$#", TABLE_INDEX_UTIL2, true);

    m_connection->sendLine(commandLine);

    emit commandSent(commandAttributes);

    return SendDone;
}

void Communicator::sendRealtimeCommand(QString command)
{
    if (command.length() != 1) return;
    if (!m_connection->isConnected() || !m_resetCompleted) return;

    m_connection->sendByteArray(QByteArray(command.toLatin1(), 1));
}

void Communicator::sendRealtimeCommand(int command)
{
    QByteArray data;
    data.append(char(command));
    m_connection->sendByteArray(data);
}

void Communicator::sendCommands(CommandSource source, QString commands, int tableIndex)
{
    QStringList list = commands.split("\n");

    bool waitFlag = false;
    foreach (QString cmd, list) {
        SendCommandResult r = sendCommand(source, cmd.trimmed(), tableIndex, waitFlag);
        if (r == SendDone || r == SendQueue) waitFlag = true;
    }
}

bool Communicator::streamCommands(GCode &streamer)
{
    // if (cannot be streamed) {
    //     return false;
    // }

    m_streamer = &streamer;
    //startStreaming();

    return true;
}

void Communicator::clearCommandsAndQueue()
{
    qDebug() << "clearing commands and queue";
    m_commands.clear();
    clearQueue();
}

void Communicator::clearQueue()
{
    m_queue.clear();
}

void Communicator::reset()
{
    assert(m_connection != nullptr);

    qDebug() << "resetting communicator";

    m_connection->sendByteArray(QByteArray(1, GRBL_LIVE_SOFT_RESET));

    resetStateVariables();

    setSenderStateAndEmitSignal(SenderStopped);
    setDeviceStateAndEmitSignal(DeviceUnknown);
    // in main form
    //m_fileCommandIndex = 0;

    m_reseting = true;
    m_homing = false;
    m_resetCompleted = false;
    // in main form
//    m_updateSpindleSpeed = true;
    m_statusReceived = true;

    // Drop all remaining commands in buffer
    clearCommandsAndQueue();
    // m_commands.clear();
    // m_queue.clear();

    // Prepare reset response catch
    QString command = "[CTRL+X]";
    CommandAttributes commandAttributes(
        CommandSource::System,
        m_commandIndex++,
        TABLE_INDEX_UI, // why UI ??
        command
    );
    m_commands.append(commandAttributes);

    if (m_streamer != nullptr) {
        m_streamer->reset();
    }
    m_updateSpindleSpeed = true;
}

void Communicator::abort()
{
    // @TODO is CommandSource::Program correct here??
    if (isSenderState(SenderPaused, SenderChangingTool)) {
        sendCommand(CommandSource::GeneralUI, "M2", TABLE_INDEX_UI, false);
    } else {
        sendCommand(CommandSource::GeneralUI, "M2", TABLE_INDEX_UI, true);
    }
}

/*
* @todo make sure we can replace connection at this point!!
*/
void Communicator::replaceConnection(Connection *newConnection)
{
    if (m_connection != nullptr || m_connection == newConnection) return;

    disconnect(m_connection, &Connection::lineReceived, this, &Communicator::onConnectionLineReceived);
    disconnect(m_connection, &Connection::stateChanged, this, &Communicator::onConnectionStateChanged);

    m_connection->disconnect();
    m_connection = newConnection;

    connect(m_connection, &Connection::lineReceived, this, &Communicator::onConnectionLineReceived);
    connect(m_connection, &Connection::stateChanged, this, &Communicator::onConnectionStateChanged);
}

void Communicator::stopUpdatingState()
{
    m_timerStateQuery.stop();
}

void Communicator::startUpdatingState(int interval)
{
    if (interval > 0) {
        m_timerStateQuery.setInterval(interval);
    }
    m_timerStateQuery.start();
}

void Communicator::restoreOffsets()
{
    // Still have pre-reset working position
    sendCommand(
        CommandSource::System,
        QString("%4G53G90X%1Y%2Z%3").arg(m_machinePos.x()).arg(m_machinePos.y()).arg(m_machinePos.z()).arg(m_machineConfiguration->unitsInches() ? "G20" : "G21"),
        TABLE_INDEX_UTIL1
    );

    sendCommand(
        CommandSource::System,
        QString("%4G92X%1Y%2Z%3").arg(m_workPos.x()).arg(m_workPos.y()).arg(m_workPos.z()).arg(m_machineConfiguration->unitsInches() ? "G20" : "G21"),
        TABLE_INDEX_UTIL1
    );
}

void Communicator::setSenderStateAndEmitSignal(SenderState state)
{
    if (m_senderState != state) {
        m_senderState = state;
        emit senderStateChanged(state);
    }

    // make sure state is updated before emitting signal, is it correct?
    emit senderStateReceived(state);
}

void Communicator::setDeviceStateAndEmitSignal(DeviceState state)
{
    if (m_deviceState != state) {
        m_deviceState = state;
        emit deviceStateChanged(state);
    }
}

int Communicator::bufferLength()
{
    int length = 0;

    foreach (CommandAttributes ca, m_commands) {
        length += ca.length;
    }

    return length;
}

// send commands until buffer is full
void Communicator::sendStreamerCommandsUntilBufferIsFull()
{
    if (m_queue.length() > 0) return;

    QString command = m_streamer->command();
    static QRegularExpression M230("(M0*2|M30|M0*6)(?!\\d)");

    qDebug() <<
        "bufferLength: " << bufferLength() <<
        "command.length: " << command.length() <<
        "commandIndex: " << m_streamer->commandIndex() <<
        "hasMoreCommands: " << m_streamer->hasMoreCommands() <<
        "m_commands.isEmpty: " << (!m_commands.isEmpty() && GcodePreprocessorUtils::removeComment(m_commands.last().commandLine).contains(M230));

    while ((bufferLength() + command.length() + 1) <= BUFFERLENGTH
           && m_streamer->hasMoreCommands() /* commandIndex() < m_form->currentModel().rowCount() - 1 */
           && !(!m_commands.isEmpty() && GcodePreprocessorUtils::removeComment(m_commands.last().commandLine).contains(M230))
    ) {
        m_streamer->commandSent();
        sendCommand(CommandSource::Program, command, m_streamer->commandIndex());
        m_streamer->advanceCommandIndex();
        command = m_streamer->command();
    }
}

bool Communicator::isMachineConfigurationReady() const
{
    return m_machineConfiguration != nullptr;
}

bool Communicator::isSenderState(SenderState state) const
{
    return m_senderState == state;
}

void Communicator::processConnectionTimer()
{
    if (m_connection == nullptr || !m_connection->isConnected()) {
        return;
    }

    // @TODO what does it do??? not homing, not hold, empty queue, are these the idle state tasks??
    // @TODO refactor ui->cmdHold->isChecked, for now we will assume that its value is always false
    if (!m_homing /*&& !ui->cmdHold->isChecked()*/ && m_queue.empty()) {
        if (m_updateSpindleSpeed) {
            m_updateSpindleSpeed = false;
            // sendCommand(CommandSource::System, QString("S%1").arg(ui->slbSpindle->value()), COMMAND_TI_UTIL1);
        }
        if (m_updateParserState) {
            m_updateParserState = false;
            sendCommand(CommandSource::System, "$G", TABLE_INDEX_UTIL2, false);
        }
    }
}

/* used by scripting engine only?? emit signal and do not use m_storedVars directly */
void Communicator::processOffsetsVars(QString response)
{
    static QRegularExpression gx("\\[(G5[4-9]|G28|G30|G92|PRB):([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)");
    static QRegularExpression tx("\\[(TLO):([\\d\\.\\-]+)");

    int p = 0;
    QRegularExpressionMatch match = gx.match(response);
    while (match.hasMatch()) {
        p = match.capturedStart();
        m_storedVars.setCoords(
            match.captured(1),
            QVector3D(
                match.captured(2).toDouble(),
                match.captured(3).toDouble(),
                match.captured(4).toDouble()
            )
        );

        p += match.capturedLength();
        match = gx.match(response, p);
    }

    match = tx.match(response);
    if (match.hasMatch()) {
        m_storedVars.setCoords(
            match.captured(1),
            QVector3D(
                0,
                0,
                match.captured(2).toDouble()
            )
        );
    }
}

void Communicator::onTimerStateQuery()
{
    if (!m_connection) {
        return;
    }

    // qDebug() << m_connection->isConnected() << m_resetCompleted << m_statusReceived;
    if (m_connection->isConnected() && m_resetCompleted && m_statusReceived) {
        m_connection->sendByteArray(QByteArray(1, '?'));
        m_statusReceived = false;
    }

    // @todo find some other way to update buffer state
    //ui->glwVisualizer->setBufferState(QString(tr("Buffer: %1 / %2 / %3")).arg(bufferLength()).arg(m_commands.length()).arg(m_queue.length()));
}

bool Communicator::compareCoordinates(double x, double y, double z)
{
    return m_machinePos.x() == x && m_machinePos.y() == y && m_machinePos.z() == z;
}

double Communicator::toMetric(double value)
{
    return m_machineConfiguration->units() == Units::Millimeters ? value : value * 25.4;
}

double Communicator::toInches(double value)
{
    return m_machineConfiguration->units() == Units::Inches ? value : value / 25.4;
}

void Communicator::storeParserState()
{
    // Remove GC:, Gx Mx, Fx, Sx ??
    // @TODO do it better
    m_storedParserState = m_lastParserState.remove(QRegularExpression("GC:|\\[|\\]|G[01234]\\s|M[0345]+\\s|\\sF[\\d\\.]+|\\sS[\\d\\.]+"));
}

void Communicator::restoreParserState()
{
    if (!m_storedParserState.isEmpty()) {
        sendCommand(CommandSource::System, m_storedParserState, TABLE_INDEX_UI);
    }
}

void Communicator::completeTransfer()
{
    // // Shadow last segment
    // GcodeViewParse *parser = m_currentDrawer->viewParser();
    // QList<LineSegment*> list = parser->getLineSegmentList();
    // if (m_lastDrawnLineIndex < list.count()) {
    //     list[m_lastDrawnLineIndex]->setDrawn(true);
    //     m_currentDrawer->update(QList<int>() << m_lastDrawnLineIndex);
    // }

    // Update state
    setSenderStateAndEmitSignal(SenderStopped);
    m_streamer->resetProcessed();
    //m_lastDrawnLineIndex = 0;
    m_storedParserState.clear();

    // updateControlsState();

    // Send end commands
    if (m_configuration->senderModule().useProgramEndCommands())
        sendCommands(CommandSource::ProgramAdditionalCommands, m_configuration->senderModule().programEndCommands());

    emit transferCompleted();

    // Show message box
    //qApp->beep();

    // stopUpdatingState();
    // m_timerConnection.stop();

    // QMessageBox::information(this, qApp->applicationDisplayName(), tr("Job done.\nTime elapsed: %1")
    //                                                                    .arg(ui->glwVisualizer->spendTime().toString("hh:mm:ss")));

    // m_timerConnection.start();
    // startUpdatingState(m_settings->queryStateTime());
}

void Communicator::onConnectionError(QString message)
{
    qDebug() << "Connection error: " << message;
}

void Communicator::onConnectionStateChanged(ConnectionState state)
{
    if (state == ConnectionState::Connected) {
        reset();
    }
}


