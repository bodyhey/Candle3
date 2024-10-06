                                                                                                                                                                                                                                                                                                                                                                                                                                        #include "communicator.h"
#include "parser/gcodepreprocessorutils.h"
#include <QMessageBox>
#include <QThread>
#include <QCoreApplication>

void Communicator::onConnectionLineReceived(QString data)
{
    assert(QThread::currentThread() == QCoreApplication::instance()->thread());
    assert(data.length() < 100);

    if (data.startsWith("[MSG:")) {
        processMessage(data);

        return;
    }

    if (m_reseting) {
        if (!dataIsReset(data)) return;
        m_reseting = false;
        stopUpdatingState();
        startUpdatingState(m_configuration->connectionModule().queryStateInterval());
    }

    if (data.isEmpty()) {
        // blank response
        return;
    }

    // Status response
    if (data[0] == '<') {
        processStatus(data);

        return;
    }

    // qDebug() << "<" << data;

    // if (dataIsReset(data)) {
    //     qDebug() << "< RST <" << data;
    // }

    if (m_commands.length() > 0 && !dataIsFloating(data) && !(m_commands[0].commandLine != "[CTRL+X]" && dataIsReset(data))) {
        processCommandResponse(data);

        return;
    }

    // Unprocessed responses
    // Handle hardware reset
    processUnhandledResponse(data);
}

void Communicator::onConnectionConnected()
{
    reset();
}

void Communicator::processFeedSpindleSpeed(QString data)
{
    static QRegExp fs("FS:([^,]*),([^,^|^>]*)");
    if (fs.indexIn(data) != -1) {
        emit feedSpindleSpeedReceived(fs.cap(1).toInt(), fs.cap(2).toInt());
    }
}

void Communicator::processOverrides(QString data)
{
    static QRegExp ov("Ov:([^,]*),([^,]*),([^,^>^|]*)");
    if (ov.indexIn(data) != -1)
    {
        int feedOverride = ov.cap(1).toInt();
        int spindleOverride = ov.cap(3).toInt();
        int rapidOverride = ov.cap(2).toInt();

        emit overridesReceived(feedOverride, spindleOverride, rapidOverride);

        // @TODO why is this here?? This shouldn't be in processOverrides.

        // Update pins state
        QString pinState;
        static QRegExp pn("Pn:([^|^>]*)");
        if (pn.indexIn(data) != -1) {
            pinState.append(QString(tr("PS: %1")).arg(pn.cap(1)));
        }

        // Process spindle state
        static QRegExp as("A:([^,^>^|]+)");
        if (as.indexIn(data) != -1) {
            QString q = as.cap(1);
            m_spindleCW = q.contains("S");
            if (q.contains("S") || q.contains("C")) {
                emit spindleStateReceived(true);
                // to spindleStateReceived handler
                // m_timerToolAnimation.start(25, this);
                // ui->cmdSpindle->setChecked(true);
            } else {
                emit spindleStateReceived(false);
                // to spindleStateReceived handler
                // m_timerToolAnimation.stop();
                // ui->cmdSpindle->setChecked(false);
            }
            emit floodStateReceived(q.contains("F"));

            if (!pinState.isEmpty()) pinState.append(" / ");
            pinState.append(QString(tr("AS: %1")).arg(as.cap(1)));
        } else {
            emit spindleStateReceived(false);
            // to spindleStateReceived handler
            // m_timerToolAnimation.stop();
            // ui->cmdSpindle->setChecked(false);
        }
        //ui->glwVisualizer->setPinState(pinState);

        emit pinStateReceived(pinState);
    }
}

void Communicator::processNewToolPosition()
{
    QVector3D toolPosition;
//    if (!(m_deviceState == DeviceCheck && m_streamer->processedCommandIndex() < m_form->currentModel().rowCount() - 1)) {
    if (!(m_deviceState == DeviceCheck && !m_streamer->isLastCommandProcessed())) {
        toolPosition = m_machinePos;
        //m_form->toolDrawer().setToolPosition(m_form->codeDrawer().getIgnoreZ() ? QVector3D(toolPosition.x(), toolPosition.y(), 0) : toolPosition);

        emit toolPositionReceived(toolPosition);
    }

    // Toolpath shadowing
    // Update tool position
    // processToolpathShadowing(toolPosition);
}

void Communicator::processWorkOffset(QString data)
{
    static QVector3D workOffset;
    static QRegExp wpx("WCO:([^,]*),([^,]*),([^,^>^|]*)");
    bool changed = false;

    if (wpx.indexIn(data) != -1) {
        QVector3D newWorkOffset(
            wpx.cap(1).toDouble(),
            wpx.cap(2).toDouble(),
            wpx.cap(3).toDouble()
        );
        changed = newWorkOffset != workOffset;
        workOffset = newWorkOffset;
    }

    // Update work coordinates
    QVector3D pos(
        m_machinePos.x() - workOffset.x(),
        m_machinePos.y() - workOffset.y(),
        m_machinePos.z() - workOffset.z()
    );
    m_workPos = pos;

    if (changed) {
        m_storedVars.setCoords("W", pos);
        emit workPosChanged(pos);
    }
}

void Communicator::processStatus(QString data)
{
    DeviceState state = DeviceUnknown;

    m_statusReceived = true;

    // Update machine coordinates
    static QRegExp mpx("MPos:([^,]*),([^,]*),([^,^>^|]*)");
    if (mpx.indexIn(data) != -1) {
        QVector3D newPos(
            mpx.cap(1).toDouble(),
            mpx.cap(2).toDouble(),
            mpx.cap(3).toDouble()
        );
        if (newPos != m_machinePos) {
            m_machinePos = newPos;
            m_storedVars.setCoords("M", newPos);
            emit machinePosChanged(newPos);
        }
    }

    // Status
    static QRegExp stx("<([^,^>^|]*)");
    if (stx.indexIn(data) != -1) {
        state = m_deviceStatesDictionary.key(stx.cap(1), DeviceUnknown);

        // Update status
        if (state != m_deviceState) {
            emit deviceStateChanged(state);
        }

        emit deviceStateReceived(state);

        // Update controls
        // moved to deviceStateReceived handler
        // ui->cmdCheck->setEnabled(state != DeviceRun && (m_senderState == SenderStopped));
        // ui->cmdCheck->setChecked(state == DeviceCheck);
        // ui->cmdHold->setChecked(state == DeviceHold0 || state == DeviceHold1 || state == DeviceQueue);
        // ui->cmdSpindle->setEnabled(state == DeviceHold0 || ((m_senderState != SenderTransferring) &&
        //                                                     (m_senderState != SenderStopping)));

        // // Update "elapsed time" timer
        // // moved to deviceStateReceived handler
        // if ((m_senderState == SenderTransferring) || (m_senderState == SenderStopping)) {
        //     QTime time(0, 0, 0);
        //     int elapsed = m_startTime.elapsed();
        //     ui->glwVisualizer->setSpendTime(time.addMSecs(elapsed));
        // }

        // Test for job complete
        if ((m_senderState == SenderStopping) &&
            ((state == DeviceIdle && m_deviceState == DeviceRun) || state == DeviceCheck))
        {
            completeTransfer();
        }

        // Abort
        static double x = sNan;
        static double y = sNan;
        static double z = sNan;

        if (m_aborting) {
            switch (state) {
                case DeviceIdle: // Idle
                    if ((m_senderState == SenderStopped) && m_resetCompleted) {
                        m_aborting = false;
                        restoreParserState();
                        restoreOffsets();
                        return;
                    }
                    break;
                case DeviceHold0: // Hold
                case DeviceHold1:
                case DeviceQueue:
                    if (!m_reseting && compareCoordinates(x, y, z)) {
                        x = sNan;
                        y = sNan;
                        z = sNan;
                        reset();
                    } else {
                        const QVector3D pos = m_machinePos;
                        x = pos.x();
                        y = pos.y();
                        z = pos.z();
                    }
                    break;
                case DeviceUnknown:
                case DeviceAlarm:
                case DeviceRun:
                case DeviceHome:
                case DeviceCheck:
                case DeviceDoor0:
                case DeviceDoor1:
                case DeviceDoor2:
                case DeviceDoor3:
                case DeviceJog:
                case DeviceSleep:
                    break;
            }
        }
    }

    processWorkOffset(data);
    processOverrides(data);
    processFeedSpindleSpeed(data);

    // Store device state
    setDeviceStateAndEmitSignal(state);

    processNewToolPosition();

    // Update continuous jog
    // @TODO jogger service??
    // m_form->jogContinuous();

    // Emit status signal
    emit statusReceived(data);
}

void Communicator::processCommandResponse(QString data)
{
    // @TODO why static?? what is this for???
    static QString response; // Full response string

    qDebug() << "< CMD <" << data;

    assert(m_commands.length() > 0);

    // Was opposite: if ((m_commands[0].commandLine != "[CTRL+X]" && dataIsEnd(data)) || (m_commands[0].commandLine == "[CTRL+X]" && dataIsReset(data))) {
    QString firstCommand = m_commands[0].commandLine;
    if ((firstCommand == "[CTRL+X]" || !dataIsEnd(data)) && (firstCommand != "[CTRL+X]" || !dataIsReset(data))) {
        response.append(data + "; ");

        return;
    }

    response.append(data);

    // Take command from buffer
    CommandAttributes commandAttributes = m_commands.takeFirst();

    QString command = GcodePreprocessorUtils::removeComment(commandAttributes.commandLine).toUpper();

    // Store current coordinate system
    if (command == "$G") {
        static QRegExp g("G5[4-9]");
        if (g.indexIn(response) != -1) {
            m_storedVars.setCS(g.cap(0));
            // @TODO how to update drawer? signal? timer?
            // m_form->machineBoundsDrawer().setOffset(
            //     QPointF(
            //         toMetric(m_storedVars.x()),
            //         toMetric(m_storedVars.y())
            //     ) + QPointF(
            //         toMetric(m_storedVars.G92x()),
            //         toMetric(m_storedVars.G92y()
            //     )
            // ));
        }
        static QRegExp t("T(\\d+)(?!\\d)");
        if (t.indexIn(response) != -1) {
            m_storedVars.setTool(g.cap(1).toInt());
        }

        // TODO: Store firmware version, features, buffer size on $I command
        // [VER:1.1d.20161014:Some string]
        // [OPT:VL,15,128]

        // Restore absolute/relative coordinate system after jog
        if (commandAttributes.tableIndex == TABLE_INDEX_UTIL1) {
            // @TODO how to handle keyboard control?? not like this!
            // if (ui->chkKeyboardControl->isChecked()) m_form->absoluteCoordinates() = response.contains("G90");
            // else if (response.contains("G90")) sendCommand(CommandSource::System, "G90", COMMAND_TI_UI);
            if (response.contains("G90")) sendCommand(CommandSource::System, "G90", TABLE_INDEX_UI);
        }

        // Process GCore parser state
        if (commandAttributes.tableIndex == TABLE_INDEX_UTIL2) {
            // @TODO what is this ; for? is it '; ok'?
            m_lastParserState = response.left(response.indexOf("; "));

            // Update status in visualizer window
            emit parserStateReceived(m_lastParserState);

            // Store parser status
            if ((m_senderState == SenderTransferring) || (m_senderState == SenderStopping)) {
                storeParserState();
            }

            // Spindle speed
            // @TODO what is the difference between this and processFeedSpindleSpeed??
            QRegExp rx(".*S([\\d\\.]+)");
            if (rx.indexIn(response) != -1) {
                double spindleSpeed = rx.cap(1).toDouble();
                emit spindleSpeedReceived(spindleSpeed);
            }

            m_updateParserState = true;
        }
    }

    // Offsets
    if (command == "$#") processOffsetsVars(response);

    // Settings response
    if (command == "$$") {
        static QRegExp gs("\\$(\\d+)\\=([^;]+)\\; ");
        QMap<int, double> rawMachineConfiguration;
        int p = 0;
        while ((p = gs.indexIn(response, p)) != -1) {
            rawMachineConfiguration[gs.cap(1).toInt()] = gs.cap(2).toDouble();
            p += gs.matchedLength();
        }

        m_machineConfiguration = new MachineConfiguration(
            rawMachineConfiguration,
            m_configuration->machineModule()
        );

        emit configurationReceived(
            *m_machineConfiguration,
            rawMachineConfiguration
        );

        // Command sent after reset
        // if (ca.tableIndex == -2) {
        //     QList<int> keys = rawMachineConfiguration.keys();
        //     if (keys.contains(13)) m_settings->setUnits(rawMachineConfiguration[13]);
        //     {...}

        //     //moved to settingsReceived signal handler
        //     //setupCoordsTextboxes();
        // }
    }

    // Homing response
    if ((command == "$H" || command == "$T") && m_homing) m_homing = false;

    // Reset complete response
    if (command == "[CTRL+X]") {
        m_resetCompleted = true;
        m_updateParserState = true;

        // Query grbl settings
        sendCommand(CommandSource::System, "$$", TABLE_INDEX_UTIL1);
        sendCommand(CommandSource::System, "$#", TABLE_INDEX_UTIL1, true);
    }

    // Clear command buffer on "M2" & "M30" command (old firmwares)
    static QRegExp M230("(M0*2|M30)(?!\\d)");
    if (command.contains(M230) && response.contains("ok") && !response.contains("Pgm End")) {
        m_commands.clear();
        m_queue.clear();

        emit aborted();
    }

    // Update probe coords on user commands
    if (command.contains("G38.2") && commandAttributes.tableIndex < 0) {
        static QRegExp PRB(".*PRB:([^,]*),([^,]*),([^,:]*)");
        if (PRB.indexIn(response) != -1) {
            m_storedVars.setCoords("PRB", QVector3D(
                PRB.cap(1).toDouble(),
                PRB.cap(2).toDouble(),
                PRB.cap(3).toDouble()
            ));
        }
    }

    // Process probing on heightmap mode only from table commands
    // @TODO refactor height map mode
    // if (uncomment.contains("G38.2") && m_form->heightMapMode() && commandAttributes.tableIndex > -1) {
    //     // Get probe Z coordinate
    //     // "[PRB:0.000,0.000,0.000:0];ok"
    //     // "[PRB:0.000,0.000,0.000,0.000:0];ok"
    //     QRegExp rx(".*PRB:([^,]*),([^,]*),([^,:]*)");
    //     double z = qQNaN();
    //     if (rx.indexIn(response) != -1) {
    //         z = toMetric(rx.cap(3).toDouble());
    //     }

    //     static double firstZ;
    //     if (m_probeIndex == -1) {
    //         firstZ = z;
    //         z = 0;
    //     } else {
    //         // Calculate delta Z
    //         z -= firstZ;

    //         // Calculate table indexes
    //         int row = (m_probeIndex / m_form->heightMapModel().columnCount());
    //         int column = m_probeIndex - row * m_form->heightMapModel().columnCount();
    //         if (row % 2) column = m_form->heightMapModel().columnCount() - 1 - column;

    //         // Store Z in table
    //         m_form->heightMapModel().setData(m_form->heightMapModel().index(row, column), z, Qt::UserRole);
    //         ui->tblHeightMap->update(m_form->heightMapModel().index(m_form->heightMapModel().rowCount() - 1 - row, column));
    //         m_form->updateHeightMapInterpolationDrawer();
    //     }

    //     m_probeIndex++;
    // }

    // Change state query time on check mode on
    if (command.contains(QRegExp("$[cC]"))) {
        m_timerStateQuery.setInterval(response.contains("Enable") ? 1000 : m_configuration->connectionModule().queryStateInterval());
    }

    // emit signal, was `Add response to console`
    commandAttributes.response = response;
    emit commandResponseReceived(commandAttributes);
    // if (tb.isValid() && tb.text() == ca.command) {
        //emit commandResponseReceived(ca, response);

        // bool scrolledDown = ui->txtConsole->verticalScrollBar()->value()
        //                     == ui->txtConsole->verticalScrollBar()->maximum();

        // removed appending to console
    // }

    // Check queue, try to send queued commands
    static bool processingQueue = false;
    if (m_queue.length() > 0 && !processingQueue) {
        processingQueue = true;
        while (m_queue.length() > 0) {
            CommandQueue command = m_queue.takeFirst();
            SendCommandResult r = sendCommand(command.source, command.commandLine, command.tableIndex, false, command.callback);
            if (r == SendDone) {
                break;
            } else if (r == SendQueue) {
                m_queue.prepend(m_queue.takeLast());
                break;
            }
        }
        processingQueue = false;
    }

    // Add response to table, send next program commands
    if (m_senderState != SenderStopped) {
        // Only if command from table
        if (commandAttributes.tableIndex > -1) {
            m_streamer->resetProcessed(commandAttributes.tableIndex);

            emit commandProcessed(commandAttributes.tableIndex, response);
        }

// Update taskbar progress
#ifdef WINDOWS
        // @TODO move progress to streamer??
        // if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        //     if (m_taskBarProgress) m_taskBarProgress->setValue(m_fileProcessedCommandIndex);
        // }
#endif
        // Process error messages
        static bool holding = false;
        static QString errors;

        if (
            commandAttributes.tableIndex > -1 &&
            response.toUpper().contains("ERROR") &&
            !m_configuration->senderModule().ignoreErrorResponses()
        ) {
            errors.append(QString::number(commandAttributes.tableIndex + 1) + ": " + commandAttributes.commandLine + " < " + response + "\n");

            // @TODO move to UI
            // m_form->senderErrorBox().setText(tr("Error message(s) received:\n") + errors);

            if (!holding) {
                holding = true;         // Hold transmit while messagebox is visible
                response.clear();

                sendRealtimeCommand(GRBL_LIVE_FEED_HOLD);
                // @TODO move to UI
                // m_form->senderErrorBox().checkBox()->setChecked(false);
                // qApp->beep();
                // int result = m_form->senderErrorBox().exec();
                int result = QMessageBox::Ignore;

                holding = false;
                errors.clear();
                // @TODO move to UI
                // if (m_form->senderErrorBox().checkBox()->isChecked()) m_settings->setIgnoreErrors(true);
                if (result == QMessageBox::Ignore) {
                    sendRealtimeCommand(GRBL_LIVE_CYCLE_START);
                } else {
                    sendRealtimeCommand(GRBL_LIVE_CYCLE_START);

                    emit aborted();
                }
            }
        }

        // Check transfer complete (last row always blank, last command row = rowcount - 2)
//        if ((m_streamer->processedCommandIndex() == m_form->currentModel().rowCount() - 2) || uncomment.contains(QRegExp("(M0*2|M30)(?!\\d)"))) {
        if (m_streamer->isLastCommandProcessed() || command.contains(QRegExp("(M0*2|M30)(?!\\d)"))) {
            if (m_deviceState == DeviceRun) {
                setSenderStateAndEmitSignal(SenderStopping);
            } else {
                completeTransfer();
            }
        } else if (m_streamer->hasMoreCommands()  // /*(m_streamer->commandIndex() < m_form->currentModel().rowCount())
                   && (m_senderState == SenderTransferring)
                   && !holding)
        {
            // Send next program commands
            sendStreamerCommandsUntilBufferIsFull();
            //m_form->sendNextFileCommands();
        }
    }

    // Tool change mode
    static QRegExp M6("(M0*6)(?!\\d)");
    if ((m_senderState == SenderPausing) && command.contains(M6)) {
        response.clear();
        
        if (m_configuration->senderModule(). pauseSenderOnToolChange()) {
            // QMessageBox::information(this, qApp->applicationDisplayName(),
            //                          tr("Change tool and press 'Pause' button to continue job"));
        }

        if (m_configuration->senderModule().useToolChangeCommands()) {
            if (m_configuration->senderModule().confirmToolChangeCommandsExecution()) {
                // QMessageBox box(this);
                // box.setIcon(QMessageBox::Information);
                // box.setText(tr("M6 command detected. Send tool change commands?\n"));
                // box.setWindowTitle(qApp->applicationDisplayName());
                // box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                // box.setCheckBox(new QCheckBox(tr("Don't show again")));
                // int res = box.exec();
                // if (box.checkBox()->isChecked()) m_settings->setToolChangeUseCommandsConfirm(false);
                // if (res == QMessageBox::Yes) {
                //     sendCommands(m_settings->toolChangeCommands());
                // }
            } else {
                sendCommands(CommandSource::ProgramAdditionalCommands, m_configuration->senderModule().toolChangeCommands());
            }
        }

        setSenderStateAndEmitSignal(SenderChangingTool);
    }

    // Pausing on button?
    if ((m_senderState == SenderPausing) && !command.contains(M6)) {
        if (m_configuration->senderModule().usePauseCommands()) {
            sendCommands(CommandSource::ProgramAdditionalCommands, m_configuration->senderModule().beforePauseCommands());
            setSenderStateAndEmitSignal(SenderPausing2);
        }
    }
    if ((m_senderState == SenderChangingTool) && !m_configuration->senderModule().pauseSenderOnToolChange()
        && m_commands.isEmpty())
    {
        setSenderStateAndEmitSignal(SenderTransferring);
    }

    // Switch to pause mode
    if (isSenderState(SenderPausing, SenderPausing2)) {
        if (m_commands.isEmpty()) {
            setSenderStateAndEmitSignal(SenderPaused);
        }
    }

    // Same as M2, Program End, turn off spindle/laser and stops the machine.
    // Scroll to first line on "M30" command
    if (command.contains("M30")) {
        // @TODO new signal here?
        emit commandProcessed(-1, "");
    }

    // Toolpath shadowing on check mode - moved to responseReceived signal handler
    // if (m_deviceState == DeviceCheck) {
    //     GcodeViewParse *parser = m_form->currentDrawer().viewParser();
    //     QList<LineSegment*> list = parser->getLineSegmentList();
    //     {...}
    // }

    // @TODO is it necessary?? does it duplicate the commandResponseReceived signal???
    // Emit response signal
    emit responseReceived(commandAttributes.commandLine, commandAttributes.tableIndex, response);

    response.clear();
}

void Communicator::processUnhandledResponse(QString data)
{
    if (dataIsReset(data)) {
        // Welcome message, hardware reset occurred?
        this->processWelcomeMessageDetected();
    }

    // @TODO do we want to log it here?
    //m_form->partConsole().append(data);
}

void Communicator::processWelcomeMessageDetected()
{
    setSenderStateAndEmitSignal(SenderStopped);
    setDeviceStateAndEmitSignal(DeviceUnknown);

    m_streamer->reset();
    //m_form->fileCommandIndex() = 0;

    m_reseting = false;
    m_homing = false;

    m_updateParserState = true;
    m_statusReceived = true;

    clearCommandsAndQueue();

    sendCommand(CommandSource::System, "$$", TABLE_INDEX_UTIL1);
    sendCommand(CommandSource::System, "$#", TABLE_INDEX_UTIL1, true);

    // @TODO moved to senderStateReceived handler, is it too soon??
    // m_form->updateControlsState();

}

void Communicator::processMessage(QString data)
{
    qDebug() << "< MSG <" << data;
    // static QRegExp msg("\\[MSG:([^\\]]+)\\]");
    // if (msg.indexIn(data) != -1) {
    //     QString message = msg.cap(1);
    //     if (message.contains("Enabled")) {
    //         m_statusReceived = true;
    //         startUpdatingState();
    //     } else if (message.contains("Disabled")) {
    //         stopUpdatingState();
    //     }
    // }
}
