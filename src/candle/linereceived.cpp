#include "communicator.h"
#include "parser/gcodeviewparse.h"
#include <QMessageBox>

/*
to be refactored/replaced by signals and slots

updateControlsState()

m_deviceStatuses // dist, move to UI
m_toolDrawer // emit toolPositionChanged
m_codeDrawer // getIgnoreZ??
m_lastDrawnLineIndex
m_currentDrawer // code drawer/probe drawer ??
m_updateParserStatus
m_storedVars
m_taskBarProgress
m_senderErrorBox
m_machineBoundsDrawer
m_absoluteCoordinates

restoreParserState();
updateOverride(ui->slbFeedOverride, ov.cap(1).toInt(), '\x91');
parser
list ???
jogContinuous();
storeOffsetsVars(response)
setupCoordsTextboxes


ui->slbFeedOverride
ui->slbSpindleOverride
ui->slbSpindle
ui->slbRapidOverride->isChecked() ? ui->slbRapidOverride
ui->glwVisualizer
ui->chkKeyboardControl
ui->chkAutoScroll
ui->tblProgram
qApp->beep()

// transfering file, streamer class?
m_fileProcessedCommandIndex
fileProcessedCommandIndex
currentModel
completeTransfer();

// height map
m_heightMapMode
updateHeightMapInterpolationDrawer
ui->tblHeightMap

*/

void Communicator::onConnectionLineReceived(QString data)
{
    assert(QThread::currentThread() == QCoreApplication::instance()->thread());
    assert(data.length() < 100);

    if (data.startsWith("[MSG:")) {
        processMessage(data);

        return;
    }

    if (m_communicator->m_reseting) {
        if (!m_communicator->dataIsReset(data)) return;
        m_communicator->m_reseting = false;
        m_communicator->stopUpdatingState();
        m_communicator->startUpdatingState(m_settings->queryStateTime());
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

    // if (m_communicator->dataIsReset(data)) {
    //     qDebug() << "< RST <" << data;
    // }

    if (m_commands.length() > 0 && !dataIsFloating(data) && !(m_commands[0].command != "[CTRL+X]" && dataIsReset(data))) {
        processCommandResponse(data);

        return;
    }

    // Unprocessed responses
    // Handle hardware reset
    processUnhandledResponse(data);
}

void Communicator::onConnectionError(QString message)
{
    qDebug() << "Connection error: " << message;
}

void Communicator::processFeedSpindleSpeed(QString data)
{
    static QRegExp fs("FS:([^,]*),([^,^|^>]*)");
    if (fs.indexIn(data) != -1) {
        ui->glwVisualizer->setSpeedState((QString(tr("F/S: %1 / %2")).arg(fs.cap(1)).arg(fs.cap(2))));
    }
}

void Communicator::processOverrides(QString data)
{
    static QRegExp ov("Ov:([^,]*),([^,]*),([^,^>^|]*)");
    if (ov.indexIn(data) != -1)
    {
        m_form->updateOverride(ui->slbFeedOverride, ov.cap(1).toInt(), '\x91');
        m_form->updateOverride(ui->slbSpindleOverride, ov.cap(3).toInt(), '\x9a');

        int rapid = ov.cap(2).toInt();
        ui->slbRapidOverride->setCurrentValue(rapid);

        int target = ui->slbRapidOverride->isChecked() ? ui->slbRapidOverride->value() : 100;

        if (rapid != target) switch (target) {
                case 25:
                    m_communicator->sendRealtimeCommand(GRBL_LIVE_RAPID_FULL_RATE);
                    break;
                case 50:
                    m_communicator->sendRealtimeCommand(GRBL_LIVE_RAPID_HALF_RATE);
                    break;
                case 100:
                    m_communicator->sendRealtimeCommand(GRBL_LIVE_RAPID_QUARTER_RATE);
                    break;
            }

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
            m_communicator->m_spindleCW = q.contains("S");
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
        ui->glwVisualizer->setPinState(pinState);
    }
}

void Communicator::processToolpathShadowing(DeviceState state, QVector3D toolPosition)
{
    if (((m_communicator->m_senderState == SenderTransferring) || (m_communicator->m_senderState == SenderStopping)
         || (m_communicator->m_senderState == SenderPausing) || (m_communicator->m_senderState == SenderPausing2) || (m_communicator->m_senderState == SenderPaused)) && state != DeviceCheck) {
        GcodeViewParse *parser = m_form->currentDrawer().viewParser();

        bool toolOntoolpath = false;

        QList<int> drawnLines;
        QList<LineSegment*> list = parser->getLineSegmentList();

        for (int i = m_form->lastDrawnLineIndex(); i < list.count()
                                                   && list.at(i)->getLineNumber()
                                                          <= (m_form->currentModel().data(m_form->currentModel().index(m_streamer->processedCommandIndex(), 4)).toInt() + 1); i++) {
            if (list.at(i)->contains(toolPosition)) {
                toolOntoolpath = true;
                m_form->lastDrawnLineIndex() = i;
                break;
            }
            drawnLines << i;
        }

        if (toolOntoolpath) {
            foreach (int i, drawnLines) {
                list.at(i)->setDrawn(true);
            }
            if (!drawnLines.isEmpty()) m_form->currentDrawer().update(drawnLines);
        }
    }
}

void Communicator::processNewToolPosition(DeviceState state)
{
    QVector3D toolPosition;
    if (!(state == DeviceCheck && m_streamer->processedCommandIndex() < m_form->currentModel().rowCount() - 1)) {
        toolPosition = m_communicator->m_machinePos;
        m_form->toolDrawer().setToolPosition(m_form->codeDrawer().getIgnoreZ() ? QVector3D(toolPosition.x(), toolPosition.y(), 0) : toolPosition);
    }

    // Toolpath shadowing
    // Update tool position
    processToolpathShadowing(state, toolPosition);
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
        m_communicator->m_machinePos.x() - workOffset.x(),
        m_communicator->m_machinePos.y() - workOffset.y(),
        m_communicator->m_machinePos.z() - workOffset.z()
    );
    m_communicator->m_workPos = pos;

    if (changed) {
        emit workPosChanged(pos);
    }
}

void Communicator::processStatus(QString data)
{
    DeviceState state = DeviceUnknown;

    m_communicator->m_statusReceived = true;

    // Update machine coordinates
    static QRegExp mpx("MPos:([^,]*),([^,]*),([^,^>^|]*)");
    if (mpx.indexIn(data) != -1) {
        QVector3D newPos(
            mpx.cap(1).toDouble(),
            mpx.cap(2).toDouble(),
            mpx.cap(3).toDouble()
        );
        bool changed = newPos != m_communicator->m_machinePos;
        if (changed) {
            m_communicator->m_machinePos = newPos;
            emit machinePosChanged(newPos);
        }
    }

    // Status
    static QRegExp stx("<([^,^>^|]*)");
    if (stx.indexIn(data) != -1) {
        state = m_form->deviceStatuses().key(stx.cap(1), DeviceUnknown);

        // Update status
        if (state != m_communicator->m_deviceState) {
            emit deviceStateChanged(state);
        }

        emit deviceStateReceived(state);

        // Update controls
        // moved to deviceStateReceived handler
        // ui->cmdCheck->setEnabled(state != DeviceRun && (m_communicator->m_senderState == SenderStopped));
        // ui->cmdCheck->setChecked(state == DeviceCheck);
        // ui->cmdHold->setChecked(state == DeviceHold0 || state == DeviceHold1 || state == DeviceQueue);
        // ui->cmdSpindle->setEnabled(state == DeviceHold0 || ((m_communicator->m_senderState != SenderTransferring) &&
        //                                                     (m_communicator->m_senderState != SenderStopping)));

        // // Update "elapsed time" timer
        // // moved to deviceStateReceived handler
        // if ((m_communicator->m_senderState == SenderTransferring) || (m_communicator->m_senderState == SenderStopping)) {
        //     QTime time(0, 0, 0);
        //     int elapsed = m_startTime.elapsed();
        //     ui->glwVisualizer->setSpendTime(time.addMSecs(elapsed));
        // }

        // Test for job complete
        if ((m_communicator->m_senderState == SenderStopping) &&
            ((state == DeviceIdle && m_communicator->m_deviceState == DeviceRun) || state == DeviceCheck))
        {
            m_form->completeTransfer();
        }

        // Abort
        static double x = sNan;
        static double y = sNan;
        static double z = sNan;

        if (m_communicator->m_aborting) {
            switch (state) {
                case DeviceIdle: // Idle
                    if ((m_communicator->m_senderState == SenderStopped) && m_communicator->m_resetCompleted) {
                        m_communicator->m_aborting = false;
                        restoreParserState();
                        m_communicator->restoreOffsets();
                        return;
                    }
                    break;
                case DeviceHold0: // Hold
                case DeviceHold1:
                case DeviceQueue:
                    if (!m_communicator->m_reseting && m_communicator->compareCoordinates(x, y, z)) {
                        x = sNan;
                        y = sNan;
                        z = sNan;
                        m_form->grblReset();
                    } else {
                        const QVector3D pos = m_communicator->m_machinePos;
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
    processNewToolPosition(state);
    processOverrides(data);
    processFeedSpindleSpeed(data);

    // Store device state
    m_communicator->setDeviceStateAndEmitSignal(state);

    // Update continuous jog
    m_form->jogContinuous();

    // Emit status signal
    emit statusReceived(data);
}

void Communicator::processCommandResponse(QString data)
{
    // @TODO why static?? what is this for???
    static QString response; // Full response string

    // Was opposite: if ((m_commands[0].command != "[CTRL+X]" && dataIsEnd(data)) || (m_commands[0].command == "[CTRL+X]" && dataIsReset(data))) {
    QString firstCommand = m_commands[0].command;
    if ((firstCommand == "[CTRL+X]" || !dataIsEnd(data)) && (firstCommand != "[CTRL+X]" || !dataIsReset(data))) {
        response.append(data + "; ");

        return;
    }

    response.append(data);

    // Take command from buffer
    CommandAttributes commandAttributes = m_communicator->m_commands.takeFirst();
    //QTextBlock tb = ui->txtConsole->document()->findBlockByNumber(ca.consoleIndex);
    //QTextCursor tc(tb);

    QString uncomment = GcodePreprocessorUtils::removeComment(commandAttributes.command).toUpper();

    // Store current coordinate system
    if (uncomment == "$G") {
        static QRegExp g("G5[4-9]");
        if (g.indexIn(response) != -1) {
            m_form->storedVars().setCS(g.cap(0));
            m_form->machineBoundsDrawer().setOffset(QPointF(m_communicator->toMetric(m_form->storedVars().x()), m_communicator->toMetric(m_form->storedVars().y())) +
                                            QPointF(m_communicator->toMetric(m_form->storedVars().G92x()), m_communicator->toMetric(m_form->storedVars().G92y())));
        }
        static QRegExp t("T(\\d+)(?!\\d)");
        if (t.indexIn(response) != -1) {
            m_form->storedVars().setTool(g.cap(1).toInt());
        }
    }

    // TODO: Store firmware version, features, buffer size on $I command
    // [VER:1.1d.20161014:Some string]
    // [OPT:VL,15,128]

    // Restore absolute/relative coordinate system after jog
    if (uncomment == "$G" && commandAttributes.tableIndex == -2) {
        if (ui->chkKeyboardControl->isChecked()) m_form->absoluteCoordinates() = response.contains("G90");
        else if (response.contains("G90")) m_communicator->sendCommand(CommandSource::System, "G90", COMMAND_TI_UI);
    }

    // Process GCore parser state
    if (uncomment == "$G" && commandAttributes.tableIndex == -3) {
        // @TODO what is this ; for? is it '; ok'?
        m_lastParserState = response.left(response.indexOf("; "));

        // Update status in visualizer window
        emit parserStateReceived(m_lastParserState);

        // Store parser status
        if ((m_senderState == SenderTransferring) || (m_senderState == SenderStopping)) {
            storeParserState();
        }

        // Spindle speed
        QRegExp rx(".*S([\\d\\.]+)");
        if (rx.indexIn(response) != -1) {
            double speed = rx.cap(1).toDouble();
            ui->slbSpindle->setCurrentValue(speed);
        }

        m_form->updateParserStatus() = true;
    }

    // Offsets
    if (uncomment == "$#") processOffsetsVars(response);

    // Settings response
    if (uncomment == "$$") {
        static QRegExp gs("\\$(\\d+)\\=([^;]+)\\; ");
        QMap<int, double> rawMachineConfiguration;
        int p = 0;
        while ((p = gs.indexIn(response, p)) != -1) {
            rawMachineConfiguration[gs.cap(1).toInt()] = gs.cap(2).toDouble();
            p += gs.matchedLength();
        }

        MachineConfiguration machineConfiguration(rawMachineConfiguration);

        emit m_communicator->configurationReceived(
            machineConfiguration,
            rawMachineConfiguration
        );

        // Command sent after reset
        // if (ca.tableIndex == -2) {
        //     QList<int> keys = rawMachineConfiguration.keys();
        //     if (keys.contains(13)) m_settings->setUnits(rawMachineConfiguration[13]);
        //     if (keys.contains(20)) m_settings->setSoftLimitsEnabled(rawMachineConfiguration[20]);
        //     if (keys.contains(22)) {
        //         m_settings->setHomingEnabled(rawMachineConfiguration[22]);
        //         m_machineBoundsDrawer.setVisible(rawMachineConfiguration[22]);
        //     }
        //     if (keys.contains(110)) m_settings->setRapidSpeed(rawMachineConfiguration[110]);
        //     if (keys.contains(120)) m_settings->setAcceleration(rawMachineConfiguration[120]);
        //     if (keys.contains(130) && keys.contains(131) && keys.contains(132)) {
        //         QVector3D bounds = QVector3D(
        //             m_settings->referenceXPlus() ? -rawMachineConfiguration[130] : rawMachineConfiguration[130],
        //             m_settings->referenceYPlus() ? -rawMachineConfiguration[131] : rawMachineConfiguration[131],
        //             m_settings->referenceZPlus() ? -rawMachineConfiguration[132] : rawMachineConfiguration[132]
        //         );
        //         m_settings->setMachineBounds(bounds);
        //         m_machineBoundsDrawer.setBorderRect(QRectF(0, 0,
        //                                                    m_settings->referenceXPlus() ? -rawMachineConfiguration[130] : rawMachineConfiguration[130],
        //                                                    m_settings->referenceYPlus() ? -rawMachineConfiguration[131] : rawMachineConfiguration[131]));
        //         qDebug() << "Machine bounds (3-axis)" << bounds;
        //     } else if (keys.contains(130) && keys.contains(131)) {
        //         // 2 axis
        //         QVector3D bounds = QVector3D(
        //             m_settings->referenceXPlus() ? -rawMachineConfiguration[130] : rawMachineConfiguration[130],
        //             m_settings->referenceYPlus() ? -rawMachineConfiguration[131] : rawMachineConfiguration[131],
        //             0
        //         );
        //         m_settings->setMachineBounds(QVector3D(
        //             m_settings->referenceXPlus() ? -rawMachineConfiguration[130] : rawMachineConfiguration[130],
        //             m_settings->referenceYPlus() ? -rawMachineConfiguration[131] : rawMachineConfiguration[131],
        //             0
        //         ));
        //         m_machineBoundsDrawer.setBorderRect(QRectF(0, 0,
        //             m_settings->referenceXPlus() ? -rawMachineConfiguration[130] : rawMachineConfiguration[130],
        //             m_settings->referenceYPlus() ? -rawMachineConfiguration[131] : rawMachineConfiguration[131]
        //         ));
        //         qDebug() << "Machine bounds (2-axis)" << bounds;
        //     }

        //     //moved to settingsReceived signal handler
        //     //setupCoordsTextboxes();
        // }
    }

    // Homing response
    if ((uncomment == "$H" || uncomment == "$T") && m_communicator->m_homing) m_communicator->m_homing = false;

    // Reset complete response
    if (uncomment == "[CTRL+X]") {
        m_communicator->m_resetCompleted = true;
        m_form->updateParserStatus() = true;

        // Query grbl settings
        m_communicator->sendCommand(CommandSource::System, "$$", COMMAND_TI_UTIL1);
        m_communicator->sendCommand(CommandSource::System, "$#", COMMAND_TI_UTIL1, true);
    }

    // Clear command buffer on "M2" & "M30" command (old firmwares)
    static QRegExp M230("(M0*2|M30)(?!\\d)");
    if (uncomment.contains(M230) && response.contains("ok") && !response.contains("Pgm End")) {
        m_communicator->m_commands.clear();
        m_communicator->m_queue.clear();

        emit aborted();
    }

    // Update probe coords on user commands
    if (uncomment.contains("G38.2") && commandAttributes.tableIndex < 0) {
        static QRegExp PRB(".*PRB:([^,]*),([^,]*),([^,:]*)");
        if (PRB.indexIn(response) != -1) {
            m_form->storedVars().setCoords("PRB", QVector3D(
                                              PRB.cap(1).toDouble(),
                                              PRB.cap(2).toDouble(),
                                              PRB.cap(3).toDouble()
                                              ));
        }
    }

    // Process probing on heightmap mode only from table commands
    if (uncomment.contains("G38.2") && m_form->heightMapMode() && commandAttributes.tableIndex > -1) {
        // Get probe Z coordinate
        // "[PRB:0.000,0.000,0.000:0];ok"
        // "[PRB:0.000,0.000,0.000,0.000:0];ok"
        QRegExp rx(".*PRB:([^,]*),([^,]*),([^,:]*)");
        double z = qQNaN();
        if (rx.indexIn(response) != -1) {
            z = m_communicator->toMetric(rx.cap(3).toDouble());
        }

        static double firstZ;
        if (m_communicator->m_probeIndex == -1) {
            firstZ = z;
            z = 0;
        } else {
            // Calculate delta Z
            z -= firstZ;

            // Calculate table indexes
            int row = (m_communicator->m_probeIndex / m_form->heightMapModel().columnCount());
            int column = m_communicator->m_probeIndex - row * m_form->heightMapModel().columnCount();
            if (row % 2) column = m_form->heightMapModel().columnCount() - 1 - column;

            // Store Z in table
            m_form->heightMapModel().setData(m_form->heightMapModel().index(row, column), z, Qt::UserRole);
            ui->tblHeightMap->update(m_form->heightMapModel().index(m_form->heightMapModel().rowCount() - 1 - row, column));
            m_form->updateHeightMapInterpolationDrawer();
        }

        m_communicator->m_probeIndex++;
    }

    // Change state query time on check mode on
    if (uncomment.contains(QRegExp("$[cC]"))) {
        m_communicator->m_timerStateQuery.setInterval(response.contains("Enable") ? 1000 : m_settings->queryStateTime());
    }

    // emit signal, was `Add response to console`
    commandAttributes.response = response;
    emit commandResponseReceived(commandAttributes);
    // if (tb.isValid() && tb.text() == ca.command) {
        //emit commandResponseReceived(ca, response);

        // bool scrolledDown = ui->txtConsole->verticalScrollBar()->value()
        //                     == ui->txtConsole->verticalScrollBar()->maximum();

        // // Update text block numbers
        // int blocksAdded = response.count("; ");

        // if (blocksAdded > 0) for (int i = 0; i < m_communicator->m_commands.count(); i++) {
        //         if (m_communicator->m_commands[i].consoleIndex != -1) m_communicator->m_commands[i].consoleIndex += blocksAdded;
        //     }

        // tc.beginEditBlock();
        // tc.movePosition(QTextCursor::EndOfBlock);

        // // @TODO response is added as multiple lines, do we have to do this?
        // tc.insertText(" < " + QString(response).replace("; ", "\r\n"));
        // tc.endEditBlock();

        // if (scrolledDown) ui->txtConsole->verticalScrollBar()->setValue(
        //         ui->txtConsole->verticalScrollBar()->maximum());
    // }

    // Check queue
    static bool processingQueue = false;
    if (m_communicator->m_queue.length() > 0 && !processingQueue) {
        processingQueue = true;
        while (m_communicator->m_queue.length() > 0) {
            CommandQueue cq = m_communicator->m_queue.takeFirst();
            SendCommandResult r = m_communicator->sendCommand(cq.source, cq.command, cq.tableIndex);
            if (r == SendDone) {
                break;
            } else if (r == SendQueue) {
                m_communicator->m_queue.prepend(m_communicator->m_queue.takeLast());
                break;
            }
        }
        processingQueue = false;
    }

    // Add response to table, send next program commands
    if (m_communicator->m_senderState != SenderStopped) {
        // Only if command from table
        if (commandAttributes.tableIndex > -1) {
            m_form->currentModel().setData(m_form->currentModel().index(commandAttributes.tableIndex, 2), GCodeItem::Processed);
            m_form->currentModel().setData(m_form->currentModel().index(commandAttributes.tableIndex, 3), response);

            m_streamer->resetProcessed(commandAttributes.tableIndex);

            if (ui->chkAutoScroll->isChecked() && commandAttributes.tableIndex != -1) {
                ui->tblProgram->scrollTo(m_form->currentModel().index(commandAttributes.tableIndex + 1, 0));      // TODO: Update by timer
                ui->tblProgram->setCurrentIndex(m_form->currentModel().index(commandAttributes.tableIndex, 1));
            }
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

        if (commandAttributes.tableIndex > -1 && response.toUpper().contains("ERROR") && !m_settings->ignoreErrors()) {
            errors.append(QString::number(commandAttributes.tableIndex + 1) + ": " + commandAttributes.command + " < " + response + "\n");

            m_form->senderErrorBox().setText(tr("Error message(s) received:\n") + errors);

            if (!holding) {
                holding = true;         // Hold transmit while messagebox is visible
                response.clear();

                m_communicator->sendRealtimeCommand("!");
                m_form->senderErrorBox().checkBox()->setChecked(false);
                qApp->beep();
                int result = m_form->senderErrorBox().exec();

                holding = false;
                errors.clear();
                if (m_form->senderErrorBox().checkBox()->isChecked()) m_settings->setIgnoreErrors(true);
                if (result == QMessageBox::Ignore) {
                    m_communicator->sendRealtimeCommand("~");
                } else {
                    m_communicator->sendRealtimeCommand("~");

                    emit aborted();
                }
            }
        }

        // Check transfer complete (last row always blank, last command row = rowcount - 2)
        if ((m_streamer->processedCommandIndex() == m_form->currentModel().rowCount() - 2) || uncomment.contains(QRegExp("(M0*2|M30)(?!\\d)"))) {
            if (m_communicator->m_deviceState == DeviceRun) {
                m_communicator->setSenderStateAndEmitSignal(SenderStopping);
            } else {
                m_form->completeTransfer();
            }
        } else if ((m_streamer->commandIndex() < m_form->currentModel().rowCount())
                   && (m_communicator->m_senderState == SenderTransferring)
                   && !holding)
        {
            // Send next program commands
            sendStreamerCommandsUntilBufferIsFull();
            //m_form->sendNextFileCommands();
        }
    }

    // Tool change mode
    static QRegExp M6("(M0*6)(?!\\d)");
    if ((m_communicator->m_senderState == SenderPausing) && uncomment.contains(M6)) {
        response.clear();

        if (m_settings->toolChangePause()) {
            // QMessageBox::information(this, qApp->applicationDisplayName(),
            //                          tr("Change tool and press 'Pause' button to continue job"));
        }

        if (m_settings->toolChangeUseCommands()) {
            if (m_settings->toolChangeUseCommandsConfirm()) {
                // QMessageBox box(this);
                // box.setIcon(QMessageBox::Information);
                // box.setText(tr("M6 command detected. Send tool change commands?\n"));
                // box.setWindowTitle(qApp->applicationDisplayName());
                // box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                // box.setCheckBox(new QCheckBox(tr("Don't show again")));
                // int res = box.exec();
                // if (box.checkBox()->isChecked()) m_settings->setToolChangeUseCommandsConfirm(false);
                // if (res == QMessageBox::Yes) {
                //     m_communicator->sendCommands(m_settings->toolChangeCommands());
                // }
            } else {
                m_communicator->sendCommands(CommandSource::ProgramAdditionalCommands, m_settings->toolChangeCommands());
            }
        }

        m_communicator->setSenderStateAndEmitSignal(SenderChangingTool);
        m_form->updateControlsState();
    }

    // Pausing on button?
    if ((m_communicator->m_senderState == SenderPausing) && !uncomment.contains(M6)) {
        if (m_settings->usePauseCommands()) {
            m_communicator->sendCommands(CommandSource::ProgramAdditionalCommands, m_settings->beforePauseCommands());
            m_communicator->setSenderStateAndEmitSignal(SenderPausing2);
            m_form->updateControlsState();
        }
    }
    if ((m_communicator->m_senderState == SenderChangingTool) && !m_settings->toolChangePause()
        && m_communicator->m_commands.isEmpty())
    {
        m_communicator->setSenderStateAndEmitSignal(SenderTransferring);
    }

    // Switch to pause mode
    if ((m_communicator->m_senderState == SenderPausing || m_communicator->m_senderState == SenderPausing2) && m_communicator->m_commands.isEmpty()) {
        m_communicator->setSenderStateAndEmitSignal(SenderPaused);
        m_form->updateControlsState();
    }

    // Same as M2, Program End, turn off spindle/laser and stops the machine.
    // Scroll to first line on "M30" command
    if (uncomment.contains("M30")) ui->tblProgram->setCurrentIndex(m_form->currentModel().index(0, 1));

    // Toolpath shadowing on check mode
    if (m_communicator->m_deviceState == DeviceCheck) {
        GcodeViewParse *parser = m_form->currentDrawer().viewParser();
        QList<LineSegment*> list = parser->getLineSegmentList();

        if ((m_communicator->m_senderState != SenderStopping) && m_streamer->processedCommandIndex() < m_form->currentModel().rowCount() - 1) {
            int i;
            QList<int> drawnLines;

            for (i = m_form->lastDrawnLineIndex(); i < list.count()
                                           && list.at(i)->getLineNumber()
                                                          <= (m_form->currentModel().data(m_form->currentModel().index(m_streamer->processedCommandIndex(), 4)).toInt()); i++) {
                drawnLines << i;
            }

            if (!drawnLines.isEmpty() && (i < list.count())) {
                m_form->lastDrawnLineIndex() = i;
                QVector3D vec = list.at(i)->getEnd();
                m_form->toolDrawer().setToolPosition(vec);
            }

            foreach (int i, drawnLines) {
                list.at(i)->setDrawn(true);
            }
            if (!drawnLines.isEmpty()) m_form->currentDrawer().update(drawnLines);
        } else {
            foreach (LineSegment* s, list) {
                if (!qIsNaN(s->getEnd().length())) {
                    m_form->toolDrawer().setToolPosition(s->getEnd());
                    break;
                }
            }
        }
    }

    // Emit response signal
    emit responseReceived(commandAttributes.command, commandAttributes.tableIndex, response);

    response.clear();
}

void Communicator::processUnhandledResponse(QString data)
{
    // Unprocessed responses
    // Handle hardware reset
    if (m_communicator->dataIsReset(data)) {
        m_communicator->setSenderStateAndEmitSignal(SenderStopped);
        m_communicator->setDeviceStateAndEmitSignal(DeviceUnknown);

        m_streamer->reset();
        //m_form->fileCommandIndex() = 0;

        m_communicator->m_reseting = false;
        m_communicator->m_homing = false;

        m_form->updateParserStatus() = true;
        m_communicator->m_statusReceived = true;

        m_communicator->clearCommandsAndQueue();

        m_form->updateControlsState();
    }

    m_form->partConsole().append(data);
}

void Communicator::processMessage(QString data)
{
    qDebug() << "< MSG <" << data;
    // static QRegExp msg("\\[MSG:([^\\]]+)\\]");
    // if (msg.indexIn(data) != -1) {
    //     QString message = msg.cap(1);
    //     if (message.contains("Enabled")) {
    //         m_communicator->m_statusReceived = true;
    //         m_communicator->startUpdatingState();
    //     } else if (message.contains("Disabled")) {
    //         m_communicator->stopUpdatingState();
    //     }
    // }
}
