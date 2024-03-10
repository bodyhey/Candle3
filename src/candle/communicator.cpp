#include "communicator.h"
#include <QVector3D>
#include <QDebug>
#include <QCheckBox>
#include <QMessageBox>
#include <QTextCursor>
#include <parser/gcodeviewparse.h>

Communicator::Communicator(
    Connection *connection,
    Ui::frmMain *ui,
    QObject *parent = nullptr
) : QObject(parent), m_connection(connection), ui(ui) {
    m_reseting = false;
    m_resetCompleted = true;
    m_aborting = false;
    m_statusReceived = false;

    m_deviceState = DeviceUnknown;
    m_senderState = SenderUnknown;

    // Initializing variables
    m_deviceStatuses[DeviceUnknown] = "Unknown";
    m_deviceStatuses[DeviceIdle] = "Idle";
    m_deviceStatuses[DeviceAlarm] = "Alarm";
    m_deviceStatuses[DeviceRun] = "Run";
    m_deviceStatuses[DeviceHome] = "Home";
    m_deviceStatuses[DeviceHold0] = "Hold:0";
    m_deviceStatuses[DeviceHold1] = "Hold:1";
    m_deviceStatuses[DeviceQueue] = "Queue";
    m_deviceStatuses[DeviceCheck] = "Check";
    m_deviceStatuses[DeviceDoor0] = "Door:0";
    m_deviceStatuses[DeviceDoor1] = "Door:1";
    m_deviceStatuses[DeviceDoor2] = "Door:2";
    m_deviceStatuses[DeviceDoor3] = "Door:3";
    m_deviceStatuses[DeviceJog] = "Jog";
    m_deviceStatuses[DeviceSleep] = "Sleep";

    m_statusCaptions[DeviceUnknown] = tr("Unknown");
    m_statusCaptions[DeviceIdle] = tr("Idle");
    m_statusCaptions[DeviceAlarm] = tr("Alarm");
    m_statusCaptions[DeviceRun] = tr("Run");
    m_statusCaptions[DeviceHome] = tr("Home");
    m_statusCaptions[DeviceHold0] = tr("Hold") + " (0)";
    m_statusCaptions[DeviceHold1] = tr("Hold") + " (1)";
    m_statusCaptions[DeviceQueue] = tr("Queue");
    m_statusCaptions[DeviceCheck] = tr("Check");
    m_statusCaptions[DeviceDoor0] = tr("Door") + " (0)";
    m_statusCaptions[DeviceDoor1] = tr("Door") + " (1)";
    m_statusCaptions[DeviceDoor2] = tr("Door") + " (2)";
    m_statusCaptions[DeviceDoor3] = tr("Door") + " (3)";
    m_statusCaptions[DeviceJog] = tr("Jog");
    m_statusCaptions[DeviceSleep] = tr("Sleep");

    m_statusBackColors[DeviceUnknown] = "red";
    m_statusBackColors[DeviceIdle] = "palette(button)";
    m_statusBackColors[DeviceAlarm] = "red";
    m_statusBackColors[DeviceRun] = "lime";
    m_statusBackColors[DeviceHome] = "lime";
    m_statusBackColors[DeviceHold0] = "yellow";
    m_statusBackColors[DeviceHold1] = "yellow";
    m_statusBackColors[DeviceQueue] = "yellow";
    m_statusBackColors[DeviceCheck] = "palette(button)";
    m_statusBackColors[DeviceDoor0] = "red";
    m_statusBackColors[DeviceDoor1] = "red";
    m_statusBackColors[DeviceDoor2] = "red";
    m_statusBackColors[DeviceDoor3] = "red";
    m_statusBackColors[DeviceJog] = "lime";
    m_statusBackColors[DeviceSleep] = "blue";

    m_statusForeColors[DeviceUnknown] = "white";
    m_statusForeColors[DeviceIdle] = "palette(text)";
    m_statusForeColors[DeviceAlarm] = "white";
    m_statusForeColors[DeviceRun] = "black";
    m_statusForeColors[DeviceHome] = "black";
    m_statusForeColors[DeviceHold0] = "black";
    m_statusForeColors[DeviceHold1] = "black";
    m_statusForeColors[DeviceQueue] = "black";
    m_statusForeColors[DeviceCheck] = "palette(text)";
    m_statusForeColors[DeviceDoor0] = "white";
    m_statusForeColors[DeviceDoor1] = "white";
    m_statusForeColors[DeviceDoor2] = "white";
    m_statusForeColors[DeviceDoor3] = "white";
    m_statusForeColors[DeviceJog] = "black";
    m_statusForeColors[DeviceSleep] = "white";

    this->connect(m_connection, SIGNAL(lineReceived(QString)), this, SLOT(onConnectionLineReceived(QString)));
    this->connect(m_connection, SIGNAL(error(QString)), this, SLOT(onConnectionError(QString)));
}

void Communicator::onSerialPortReadyRead(QString data)
{
    /*
    // Filter prereset responses
    if (m_reseting) {
        if (!dataIsReset(data)) {
            return;
        } else {
            m_reseting = false;
//            m_timerStateQuery.setInterval(m_settings->queryStateTime());
        }
    }

    // Status response
    if (data[0] == '<') {
        DeviceState state = DeviceUnknown;

        m_statusReceived = true;

        // Update machine coordinates
        static QRegExp mpx("MPos:([^,]*),([^,]*),([^,^>^|]*)");
        if (mpx.indexIn(data) != -1) {
            ui->txtMPosX->setValue(mpx.cap(1).toDouble());
            ui->txtMPosY->setValue(mpx.cap(2).toDouble());
            ui->txtMPosZ->setValue(mpx.cap(3).toDouble());

            // Update stored vars
            // @todo scripting
            // m_storedVars.setCoords("M", QVector3D(
            //                                 ui->txtMPosX->value(),
            //                                 ui->txtMPosY->value(),
            //                                 ui->txtMPosZ->value()));
        }

        // Status
        static QRegExp stx("<([^,^>^|]*)");
        if (stx.indexIn(data) != -1) {
            state = m_deviceStatuses.key(stx.cap(1), DeviceUnknown);

            // Update status
            if (state != m_deviceState) {
                ui->txtStatus->setText(m_statusCaptions[state]);
                ui->txtStatus->setStyleSheet(QString("background-color: %1; color: %2;")
                                                 .arg(m_statusBackColors[state]).arg(m_statusForeColors[state]));
            }

            // Update controls
            ui->cmdCheck->setEnabled(state != DeviceRun && (m_senderState == SenderStopped));
            ui->cmdCheck->setChecked(state == DeviceCheck);
            ui->cmdHold->setChecked(state == DeviceHold0 || state == DeviceHold1 || state == DeviceQueue);
            ui->cmdSpindle->setEnabled(state == DeviceHold0 || ((m_senderState != SenderTransferring) &&
                                                                (m_senderState != SenderStopping)));

            // Update "elapsed time" timer
            if ((m_senderState == SenderTransferring) || (m_senderState == SenderStopping)) {
                QTime time(0, 0, 0);
                int elapsed = m_startTime.elapsed();
                ui->glwVisualizer->setSpendTime(time.addMSecs(elapsed));
            }

            // Test for job complete
            if ((m_senderState == SenderStopping) &&
                ((state == DeviceIdle && m_deviceState == DeviceRun) || state == DeviceCheck))
            {
                emit formCompleteTransfer();
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
                        emit formRestoreParserState();
                        emit formRestoreOffsets();
                        return;
                    }
                    break;
                case DeviceHold0: // Hold
                case DeviceHold1:
                case DeviceQueue:
                    if (!m_reseting && emit formCompareCoordinates(x, y, z)) {
                        x = sNan;
                        y = sNan;
                        z = sNan;
                        emit formGrblReset();
                    } else {
                        x = ui->txtMPosX->value();
                        y = ui->txtMPosY->value();
                        z = ui->txtMPosZ->value();
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

        // Store work offset
        static QVector3D workOffset;
        static QRegExp wpx("WCO:([^,]*),([^,]*),([^,^>^|]*)");

        if (wpx.indexIn(data) != -1)
        {
            workOffset = QVector3D(wpx.cap(1).toDouble(), wpx.cap(2).toDouble(), wpx.cap(3).toDouble());
        }

        // Update work coordinates
        ui->txtWPosX->setValue(ui->txtMPosX->value() - workOffset.x());
        ui->txtWPosY->setValue(ui->txtMPosY->value() - workOffset.y());
        ui->txtWPosZ->setValue(ui->txtMPosZ->value() - workOffset.z());

        // Update stored vars
        form->m_storedVars.setCoords("W", QVector3D(
                                        ui->txtWPosX->value(),
                                        ui->txtWPosY->value(),
                                        ui->txtWPosZ->value()));

        // Update tool position
        QVector3D toolPosition;
        if (!(state == DeviceCheck && form->m_fileProcessedCommandIndex < form->m_currentModel->rowCount() - 1)) {
            toolPosition = QVector3D(form->toMetric(ui->txtWPosX->value()),
                                     form->toMetric(ui->txtWPosY->value()),
                                     form->toMetric(ui->txtWPosZ->value()));
            form->m_toolDrawer.setToolPosition(form->m_codeDrawer->getIgnoreZ() ? QVector3D(toolPosition.x(), toolPosition.y(), 0) : toolPosition);
        }


        // Toolpath shadowi`ng
        if (((m_senderState == SenderTransferring) || (m_senderState == SenderStopping)
             || (m_senderState == SenderPausing) || (m_senderState == SenderPausing2) || (m_senderState == SenderPaused)) && state != DeviceCheck) {
            GcodeViewParse *parser = form->m_currentDrawer->viewParser();

            bool toolOntoolpath = false;

            QList<int> drawnLines;
            QList<LineSegment*> list = parser->getLineSegmentList();

            for (int i = form->m_lastDrawnLineIndex; i < list.count()
                                               && list.at(i)->getLineNumber()
                                                      <= (form->m_currentModel->data(form->m_currentModel->index(form->m_fileProcessedCommandIndex, 4)).toInt() + 1); i++) {
                if (list.at(i)->contains(toolPosition)) {
                    toolOntoolpath = true;
                    form->m_lastDrawnLineIndex = i;
                    break;
                }
                drawnLines << i;
            }

            if (toolOntoolpath) {
                foreach (int i, drawnLines) {
                    list.at(i)->setDrawn(true);
                }
                if (!drawnLines.isEmpty()) form->m_currentDrawer->update(drawnLines);
            }
        }

        // Get overridings
        static QRegExp ov("Ov:([^,]*),([^,]*),([^,^>^|]*)");
        if (ov.indexIn(data) != -1)
        {
            emit formUpdateOverride(ui->slbFeedOverride, ov.cap(1).toInt(), '\x91');
            emit formUpdateOverride(ui->slbSpindleOverride, ov.cap(3).toInt(), '\x9a');

            int rapid = ov.cap(2).toInt();
            ui->slbRapidOverride->setCurrentValue(rapid);

            int target = ui->slbRapidOverride->isChecked() ? ui->slbRapidOverride->value() : 100;

            if (rapid != target) switch (target) {
                case 25:
                    m_serialPort.write(QByteArray(1, char(0x97)));
                    break;
                case 50:
                    m_serialPort.write(QByteArray(1, char(0x96)));
                    break;
                case 100:
                    m_serialPort.write(QByteArray(1, char(0x95)));
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
                form->m_spindleCW = q.contains("S");
                if (q.contains("S") || q.contains("C")) {
                    form->m_timerToolAnimation.start(25, this);
                    ui->cmdSpindle->setChecked(true);
                } else {
                    form->m_timerToolAnimation.stop();
                    ui->cmdSpindle->setChecked(false);
                }
                ui->cmdFlood->setChecked(q.contains("F"));

                if (!pinState.isEmpty()) pinState.append(" / ");
                pinState.append(QString(tr("AS: %1")).arg(as.cap(1)));
            } else {
                form->m_timerToolAnimation.stop();
                ui->cmdSpindle->setChecked(false);
            }
            ui->glwVisualizer->setPinState(pinState);
        }

        // Get feed/spindle values
        static QRegExp fs("FS:([^,]*),([^,^|^>]*)");
        if (fs.indexIn(data) != -1) {
            ui->glwVisualizer->setSpeedState((QString(tr("F/S: %1 / %2")).arg(fs.cap(1)).arg(fs.cap(2))));
        }

        // Store device state
        setDeviceState(state);

        // Update continuous jog
        form->jogContinuous();

        // Emit status signal
        emit statusReceived(data);

        // Command response
    } else if (data.length() > 0) {

        if (m_commands.length() > 0 && !form->dataIsFloating(data)
            && !(m_commands[0].command != "[CTRL+X]" && dataIsReset(data))) {

            static QString response; // Full response string

            if ((m_commands[0].command != "[CTRL+X]" && form->dataIsEnd(data))
                || (m_commands[0].command == "[CTRL+X]" && dataIsReset(data))) {

                response.append(data);

                // Take command from buffer
                CommandAttributes ca = m_commands.takeFirst();
                QTextBlock tb = ui->txtConsole->document()->findBlockByNumber(ca.consoleIndex);
                QTextCursor tc(tb);

                QString uncomment = GcodePreprocessorUtils::removeComment(ca.command).toUpper();

                // Store current coordinate system
                if (uncomment == "$G") {
                    static QRegExp g("G5[4-9]");
                    if (g.indexIn(response) != -1) {
                        form->m_storedVars.setCS(g.cap(0));
                        form->m_machineBoundsDrawer.setOffset(QPointF(toMetric(form->m_storedVars.x()), toMetric(m_storedVars.y())) +
                                                        QPointF(toMetric(form->m_storedVars.G92x()), toMetric(m_storedVars.G92y())));
                    }
                    static QRegExp t("T(\\d+)(?!\\d)");
                    if (t.indexIn(response) != -1) {
                        form->m_storedVars.setTool(g.cap(1).toInt());
                    }
                }

                // TODO: Store firmware version, features, buffer size on $I command
                // [VER:1.1d.20161014:Some string]
                // [OPT:VL,15,128]

                // Restore absolute/relative coordinate system after jog
                if (uncomment == "$G" && ca.tableIndex == -2) {
                    // @TODO ui
                    // if (ui->chkKeyboardControl->isChecked()) m_absoluteCoordinates = response.contains("G90");
                    // else if (response.contains("G90")) sendCommand("G90", -1, m_settings->showUICommands());
                }

                // Process parser status
                if (uncomment == "$G" && ca.tableIndex == -3) {
                    // Update status in visualizer window
                    ui->glwVisualizer->setParserStatus(response.left(response.indexOf("; ")));

                    // Store parser status
                    if ((m_senderState == SenderTransferring) || (m_senderState == SenderStopping)) emit formStoreParserState();

                    // Spindle speed
                    QRegExp rx(".*S([\\d\\.]+)");
                    if (rx.indexIn(response) != -1) {
                        double speed = rx.cap(1).toDouble();
                        ui->slbSpindle->setCurrentValue(speed);
                    }

                    m_updateParserStatus = true;
                }

                // Offsets
                if (uncomment == "$#") form->storeOffsetsVars(response);

                // Settings response
                if (uncomment == "$$" && ca.tableIndex == -2) {
                    static QRegExp gs("\\$(\\d+)\\=([^;]+)\\; ");
                    QMap<int, double> set;
                    int p = 0;
                    while ((p = gs.indexIn(response, p)) != -1) {
                        set[gs.cap(1).toInt()] = gs.cap(2).toDouble();
                        p += gs.matchedLength();
                    }
                    QList<int> keys = set.keys();
                    if (keys.contains(13)) m_settings->setUnits(set[13]);
                    if (keys.contains(20)) m_settings->setSoftLimitsEnabled(set[20]);
                    if (keys.contains(22)) {
                        m_settings->setHomingEnabled(set[22]);
                        emit m_machineBoundsDrawer.setVisible(set[22]);
                    }
                    if (keys.contains(110)) m_settings->setRapidSpeed(set[110]);
                    if (keys.contains(120)) m_settings->setAcceleration(set[120]);
                    if (keys.contains(130) && keys.contains(131) && keys.contains(132)) {
                        QVector3D bounds = QVector3D(
                            m_settings->referenceXPlus() ? -set[130] : set[130],
                            m_settings->referenceYPlus() ? -set[131] : set[131],
                            m_settings->referenceZPlus() ? -set[132] : set[132]
                            );
                        m_settings->setMachineBounds(bounds);
                        form->m_machineBoundsDrawer.setBorderRect(QRectF(0, 0,
                                                                   m_settings->referenceXPlus() ? -set[130] : set[130],
                                                                   m_settings->referenceYPlus() ? -set[131] : set[131]));
                        qDebug() << "Machine bounds (3-axis)" << bounds;
                    } else if (keys.contains(130) && keys.contains(131)) {
                        // 2 axis
                        QVector3D bounds = QVector3D(
                            m_settings->referenceXPlus() ? -set[130] : set[130],
                            m_settings->referenceYPlus() ? -set[131] : set[131],
                            0
                            );
                        m_settings->setMachineBounds(QVector3D(
                            m_settings->referenceXPlus() ? -set[130] : set[130],
                            m_settings->referenceYPlus() ? -set[131] : set[131],
                            0));
                        form->m_machineBoundsDrawer.setBorderRect(QRectF(0, 0,
                                                                   m_settings->referenceXPlus() ? -set[130] : set[130],
                                                                   m_settings->referenceYPlus() ? -set[131] : set[131]));
                        qDebug() << "Machine bounds (2-axis)" << bounds;
                    }

                    form->setupCoordsTextboxes();
                }

                // Homing response
                if ((uncomment == "$H" || uncomment == "$T") && m_homing) m_homing = false;

                // Reset complete response
                if (uncomment == "[CTRL+X]") {
                    m_resetCompleted = true;
                    form->m_updateParserStatus = true;

                    // Query grbl settings
                    sendCommand("$$", -2, false);
                    sendCommand("$#", -2, false, true);
                }

                // Clear command buffer on "M2" & "M30" command (old firmwares)
                static QRegExp M230("(M0*2|M30)(?!\\d)");
                if (uncomment.contains(M230) && response.contains("ok") && !response.contains("Pgm End")) {
                    m_commands.clear();
                    m_queue.clear();
                }

                // Update probe coords on user commands
                if (uncomment.contains("G38.2") && ca.tableIndex < 0) {
                    static QRegExp PRB(".*PRB:([^,]*),([^,]*),([^,:]*)");
                    if (PRB.indexIn(response) != -1) {
                        form->m_storedVars.setCoords("PRB", QVector3D(
                                                          PRB.cap(1).toDouble(),
                                                          PRB.cap(2).toDouble(),
                                                          PRB.cap(3).toDouble()
                                                          ));
                    }
                }

                // Process probing on heightmap mode only from table commands
                if (uncomment.contains("G38.2") && form->m_heightMapMode && ca.tableIndex > -1) {
                    // Get probe Z coordinate
                    // "[PRB:0.000,0.000,0.000:0];ok"
                    // "[PRB:0.000,0.000,0.000,0.000:0];ok"
                    QRegExp rx(".*PRB:([^,]*),([^,]*),([^,:]*)");
                    double z = qQNaN();
                    if (rx.indexIn(response) != -1) {
                        z = form->toMetric(rx.cap(3).toDouble());
                    }

                    static double firstZ;
                    if (form->m_probeIndex == -1) {
                        firstZ = z;
                        z = 0;
                    } else {
                        // Calculate delta Z
                        z -= firstZ;

                        // Calculate table indexes
                        int row = (form->m_probeIndex / form->m_heightMapModel.columnCount());
                        int column = form->m_probeIndex - row * form->m_heightMapModel.columnCount();
                        if (row % 2) column = form->m_heightMapModel.columnCount() - 1 - column;

                        // Store Z in table
                        form->m_heightMapModel.setData(form->m_heightMapModel.index(row, column), z, Qt::UserRole);
                        ui->tblHeightMap->update(form->m_heightMapModel.index(form->m_heightMapModel.rowCount() - 1 - row, column));
                        form->updateHeightMapInterpolationDrawer();
                    }

                    form->m_probeIndex++;
                }

                // Change state query time on check mode on
                if (uncomment.contains(QRegExp("$[cC]"))) {
                    form->m_timerStateQuery.setInterval(response.contains("Enable") ? 1000 : m_settings->queryStateTime());
                }

                // Add response to console
                if (tb.isValid() && tb.text() == ca.command) {

                    bool scrolledDown = ui->txtConsole->verticalScrollBar()->value()
                                        == ui->txtConsole->verticalScrollBar()->maximum();

                    // Update text block numbers
                    int blocksAdded = response.count("; ");

                    if (blocksAdded > 0) for (int i = 0; i < m_commands.count(); i++) {
                            if (m_commands[i].consoleIndex != -1) m_commands[i].consoleIndex += blocksAdded;
                        }

                    tc.beginEditBlock();
                    tc.movePosition(QTextCursor::EndOfBlock);

                    tc.insertText(" < " + QString(response).replace("; ", "\r\n"));
                    tc.endEditBlock();

                    if (scrolledDown) ui->txtConsole->verticalScrollBar()->setValue(
                            ui->txtConsole->verticalScrollBar()->maximum());
                }

                // Check queue
                static bool processingQueue = false;
                if (m_queue.length() > 0 && !processingQueue) {
                    processingQueue = true;
                    while (m_queue.length() > 0) {
                        CommandQueue cq = m_queue.takeFirst();
                        SendCommandResult r = sendCommand(cq.command, cq.tableIndex, cq.showInConsole);
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
                    if (ca.tableIndex > -1) {
                        form->m_currentModel->setData(m_currentModel->index(ca.tableIndex, 2), GCodeItem::Processed);
                        form->m_currentModel->setData(m_currentModel->index(ca.tableIndex, 3), response);

                        form->m_fileProcessedCommandIndex = ca.tableIndex;

                        if (ui->chkAutoScroll->isChecked() && ca.tableIndex != -1) {
                            ui->tblProgram->scrollTo(form->m_currentModel->index(ca.tableIndex + 1, 0));      // TODO: Update by timer
                            ui->tblProgram->setCurrentIndex(form->m_currentModel->index(ca.tableIndex, 1));
                        }
                    }

// Update taskbar progress
#ifdef WINDOWS
                    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
                        if (m_taskBarProgress) form->m_taskBarProgress->setValue(m_fileProcessedCommandIndex);
                    }
#endif
                    // Process error messages
                    static bool holding = false;
                    static QString errors;

                    if (ca.tableIndex > -1 && response.toUpper().contains("ERROR") && !m_settings->ignoreErrors()) {
                        errors.append(QString::number(ca.tableIndex + 1) + ": " + ca.command
                                      + " < " + response + "\n");

                        form->m_senderErrorBox->setText(tr("Error message(s) received:\n") + errors);

                        if (!holding) {
                            holding = true;         // Hold transmit while messagebox is visible
                            response.clear();

                            form->m_serialPort.write("!");
                            form->m_senderErrorBox->checkBox()->setChecked(false);
                            qApp->beep();
                            int result = form->m_senderErrorBox->exec();

                            holding = false;
                            errors.clear();
                            if (form->m_senderErrorBox->checkBox()->isChecked()) m_settings->setIgnoreErrors(true);
                            if (result == QMessageBox::Ignore) {
                                m_connection->sendByteArray(QByteArray())
                                form->m_serialPort.write("~");
                            } else {
                                form->m_serialPort.write("~");
                                ui->cmdFileAbort->click();
                            }
                        }
                    }

                    // Check transfer complete (last row always blank, last command row = rowcount - 2)
                    if ((form->m_fileProcessedCommandIndex == form->m_currentModel->rowCount() - 2) ||
                        uncomment.contains(QRegExp("(M0*2|M30)(?!\\d)")))
                    {
                        if (m_deviceState == DeviceRun) {
                            form->setSenderState(SenderStopping);
                        } else {
                            form->completeTransfer();
                        }
                    } else if ((form->m_fileCommandIndex < form->m_currentModel->rowCount())
                               && (m_senderState == SenderTransferring)
                               && !holding)
                    {
                        // Send next program commands
                        form->sendNextFileCommands();
                    }
                }

                // Tool change mode
                static QRegExp M6("(M0*6)(?!\\d)");
                if ((m_senderState == SenderPausing) && uncomment.contains(M6)) {
                    response.clear();

                    if (m_settings->toolChangePause()) {
                        QMessageBox::information(this, qApp->applicationDisplayName(),
                                                 tr("Change tool and press 'Pause' button to continue job"));
                    }

                    if (form->m_settings->toolChangeUseCommands()) {
                        if (m_settings->toolChangeUseCommandsConfirm()) {
                            QMessageBox box(this);
                            box.setIcon(QMessageBox::Information);
                            box.setText(tr("M6 command detected. Send tool change commands?\n"));
                            box.setWindowTitle(qApp->applicationDisplayName());
                            box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                            box.setCheckBox(new QCheckBox(tr("Don't show again")));
                            int res = box.exec();
                            if (box.checkBox()->isChecked()) m_settings->setToolChangeUseCommandsConfirm(false);
                            if (res == QMessageBox::Yes) {
                                form->sendCommands(m_settings->toolChangeCommands());
                            }
                        } else {
                            form->sendCommands(m_settings->toolChangeCommands());
                        }
                    }

                    setSenderState(SenderChangingTool);
                    form->updateControlsState();
                }
                // Pausing on button?
                if ((m_senderState == SenderPausing) && !uncomment.contains(M6)) {
                    if (m_settings->usePauseCommands()) {
                        form->sendCommands(m_settings->beforePauseCommands());
                        setSenderState(SenderPausing2);
                        form->updateControlsState();
                    }
                }
                if ((m_senderState == SenderChangingTool) && !m_settings->toolChangePause()
                    && m_commands.isEmpty())
                {
                    setSenderState(SenderTransferring);
                }

                // Switch to pause mode
                if ((m_senderState == SenderPausing || m_senderState == SenderPausing2) && m_commands.isEmpty()) {
                    setSenderState(SenderPaused);
                    form->updateControlsState();
                }

                // Scroll to first line on "M30" command
                if (uncomment.contains("M30")) ui->tblProgram->setCurrentIndex(form->m_currentModel->index(0, 1));

                // Toolpath shadowing on check mode
                if (m_deviceState == DeviceCheck) {
                    GcodeViewParse *parser = form->m_currentDrawer->viewParser();
                    QList<LineSegment*> list = parser->getLineSegmentList();

                    if ((m_senderState != SenderStopping) && form->m_fileProcessedCommandIndex < form->m_currentModel->rowCount() - 1) {
                        int i;
                        QList<int> drawnLines;

                        for (i = form->m_lastDrawnLineIndex; i < list.count()
                                                       && list.at(i)->getLineNumber()
                                                                    <= (form->m_currentModel->data(form->m_currentModel->index(form->m_fileProcessedCommandIndex, 4)).toInt()); i++) {
                            drawnLines << i;
                        }

                        if (!drawnLines.isEmpty() && (i < list.count())) {
                            form->m_lastDrawnLineIndex = i;
                            QVector3D vec = list.at(i)->getEnd();
                            form->m_toolDrawer.setToolPosition(vec);
                        }

                        foreach (int i, drawnLines) {
                            list.at(i)->setDrawn(true);
                        }
                        if (!drawnLines.isEmpty()) form->m_currentDrawer->update(drawnLines);
                    } else {
                        foreach (LineSegment* s, list) {
                            if (!qIsNaN(s->getEnd().length())) {
                                form->m_toolDrawer.setToolPosition(s->getEnd());
                                break;
                            }
                        }
                    }
                }

                // Emit response signal
                emit responseReceived(ca.command, ca.tableIndex, response);

                response.clear();
            } else {
                response.append(data + "; ");
            }

        } else {
            // Unprocessed responses
            // Handle hardware reset
            if (dataIsReset(data)) {
                setSenderState(SenderStopped);
                setDeviceState(DeviceUnknown);

                form->m_fileCommandIndex = 0;

                m_reseting = false;
                m_homing = false;

                form->m_updateParserStatus = true;
                m_statusReceived = true;

                m_commands.clear();
                m_queue.clear();

                form->updateControlsState();
            }
            ui->txtConsole->appendPlainText(data);
        }
    } else {
        // Blank response
    }
*/
}

bool Communicator::dataIsReset(QString data)
{
    return QRegExp("^GRBL|GCARVIN\\s\\d\\.\\d.").indexIn(data.toUpper()) != -1;
}
