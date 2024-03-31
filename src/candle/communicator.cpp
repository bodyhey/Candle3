#include "communicator.h"
#include "globals.h"
#include <QVector3D>
#include <QDebug>
#include <QCheckBox>
#include <QMessageBox>
#include <QTextCursor>
#include <parser/gcodeviewparse.h>

Communicator::Communicator(
    Connection *connection,
    TempConnectionToUiProxy *frmMain,
    Configuration *configuration,
    Ui::frmMain *ui,
    frmSettings *frmSettings,
    QObject *parent = nullptr
    ) : QObject(parent),
        m_connection(connection),
        m_settings(frmSettings),
        m_configuration(configuration),
        ui(ui),
        m_form(frmMain),
        m_timerStateQuery(this)
{
    assert(m_connection != nullptr);
    assert(m_settings != nullptr);

    m_reseting = false;
    m_resetCompleted = true;
    m_aborting = false;
    m_statusReceived = false;
    m_spindleCW = true;
    // temporary !!!
    m_communicator = this;

    m_deviceState = DeviceUnknown;
    m_senderState = SenderUnknown;

    m_machinePos = QVector3D(0, 0, 0);
    m_workPos = QVector3D(0, 0, 0);

    // this->connect(m_connection, SIGNAL(error(QString)), this, SLOT(onConnectionError(QString)));
    connect(m_connection, SIGNAL(lineReceived(QString)), this, SLOT(onConnectionLineReceived(QString)));

    setSenderStateAndEmitSignal(SenderStopped);

    // Update state timer
    connect(&m_timerStateQuery, SIGNAL(timeout()), this, SLOT(onTimerStateQuery()));
    m_timerStateQuery.start();
}

/**
 * @param tableIndex -1 - ui commands, -2 - utility commands, -3 - utility commands
 */
SendCommandResult Communicator::sendCommand(QString command, int tableIndex, bool showInConsole, bool wait)
{
    // tableIndex:
    // 0...n - commands from g-code program
    // -1 - ui commands
    // -2 - utility commands
    // -3 - utility commands

    if (!m_connection->isConnected() || !m_resetCompleted) return SendDone;

    // Check command
    if (command.isEmpty()) return SendEmpty;

    // Place to queue on 'wait' flag
    if (wait) {
        m_queue.append(CommandQueue(command, tableIndex, showInConsole));

        return SendQueue;
    }

    // Evaluate scripts in command
    // @todo scripting??
    //if (tableIndex < 0) command = evaluateCommand(command);

    // Check evaluated command
    if (command.isEmpty()) return SendEmpty;

    // Place to queue if command buffer is full
    if ((bufferLength() + command.length() + 1) > BUFFERLENGTH) {
        m_queue.append(CommandQueue(command, tableIndex, showInConsole));

        return SendQueue;
    }

    command = command.toUpper();

    //int length_, int consoleIndex_, int commandIndex_, int tableIndex_, QString command_
    CommandAttributes commandAttributes(
        command.length() + 1,
        -1,
        m_commandIndex++,
        tableIndex,
        command
    );
    // if (showInConsole) {
    //     // @todo ui
    //     // writeConsole(command);
    //     commandAttributes.consoleIndex = -1; //ui->txtConsole->blockCount() - 1;
    // } else {
    //     commandAttributes.consoleIndex = -1;
    // }

    // commandAttributes.command = command;
    // commandAttributes.commandIndex = m_commandIndex++;
    // commandAttributes.length = command.length() + 1;
    // commandAttributes.tableIndex = tableIndex;

    m_commands.append(commandAttributes);

    QString uncomment = GcodePreprocessorUtils::removeComment(command);

    // Processing spindle speed only from g-code program
    static QRegExp s("[Ss]0*(\\d+)");
    if (s.indexIn(uncomment) != -1 && commandAttributes.tableIndex > -2) {
        int speed = s.cap(1).toInt();
        if (ui->slbSpindle->value() != speed) {
            ui->slbSpindle->setValue(speed);
        }
    }

    // Set M2 & M30 commands sent flag
    static QRegExp M230("(M0*2|M30|M0*6|M25)(?!\\d)");
    static QRegExp M6("(M0*6)(?!\\d)");
    if ((m_senderState == SenderTransferring) && uncomment.contains(M230)) {
        if (!uncomment.contains(M6) || m_settings->toolChangeUseCommands() || m_settings->toolChangePause()) setSenderStateAndEmitSignal(SenderPausing);
    }

    // Queue offsets request on G92, G10 commands
    static QRegExp G92("(G92|G10)(?!\\d)");
    if (uncomment.contains(G92)) sendCommand("$#", COMMAND_TI_UTIL2, showInConsole, true);

    m_connection->sendLine(command);

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

void Communicator::sendCommands(QString commands, int tableIndex)
{
    QStringList list = commands.split("\n");

    bool q = false;
    foreach (QString cmd, list) {
        SendCommandResult r = sendCommand(cmd.trimmed(), tableIndex, m_settings->showUICommands(), q);
        if (r == SendDone || r == SendQueue) q = true;
    }
}

bool Communicator::streamCommands(Streamer *streamer)
{
    // if (cannot be streamed) {
    //     return false;
    // }

    m_streamer = streamer;
    //startStreaming();

    return true;
}

void Communicator::clearCommandsAndQueue()
{
    m_commands.clear();
    clearQueue();
}

void Communicator::clearQueue()
{
    m_queue.clear();
}

void Communicator::reset()
{
    m_connection->sendByteArray(QByteArray(1, GRBL_LIVE_SOFT_RESET));

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
    CommandAttributes ca;
    ca.command = "[CTRL+X]";
    // @todo ui
    // if (m_settings->showUICommands()) ui->txtConsole->appendPlainText(ca.command);
    // ca.consoleIndex = m_settings->showUICommands() ? ui->txtConsole->blockCount() - 1 : -1;
    ca.consoleIndex = -1;
    ca.tableIndex = -1;
    ca.length = ca.command.length() + 1;
    m_commands.append(ca);
}

void Communicator::abort()
{
    if ((m_communicator->senderState() == SenderPaused) || (m_communicator->senderState() == SenderChangingTool)) {
        m_communicator->sendCommand("M2", COMMAND_TI_UI, m_configuration->consoleModule().showUiCommands(), false);
    } else {
        m_communicator->sendCommand("M2", COMMAND_TI_UI, m_configuration->consoleModule().showUiCommands(), true);
    }
}

/*
* @todo make sure we can replace connection at this point!!
*/
void Communicator::replaceConnection(Connection *newConnection)
{
    if (m_connection == newConnection) return;
    disconnect(m_connection, SIGNAL(lineReceived(QString)), this, SLOT(onConnectionLineReceived(QString)));
    m_connection->disconnect();
    m_connection = newConnection;
    connect(m_connection, SIGNAL(lineReceived(QString)), this, SLOT(onConnectionLineReceived(QString)));
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
    // sendCommand(QString("%4G53G90X%1Y%2Z%3").arg(ui->txtMPosX->value())
    //                 .arg(ui->txtMPosY->value())
    //                 .arg(ui->txtMPosZ->value())
    //                 .arg(m_settings->units() ? "G20" : "G21"),
    //             COMMAND_TI_UTIL1, m_settings->showUICommands());

    // sendCommand(QString("%4G92X%1Y%2Z%3").arg(ui->txtWPosX->value())
    //                 .arg(ui->txtWPosY->value())
    //                 .arg(ui->txtWPosZ->value())
    //                 .arg(m_settings->units() ? "G20" : "G21"),
    //             COMMAND_TI_UTIL1, m_settings->showUICommands());
}

void Communicator::setSenderStateAndEmitSignal(SenderState state)
{
    if (m_senderState != state) {
        m_senderState = state;
        emit senderStateChanged(state);
    }
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

    QString command = m_form->currentModel().data(m_form->currentModel().index(m_streamer->commandIndex(), 1)).toString();
    static QRegExp M230("(M0*2|M30|M0*6)(?!\\d)");

    while ((bufferLength() + command.length() + 1) <= BUFFERLENGTH
           && m_streamer->commandIndex() < m_form->currentModel().rowCount() - 1
           && !(!m_commands.isEmpty() && GcodePreprocessorUtils::removeComment(m_commands.last().command).contains(M230))
    ) {
        m_form->currentModel().setData(m_form->currentModel().index(m_streamer->commandIndex(), 2), GCodeItem::Sent);
        sendCommand(command, m_streamer->commandIndex(), m_configuration->consoleModule().showProgramCommands());
        m_streamer->advanceCommandIndex();
        command = m_form->currentModel().data(m_form->currentModel().index(m_streamer->commandIndex(), 1)).toString();
    }
}

/* used by scripting engine only?? emit signal and do not use m_storedVars directly */
void Communicator::storeOffsetsVars(QString response)
{
    static QRegExp gx("\\[(G5[4-9]|G28|G30|G92|PRB):([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)");
    static QRegExp tx("\\[(TLO):([\\d\\.\\-]+)");

    // int p = 0;
    // while ((p = gx.indexIn(response, p)) != -1) {
    //     m_storedVars.setCoords(gx.cap(1), QVector3D(
    //                                           gx.cap(2).toDouble(),
    //                                           gx.cap(3).toDouble(),
    //                                           gx.cap(4).toDouble()
    //                                           ));

    //     p += gx.matchedLength();
    // }

    // if (tx.indexIn(response) != -1) {
    //     m_storedVars.setCoords(tx.cap(1), QVector3D(
    //                                           0,
    //                                           0,
    //                                           tx.cap(2).toDouble()
    //                                           ));
    // }
}

bool Communicator::dataIsFloating(QString data)
{
    QStringList ends;

    ends << "Reset to continue";
    ends << "'$H'|'$X' to unlock";
    ends << "ALARM: Soft limit";
    ends << "ALARM: Hard limit";
    ends << "Check Door";

    foreach (QString str, ends) {
        if (data.contains(str)) return true;
    }

    return false;
}

bool Communicator::dataIsEnd(QString data)
{
    QStringList ends;

    ends << "ok";
    ends << "error";

    foreach (QString str, ends) {
        if (data.contains(str)) return true;
    }

    return false;
}

void Communicator::onTimerStateQuery()
{
    // qDebug() << m_connection->isConnected() << m_resetCompleted << m_statusReceived;
    if (m_connection->isConnected() && m_resetCompleted && m_statusReceived) {
        m_connection->sendByteArray(QByteArray(1, '?'));
        m_statusReceived = false;
    }

    // @todo find some other way to update buffer state
    //ui->glwVisualizer->setBufferState(QString(tr("Buffer: %1 / %2 / %3")).arg(bufferLength()).arg(m_communicator->m_commands.length()).arg(m_communicator->m_queue.length()));
}

// detects first line of communication?
bool Communicator::dataIsReset(QString data)
{
    // "GRBL" in either case, optionally followed by a number of non-whitespace characters,
    // followed by a version number in the format x.y.
    // This matches e.g.
    // Grbl 1.1h ['$' for help]
    // GrblHAL 1.1f ['$' or '' for help]
    // Grbl 1.8 [uCNC v1.8.8 '$' for help]
    // Gcarvin ?? https://github.com/inventables/gCarvin

    return QRegExp("^(GRBL|GCARVIN)\\s\\d\\.\\d.").indexIn(data.toUpper()) != -1;
}

bool Communicator::compareCoordinates(double x, double y, double z)
{
    return m_machinePos.x() == x && m_machinePos.y() == y && m_machinePos.z() == z;
}

double Communicator::toMetric(double value)
{
    return m_settings->units() == 0 ? value : value * 25.4;
}

double Communicator::toInches(double value)
{
    return m_settings->units() == 0 ? value : value / 25.4;
}
