// This file is a part of "Candle" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich

#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QTextBlock>
#include <QTextCursor>
#include <QMessageBox>
#include <QComboBox>
#include <QScrollBar>
#include <QShortcut>
#include <QAction>
#include <QLayout>
#include <QDrag>
#include <QMimeData>
#include <QTranslator>
#include <QDockWidget>
#include "frmmain.h"
#include "form_partial/main/jog.h"
#include "form_partial/main/control.h"
#include "ui_frmmain.h"
#include "ui_frmsettings.h"
#include "widgets/widgetmimedata.h"
#include "connection/connectionmanager.h"

#define FILE_FILTER_TEXT "G-Code files (*.nc *.ncc *.ngc *.tap *.gc *.gcode *.txt)"

frmMain::frmMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frmMain),
    m_connectionManager(this, m_configuration.connectionModule()),
    m_configuration(this)
{
    m_configuration.load();
//    setDefaults();
    // m_configuration.save();

    // qApp->quit();
    // return;

    // Loading settings
    m_settingsFileName = qApp->applicationDirPath() + "/settings.ini";
    preloadSettings();

    m_settings = new frmSettings(this, m_configuration);

    initializeConnection(ConnectionMode::VIRTUAL);
    initializeCommunicator();

    // Initializing variables
    // m_deviceStatuses[DeviceUnknown] = "Unknown";
    // m_deviceStatuses[DeviceIdle] = "Idle";
    // m_deviceStatuses[DeviceAlarm] = "Alarm";
    // m_deviceStatuses[DeviceRun] = "Run";
    // m_deviceStatuses[DeviceHome] = "Home";
    // m_deviceStatuses[DeviceHold0] = "Hold:0";
    // m_deviceStatuses[DeviceHold1] = "Hold:1";
    // m_deviceStatuses[DeviceQueue] = "Queue";
    // m_deviceStatuses[DeviceCheck] = "Check";
    // m_deviceStatuses[DeviceDoor0] = "Door:0";
    // m_deviceStatuses[DeviceDoor1] = "Door:1";
    // m_deviceStatuses[DeviceDoor2] = "Door:2";
    // m_deviceStatuses[DeviceDoor3] = "Door:3";
    // m_deviceStatuses[DeviceJog] = "Jog";
    // m_deviceStatuses[DeviceSleep] = "Sleep";

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

    m_fileChanged = false;
    m_heightmapChanged = false;

    // to communicator
    // m_communicator->m_homing = false;
    // m_updateSpindleSpeed = false;
    // m_updateParserStatus = false;

    // to communicator
    // m_reseting = false;
    // m_communicator->m_resetCompleted = true;
    // m_aborting = false;
    // m_statusReceived = false;

    // to communicator
    // m_deviceState = DeviceUnknown;
    // m_communicator->m_senderState = SenderUnknown;

    ui->setupUi(this);

    ui->widgetJog->setParent(nullptr);
    static_cast<QVBoxLayout*>(ui->grpJog->layout())->removeWidget(ui->widgetJog);

    ui->widgetJog = new partMainJog(this);
    static_cast<QVBoxLayout*>(ui->grpJog->layout())->insertWidget(0, ui->widgetJog);
    ui->widgetJog->show();

    QWidget *widgetControl = new partMainControl(this);
    static_cast<QVBoxLayout*>(ui->grpControl->layout())->insertWidget(0, widgetControl);
    widgetControl->show();

    m_partState = new partMainState(this, &m_configuration);
    static_cast<QVBoxLayout*>(ui->grpState->layout())->insertWidget(0, this->m_partState);

    m_partConsole = new partMainConsole(this, m_configuration.consoleModule());
    connect(this->m_partConsole, SIGNAL(newCommand(QString)), this, SLOT(onConsoleNewCommand(QString)));
    m_partConsole->append(QString("G-Candle %1 started").arg( qApp->applicationVersion()));
    m_partConsole->append("---");

    QLayout *layout = ui->dockConsole->widget()->layout();
    layout->takeAt(0);
    layout->addWidget(this->m_partConsole);

    // Drag&drop placeholders
    ui->fraDropDevice->setVisible(false);
    ui->fraDropModification->setVisible(false);
    ui->fraDropUser->setVisible(false);

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        m_taskBarButton = NULL;
        m_taskBarProgress = NULL;
    }
#endif

//    ui->scrollArea->updateMinimumWidth();

    m_heightmapMode = false;
    m_lastDrawnLineIndex = 0;
    m_streamer->resetProcessed();
    m_programLoading = false;
    updateCurrentModel(&m_programModel);

    // Dock widgets
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    ui->widgetHeightmapSettings->setVisible(false);

    ui->cmdXMinus->setBackColor(QColor(153, 180, 209));
    ui->cmdXPlus->setBackColor(ui->cmdXMinus->backColor());
    ui->cmdYMinus->setBackColor(ui->cmdXMinus->backColor());
    ui->cmdYPlus->setBackColor(ui->cmdXMinus->backColor());

    ui->cmdToggleProjection->setParent(ui->glwVisualizer);
    ui->cmdFit->setParent(ui->glwVisualizer);
    ui->cmdIsometric->setParent(ui->glwVisualizer);
    ui->cmdTop->setParent(ui->glwVisualizer);
    ui->cmdFront->setParent(ui->glwVisualizer);
    ui->cmdLeft->setParent(ui->glwVisualizer);

    ui->cmdHeightMapBorderAuto->setMinimumHeight(ui->chkHeightMapBorderShow->sizeHint().height());
    ui->cmdHeightMapCreate->setMinimumHeight(ui->cmdFileOpen->sizeHint().height());
    ui->cmdHeightMapLoad->setMinimumHeight(ui->cmdFileOpen->sizeHint().height());
    ui->cmdHeightMapMode->setMinimumHeight(ui->cmdFileOpen->sizeHint().height());

    ui->cboJogStep->setValidator(new QDoubleValidator(0, 10000, 2));
    ui->cboJogFeed->setValidator(new QIntValidator(0, 100000));
    connect(ui->cboJogStep, &ComboBoxKey::currentTextChanged, this, &frmMain::updateJogTitle);
    connect(ui->cboJogFeed, &ComboBoxKey::currentTextChanged, this, &frmMain::updateJogTitle);

    // Prepare "Send"-button
    ui->cmdFileSend->setMinimumWidth(qMax(ui->cmdFileSend->width(), ui->cmdFileOpen->width()));
    QMenu *menuSend = new QMenu();
    menuSend->addAction(tr("Send from current line"), this, SLOT(onActSendFromLineTriggered()));
    ui->cmdFileSend->setMenu(menuSend);

    // connect(ui->cboCommand, SIGNAL(returnPressed()), this, SLOT(onCboCommandReturnPressed()));

    foreach (StyledToolButton* button, this->findChildren<StyledToolButton*>(QRegExp("cmdUser\\d"))) {
        connect(button, SIGNAL(clicked(bool)), this, SLOT(onCmdUserClicked(bool)));
    }

    // Setting up slider boxes
    ui->slbFeedOverride->setRatio(1);
    ui->slbFeedOverride->setMinimum(10);
    ui->slbFeedOverride->setMaximum(200);
    ui->slbFeedOverride->setCurrentValue(100);
    ui->slbFeedOverride->setTitle(tr("Feed rate:"));
    ui->slbFeedOverride->setSuffix("%");
    connect(ui->slbFeedOverride, SIGNAL(toggled(bool)), this, SLOT(onOverridingToggled(bool)));
    connect(ui->slbFeedOverride, &SliderBox::toggled, this, &frmMain::onOverrideChanged);
    connect(ui->slbFeedOverride, &SliderBox::valueChanged, this, &frmMain::onOverrideChanged);

    ui->slbRapidOverride->setRatio(50);
    ui->slbRapidOverride->setMinimum(25);
    ui->slbRapidOverride->setMaximum(100);
    ui->slbRapidOverride->setCurrentValue(100);
    ui->slbRapidOverride->setTitle(tr("Rapid speed:"));
    ui->slbRapidOverride->setSuffix("%");
    connect(ui->slbRapidOverride, SIGNAL(toggled(bool)), this, SLOT(onOverridingToggled(bool)));
    connect(ui->slbRapidOverride, &SliderBox::toggled, this, &frmMain::onOverrideChanged);
    connect(ui->slbRapidOverride, &SliderBox::valueChanged, this, &frmMain::onOverrideChanged);   

    ui->slbSpindleOverride->setRatio(1);
    ui->slbSpindleOverride->setMinimum(50);
    ui->slbSpindleOverride->setMaximum(200);
    ui->slbSpindleOverride->setCurrentValue(100);
    ui->slbSpindleOverride->setTitle(tr("Spindle speed:"));
    ui->slbSpindleOverride->setSuffix("%");
    connect(ui->slbSpindleOverride, SIGNAL(toggled(bool)), this, SLOT(onOverridingToggled(bool)));

    m_originDrawer = new OriginDrawer();
    m_codeDrawer = new GcodeDrawer();
    m_codeDrawer->setViewParser(&m_viewParser);
    m_probeDrawer = new GcodeDrawer();
    m_probeDrawer->setViewParser(&m_probeParser);
    m_probeDrawer->setVisible(false);
    m_heightmapGridDrawer.setModel(&m_heightmapModel);
    m_currentDrawer = m_codeDrawer;
    m_toolDrawer.setToolPosition(QVector3D(0, 0, 0));

    m_tableMenu = new QMenu(this);
    m_tableMenu->addAction(tr("&Insert line"), this, SLOT(onTableInsertLine()), QKeySequence(Qt::Key_Insert));
    m_tableMenu->addAction(tr("&Delete lines"), this, SLOT(onTableDeleteLines()), QKeySequence(Qt::Key_Delete));

    initializeVisualizer();

    connect(ui->glwVisualizer, SIGNAL(resized()), this, SLOT(placeVisualizerButtons()));
    connect(ui->glwVisualizer, SIGNAL(toolPos(QPointF)), this, SLOT(onToolPos(QPointF)));
    connect(&m_programModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_programHeightmapModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_probeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_heightmapModel, SIGNAL(dataChangedByUserInput()), this, SLOT(updateHeightMapInterpolationDrawer()));

    ui->tblProgram->setModel(&m_programModel);
    ui->tblProgram->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    connect(ui->tblProgram->verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(onScroolBarAction(int)));
    connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));    
    clearTable();

    m_senderErrorBox = new QMessageBox(QMessageBox::Warning, qApp->applicationDisplayName(), QString(),
                                       QMessageBox::Ignore | QMessageBox::Abort, this);
    m_senderErrorBox->setCheckBox(new QCheckBox(tr("Don't show again")));

    // Loading settings
    loadSettings();
    ui->tblProgram->hideColumn(4);
    ui->tblProgram->hideColumn(5);

    // to communicator
    //setSenderState(SenderStopped);
    updateControlsState();

    // Prepare jog buttons
    foreach (StyledToolButton* button, ui->grpJog->findChildren<StyledToolButton*>(QRegExp("cmdJogFeed\\d")))
    {
        connect(button, SIGNAL(clicked(bool)), this, SLOT(onCmdJogFeedClicked()));
    }

    // Setting up spindle slider box
    ui->slbSpindle->setTitle(tr("Speed:"));
    ui->slbSpindle->setCheckable(false);
    ui->slbSpindle->setChecked(true);
    connect(ui->slbSpindle, &SliderBox::valueUserChanged, this, &frmMain::onSlbSpindleValueUserChanged);
    connect(ui->slbSpindle, &SliderBox::valueChanged, this, &frmMain::onSlbSpindleValueChanged);

    // Setup serial port
    // m_serialPort.setParity(QSerialPort::NoParity);
    // m_serialPort.setDataBits(QSerialPort::Data8);
    // m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
    // m_serialPort.setStopBits(QSerialPort::OneStop);

    // if (m_settings->port() != "") {
    //     SerialConnection *connection = qobject_cast<SerialConnection*>(m_connection);
    //     if (connection != nullptr) {
    //         connection->setPortName(m_settings->port());
    //         connection->setBaudRate(m_settings->baud());
    //     }
    // }

    // Enable form actions
    QList<QAction*> noActions;
    noActions << ui->actJogXMinus << ui->actJogXPlus
              << ui->actJogYMinus << ui->actJogYPlus
              << ui->actJogZMinus << ui->actJogZPlus;  
    foreach (QAction* a, findChildren<QAction*>()) if (!noActions.contains(a)) addAction(a);

    // Handle file drop
    if (qApp->arguments().count() > 1 && isGCodeFile(qApp->arguments().last())) {
        loadFile(qApp->arguments().last());
    }
    
    // Signals/slots
    connect(&m_timerConnection, SIGNAL(timeout()), this, SLOT(onTimerConnection()));

    // Event filter
    qApp->installEventFilter(this);

    // Start timers
    m_timerConnection.start(1000);
}

frmMain::~frmMain()
{
    delete m_communicator;
    delete m_connection;
    delete m_senderErrorBox;
    delete ui; ui = nullptr;
}

void frmMain::initializeCommunicator()
{
    m_communicator = new Communicator(
        this,
        m_connection,
        &m_configuration
    );
    m_streamer = new Streamer();
    // @TODO temporary!
    m_communicator->streamCommands(m_streamer);

    connect(m_communicator, SIGNAL(machinePosChanged(QVector3D)), this, SLOT(onMachinePosChanged(QVector3D)));
    connect(m_communicator, SIGNAL(workPosChanged(QVector3D)), this, SLOT(onWorkPosChanged(QVector3D)));
    connect(m_communicator, SIGNAL(deviceStateReceived(DeviceState)), this, SLOT(onDeviceStateReceived(DeviceState)));
    connect(m_communicator, SIGNAL(deviceStateChanged(DeviceState)), this, SLOT(onDeviceStateChanged(DeviceState)));
    connect(m_communicator, SIGNAL(senderStateReceived(SenderState)), this, SLOT(onSenderStateReceived(SenderState)));
    connect(m_communicator, SIGNAL(spindleStateReceived(bool)), this, SLOT(onSpindleStateReceived(bool)));
    connect(m_communicator, SIGNAL(floodStateReceived(bool)), this, SLOT(onFloodStateReceived(bool)));
    connect(m_communicator, SIGNAL(commandSent(CommandAttributes)), this, SLOT(onCommandSent(CommandAttributes)));
    connect(m_communicator, SIGNAL(commandResponseReceived(CommandAttributes)), this, SLOT(onCommandResponseReceived(CommandAttributes)));
    connect(m_communicator, SIGNAL(parserStateReceived(QString)), this, SLOT(onParserStateReceived(QString)));
    connect(m_communicator, SIGNAL(pinStateReceived(QString)), this, SLOT(onPinStateReceived(QString)));
    connect(m_communicator, SIGNAL(spindleSpeedReceived(int)), this, SLOT(onSpindleSpeedReceived(int)));
    connect(m_communicator, SIGNAL(commandProcessed(int,QString)), this, SLOT(onCommandProcessed(int,QString)));
    connect(m_communicator, SIGNAL(feedSpindleSpeedReceived(int,int)), this, SLOT(onFeedSpindleSpeedReceived(int,int)));
    connect(m_communicator, SIGNAL(overridesReceived(int,int,int)), this, SLOT(onOverridesReceived(int,int,int)));
    connect(m_communicator, SIGNAL(toolPositionReceived(QVector3D)), this, SLOT(onToolPositionReceived(QVector3D)));
    connect(m_communicator, SIGNAL(transferCompleted()), this, SLOT(onTransferCompleted()));
    connect(m_communicator, SIGNAL(aborted()), this, SLOT(onAborted()));
}

void frmMain::initializeVisualizer()
{
    *ui->glwVisualizer << &m_tableSurfaceDrawer
                      << m_originDrawer
                      << m_codeDrawer
                      << m_probeDrawer
                      << &m_toolDrawer
                       << &m_heightmapBorderDrawer
                       << &m_heightmapGridDrawer
                       << &m_heightmapInterpolationDrawer
                      << &m_selectionDrawer
                      << &m_machineBoundsDrawer;

    ui->glwVisualizer->fitDrawable();
}

void frmMain::showEvent(QShowEvent *se)
{
    Q_UNUSED(se)

    placeVisualizerButtons();

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        if (m_taskBarButton == NULL) {
            m_taskBarButton = new QWinTaskbarButton(this);
            m_taskBarButton->setWindow(this->windowHandle());
            m_taskBarProgress = m_taskBarButton->progress();
        }
    }
#endif
}

void frmMain::hideEvent(QHideEvent *he)
{
    Q_UNUSED(he)
}

void frmMain::resizeEvent(QResizeEvent *re)
{
    Q_UNUSED(re)

    placeVisualizerButtons();
    resizeTableHeightmapSections();
}

void frmMain::timerEvent(QTimerEvent *te)
{
    if (te->timerId() == m_timerToolAnimation.timerId()) {
        m_toolDrawer.rotate((m_communicator->m_spindleCW ? -40 : 40) * (double)(ui->slbSpindle->currentValue())
                            / (ui->slbSpindle->maximum()));
    } else {
        QMainWindow::timerEvent(te);
    }
}

void frmMain::closeEvent(QCloseEvent *ce)
{
    bool mode = m_heightmapMode;
    m_heightmapMode = false;

    if (!saveChanges(m_heightmapMode)) {
        ce->ignore();
        m_heightmapMode = mode;
        return;
    }

    if ((m_communicator->senderState() != SenderStopped) &&
        QMessageBox::warning(this, this->windowTitle(), tr("File sending in progress. Terminate and exit?"),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) 
    {    
        ce->ignore();
        m_heightmapMode = mode;
        return;
    }

    m_timerConnection.stop();
    m_connection->closeConnection();
    
    m_communicator->clearCommandsAndQueue();
    // moved to communicator
    // if (m_communicator->m_queue.length() > 0) {
    //     m_communicator->m_commands.clear();
    //     m_communicator->m_queue.clear();
    // }

    saveSettings();
}

void frmMain::dragEnterEvent(QDragEnterEvent *dee)
{
    if (m_communicator->senderState() != SenderStopped) return;

    if (dee->mimeData()->hasFormat("application/widget")) return;

    if (dee->mimeData()->hasFormat("text/plain") && !m_heightmapMode) dee->acceptProposedAction();
    else if (dee->mimeData()->hasFormat("text/uri-list") && dee->mimeData()->urls().count() == 1) {
        QString fileName = dee->mimeData()->urls().at(0).toLocalFile();

        if ((!m_heightmapMode && isGCodeFile(fileName))
        || (m_heightmapMode && isHeightmapFile(fileName)))
            dee->acceptProposedAction();
    }
}

void frmMain::dropEvent(QDropEvent *de)
{    
    QString fileName = de->mimeData()->urls().at(0).toLocalFile();

    if (!m_heightmapMode) {
        if (!saveChanges(false)) return;

        // Load dropped g-code file
        if (!fileName.isEmpty()) {
            addRecentFile(fileName);
            updateRecentFilesMenu();
            loadFile(fileName);
        // Load dropped text
        } else {
            m_programFileName.clear();
            m_fileChanged = true;
            //@todo fix after refactoring loadFile to be faster
            //loadFile(de->mimeData()->text().split("\n"));
        }
    } else {
        if (!saveChanges(true)) return;

        // Load dropped heightmap file
        addRecentHeightmap(fileName);
        updateRecentFilesMenu();
        loadHeightmap(fileName);
    }
}

QMenu *frmMain::createPopupMenu()
{
    QMenu *menu = QMainWindow::createPopupMenu();

    foreach (QAction *a, menu->actions()) {
        if (a->text().contains("_spacer")) {
            a->setVisible(false);
        }
    } 

    return menu;
}

void frmMain::on_actFileNew_triggered()
{
    if (!saveChanges(m_heightmapMode)) return;

    if (!m_heightmapMode) {
        newFile();
    } else {
        newHeightmap();
    }
}

void frmMain::on_actFileOpen_triggered()
{
    on_cmdFileOpen_clicked();
}

void frmMain::on_actFileSave_triggered()
{
    if (!m_heightmapMode) {
        // G-code saving
        if (m_programFileName.isEmpty()) on_actFileSaveAs_triggered(); else {
            saveProgramToFile(m_programFileName, &m_programModel);
            m_fileChanged = false;
        }
    } else {
        // Height map saving
        if (m_heightmapFileName.isEmpty()) on_actFileSaveAs_triggered(); else saveHeightmap(m_heightmapFileName);
    }
}

void frmMain::on_actFileSaveAs_triggered()
{
    if (!m_heightmapMode) {
        QString fileName = (QFileDialog::getSaveFileName(this, tr("Save file as"), m_lastFolder, tr(FILE_FILTER_TEXT)));

        if (!fileName.isEmpty()) if (saveProgramToFile(fileName, &m_programModel)) {
            m_programFileName = fileName;
            m_fileChanged = false;

            addRecentFile(fileName);
            updateRecentFilesMenu();

            updateControlsState();
        }
    } else {
        QString fileName = (QFileDialog::getSaveFileName(this, tr("Save file as"), m_lastFolder, tr("Heightmap files (*.map)")));

        if (!fileName.isEmpty()) if (saveHeightmap(fileName)) {
            ui->txtHeightMap->setText(fileName.mid(fileName.lastIndexOf("/") + 1));
            m_heightmapFileName = fileName;
            m_heightmapChanged = false;

            addRecentHeightmap(fileName);
            updateRecentFilesMenu();

            updateControlsState();
        }
    }
}

void frmMain::on_actFileSaveTransformedAs_triggered()
{
    QString fileName = (QFileDialog::getSaveFileName(this, tr("Save file as"), m_lastFolder, tr(FILE_FILTER_TEXT)));

    if (!fileName.isEmpty()) {
        saveProgramToFile(fileName, &m_programHeightmapModel);
    }
}

void frmMain::on_actRecentClear_triggered()
{
    if (!m_heightmapMode) m_recentFiles.clear(); else m_recentHeightmaps.clear();
    updateRecentFilesMenu();
}

void frmMain::on_actFileExit_triggered()
{
    close();
}

void frmMain::on_actServiceSettings_triggered()
{
    QList<QAction*> acts = findChildren<QAction*>(QRegExp("act.*"));
    QTableWidget *table = m_settings->ui->tblShortcuts;

    table->clear();
    table->setColumnCount(3);
    table->setRowCount(acts.count());
    table->setHorizontalHeaderLabels(QStringList() << tr("Command") << tr("Text") << tr("Shortcuts"));

    table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    table->verticalHeader()->setFixedWidth(table->verticalHeader()->sizeHint().width() + 11);

    qSort(acts.begin(), acts.end(), frmMain::actionLessThan);
    for (int i = 0; i < acts.count(); i++) {
        table->setItem(i, 0, new QTableWidgetItem(acts.at(i)->objectName()));
        table->setItem(i, 1, new QTableWidgetItem(acts.at(i)->text().remove("&")));
        table->setItem(i, 2, new QTableWidgetItem(acts.at(i)->shortcut().toString()));

        table->item(i, 0)->setFlags(Qt::ItemIsEnabled);
        table->item(i, 1)->setFlags(Qt::ItemIsEnabled);
        table->item(i, 2)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
    }

    table->resizeColumnsToContents();
    table->setMinimumHeight(table->rowHeight(0) * 10
        + table->horizontalHeader()->height() + table->frameWidth() * 2);
    table->horizontalHeader()->setMinimumSectionSize(table->horizontalHeader()->sectionSize(2));
    table->horizontalHeader()->setStretchLastSection(true);

    emit settingsAboutToShow();

    if (m_settings->exec()) {
        // @TODO connection
        // if (m_settings->port() != "" && (m_settings->port() != m_serialPort.portName() ||
        //                                    m_settings->baud() != m_serialPort.baudRate())) {
        //     if (m_serialPort.isOpen()) m_serialPort.close();
        //     m_serialPort.setPortName(m_settings->port());
        //     m_serialPort.setBaudRate(m_settings->baud());
        //     openPort();
        // }

        m_configuration.save();

        updateControlsState();
        applySettings();

        // Update shortcuts
        for (int i = 0; i < acts.count(); i++) {
            acts[i]->setShortcut(QKeySequence(table->item(i, 2)->data(Qt::DisplayRole).toString()));
        }

        emit settingsAccepted();

    } else {
        // @TODO so weird!
        // m_settings->undo();

        emit settingsRejected();
    }
}

void frmMain::on_actServiceConfigureGRBL_triggered()
{
    m_grblConfigurator = new frmGrblConfigurator(this, m_communicator);
    m_grblConfigurator->exec();
    m_grblConfigurator->deleteLater();
}

void frmMain::on_actAbout_triggered()
{
    m_frmAbout.exec();
}

void frmMain::on_actJogStepNext_triggered()
{
    ui->cboJogStep->setCurrentNext();
}

void frmMain::on_actJogStepPrevious_triggered()
{
    ui->cboJogStep->setCurrentPrevious();
}

void frmMain::on_actJogFeedNext_triggered()
{
    ui->cboJogFeed->setCurrentNext();
}

void frmMain::on_actJogFeedPrevious_triggered()
{
    ui->cboJogFeed->setCurrentPrevious();
}

void frmMain::on_actSpindleSpeedPlus_triggered()
{
    ui->slbSpindle->setSliderPosition(ui->slbSpindle->sliderPosition() + 1);
}

void frmMain::on_actSpindleSpeedMinus_triggered()
{
    ui->slbSpindle->setSliderPosition(ui->slbSpindle->sliderPosition() - 1);
}

void frmMain::on_actOverrideFeedPlus_triggered()
{
    ui->slbFeedOverride->setSliderPosition(ui->slbFeedOverride->sliderPosition() + 1);
}

void frmMain::on_actOverrideFeedMinus_triggered()
{
    ui->slbFeedOverride->setSliderPosition(ui->slbFeedOverride->sliderPosition() - 1);
}

void frmMain::on_actOverrideRapidPlus_triggered()
{
    ui->slbRapidOverride->setSliderPosition(ui->slbRapidOverride->sliderPosition() + 1);
}

void frmMain::on_actOverrideRapidMinus_triggered()
{
    ui->slbRapidOverride->setSliderPosition(ui->slbRapidOverride->sliderPosition() - 1);
}

void frmMain::on_actOverrideSpindlePlus_triggered()
{
    ui->slbSpindleOverride->setSliderPosition(ui->slbSpindleOverride->sliderPosition() + 1);
}

void frmMain::on_actOverrideSpindleMinus_triggered()
{
    ui->slbSpindleOverride->setSliderPosition(ui->slbSpindleOverride->sliderPosition() - 1);
}

void frmMain::on_actViewLockWindows_toggled(bool checked)
{
    QList<QDockWidget*> dl = findChildren<QDockWidget*>();

    foreach (QDockWidget *d, dl) {
        d->setFeatures(checked ? QDockWidget::NoDockWidgetFeatures : QDockWidget::AllDockWidgetFeatures);
    }
}

void frmMain::on_cmdFileOpen_clicked()
{
    if (!m_communicator->isMachineConfigurationReady()) {
        return;
    }

    if (!m_heightmapMode) {
        loadFile("d:\\Obiekty3d\\TestGcode.nc");
        return;

        if (!saveChanges(false)) return;

        QString fileName  = QFileDialog::getOpenFileName(this, tr("Open"), m_lastFolder,
                                   tr(FILE_FILTER_TEXT";;All files (*.*)"));

        if (!fileName.isEmpty()) m_lastFolder = fileName.left(fileName.lastIndexOf(QRegExp("[/\\\\]+")));

        if (fileName != "") {
            addRecentFile(fileName);
            updateRecentFilesMenu();

            loadFile(fileName);
        }
    } else {
        if (!saveChanges(true)) return;

        QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), m_lastFolder, tr("Heightmap files (*.map)"));

        if (fileName != "") {
            addRecentHeightmap(fileName);
            updateRecentFilesMenu();
            loadHeightmap(fileName);
        }
    }
}

void frmMain::on_cmdFileSend_clicked()
{
    if (m_currentModel->rowCount() == 1) return;

    on_cmdFileReset_clicked();

    m_startTime.start();

    m_communicator->setSenderStateAndEmitSignal(SenderTransferring);

    m_storedKeyboardControl = ui->chkKeyboardControl->isChecked();
    ui->chkKeyboardControl->setChecked(false);

    m_communicator->storeParserState();

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        if (m_taskBarProgress) {
            m_taskBarProgress->setMaximum(m_currentModel->rowCount() - 2);
            m_taskBarProgress->setValue(0);
            m_taskBarProgress->show();
        }
    }
#endif

    updateControlsState();
    ui->cmdFilePause->setFocus();

    if (m_configuration.senderModule().useProgramStartCommands())
        m_communicator->sendCommands(CommandSource::ProgramAdditionalCommands, m_configuration.senderModule().programStartCommands());

    m_communicator->sendStreamerCommandsUntilBufferIsFull();
}

void frmMain::on_cmdFilePause_clicked(bool checked)
{
    static SenderState s;

    if (checked) {
        //PAUSE
        s = m_communicator->senderState();
        // setSenderState(SenderPaused);
        m_communicator->setSenderStateAndEmitSignal(SenderPausing);
        ui->cmdFilePause->setText(tr("Pausing..."));
        ui->cmdFilePause->setEnabled(false);
    } else {
        //RESUME
        if (m_communicator->senderState() == SenderChangingTool) {
            m_communicator->setSenderStateAndEmitSignal(SenderTransferring);
        } else {
            if (m_configuration.senderModule().usePauseCommands()) {
                m_communicator->sendCommands(CommandSource::ProgramAdditionalCommands, m_configuration.senderModule().afterPauseCommands());
            }
            m_communicator->setSenderStateAndEmitSignal(s);
        }
        updateControlsState();
    }
}

void frmMain::on_cmdFileAbort_clicked()
{
    m_communicator->abort();
}

void frmMain::on_cmdFileReset_clicked()
{
    m_streamer->reset();
    m_lastDrawnLineIndex = 0;
    m_communicator->m_probeIndex = -1;

    if (!m_heightmapMode) {
        QList<LineSegment*> list = m_viewParser.getLineSegmentList();

        QList<int> indexes;
        for (int i = 0; i < list.count(); i++) {
            list[i]->setDrawn(false);
            indexes.append(i);
        }
        m_codeDrawer->update(indexes);

        ui->tblProgram->setUpdatesEnabled(false);

        for (int i = 0; i < m_currentModel->data().count() - 1; i++) {
            m_currentModel->data()[i].state = GCodeItem::InQueue;
            m_currentModel->data()[i].response = QString();
        }
        ui->tblProgram->setUpdatesEnabled(true);

        ui->tblProgram->scrollTo(m_currentModel->index(0, 0));
        ui->tblProgram->clearSelection();
        ui->tblProgram->selectRow(0);

        ui->glwVisualizer->setSpendTime(QTime(0, 0, 0));
    } else {
        ui->txtHeightMapGridX->setEnabled(true);
        ui->txtHeightMapGridY->setEnabled(true);
        ui->txtHeightMapGridZBottom->setEnabled(true);
        ui->txtHeightMapGridZTop->setEnabled(true);

        delete m_heightmapInterpolationDrawer.data();
        m_heightmapInterpolationDrawer.setData(NULL);

        m_heightmapModel.clear();
        updateHeightmapGrid();
    }
}

// void frmMain::on_cmdCommandSend_clicked()
// {
//     QString command = ui->cboCommand->currentText();
//     if (command.isEmpty()) return;

//     ui->cboCommand->storeText();
//     ui->cboCommand->setCurrentText("");
//     m_communicator->sendCommand(command, COMMAND_TI_UI);
// }

// void frmMain::on_cmdClearConsole_clicked()
// {
//     ui->txtConsole->clear();
// }

void frmMain::on_cmdHome_clicked()
{
    m_communicator->m_homing = true;
    m_communicator->m_updateSpindleSpeed = true;
    m_communicator->sendCommand(CommandSource::GeneralUI, "$H", COMMAND_TI_UI);
}

void frmMain::on_cmdCheck_clicked(bool checked)
{
    if (checked) {
        m_communicator->storeParserState();
        m_communicator->sendCommand(CommandSource::GeneralUI, "$C", COMMAND_TI_UI);
    } else {
        m_communicator->m_aborting = true;
        m_communicator->reset();
//        grblReset();
    };
}

void frmMain::on_cmdReset_clicked()
{
    m_communicator->reset();
    //grblReset();
}

void frmMain::on_cmdUnlock_clicked()
{
    m_communicator->m_updateSpindleSpeed = true;
    m_communicator->sendCommand(CommandSource::GeneralUI, "$X", COMMAND_TI_UI);
}

void frmMain::on_cmdHold_clicked(bool checked)
{
    m_connection->sendByteArray(QByteArray(1, checked ? (char)'!' : (char)'~'));
}

void frmMain::on_cmdSleep_clicked()
{
    m_communicator->sendCommand(CommandSource::GeneralUI, "$SLP", COMMAND_TI_UI);
}

void frmMain::on_cmdDoor_clicked()
{
    m_connection->sendByteArray(QByteArray(1, (char)0x84));
}

void frmMain::on_cmdFlood_clicked()
{
    m_connection->sendByteArray(QByteArray(1, (char)0xa0));
}

void frmMain::on_cmdSpindle_toggled(bool checked)
{
    ui->grpSpindle->setProperty("overrided", checked);
    style()->unpolish(ui->grpSpindle);
    ui->grpSpindle->ensurePolished();

    if (checked) {
        if (!ui->grpSpindle->isChecked()) ui->grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(ui->slbSpindle->value()));
    } else {
        ui->grpSpindle->setTitle(tr("Spindle"));
    }
}

void frmMain::on_cmdSpindle_clicked(bool checked)
{
    if (ui->cmdHold->isChecked()) {
        m_connection->sendByteArray(QByteArray(1, char(0x9e)));
    } else {
        m_communicator->sendCommand(CommandSource::GeneralUI, checked ? QString("M3 S%1").arg(ui->slbSpindle->value()) : "M5", COMMAND_TI_UI);
    }
}

void frmMain::on_cmdTop_clicked()
{
    ui->glwVisualizer->setTopView();
}

void frmMain::on_cmdFront_clicked()
{
    ui->glwVisualizer->setFrontView();
}

void frmMain::on_cmdLeft_clicked()
{
    ui->glwVisualizer->setLeftView();
}

void frmMain::on_cmdIsometric_clicked()
{
    ui->glwVisualizer->setIsometricView();
}

void frmMain::on_cmdToggleProjection_clicked()
{
    ui->glwVisualizer->toggleProjectionType();
}

void frmMain::on_cmdFit_clicked()
{
    ui->glwVisualizer->fitDrawable(m_currentDrawer);
}

void frmMain::on_grpOverriding_toggled(bool checked)
{
    if (checked) {
        ui->grpOverriding->setTitle(tr("Overriding"));
    } else if (ui->slbFeedOverride->isChecked() | ui->slbRapidOverride->isChecked() | ui->slbSpindleOverride->isChecked()) {
        ui->grpOverriding->setTitle(tr("Overriding") + QString(tr(" (%1/%2/%3)"))
                                    .arg(ui->slbFeedOverride->isChecked() ? QString::number(ui->slbFeedOverride->value()) : "-")
                                    .arg(ui->slbRapidOverride->isChecked() ? QString::number(ui->slbRapidOverride->value()) : "-")
                                    .arg(ui->slbSpindleOverride->isChecked() ? QString::number(ui->slbSpindleOverride->value()) : "-"));
    }
    updateLayouts();

    ui->widgetFeed->setVisible(checked);
}

void frmMain::on_grpSpindle_toggled(bool checked)
{
    if (checked) {
        ui->grpSpindle->setTitle(tr("Spindle"));
    } else if (ui->cmdSpindle->isChecked()) {
//        ui->grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(ui->txtSpindleSpeed->text()));
        ui->grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(ui->slbSpindle->value()));
    }
    updateLayouts();

    ui->widgetSpindle->setVisible(checked);
}

void frmMain::on_grpJog_toggled(bool checked)
{
    updateJogTitle();
    updateLayouts();

    ui->widgetJog->setVisible(checked);
}

void frmMain::on_grpHeightMap_toggled(bool arg1)
{
    ui->widgetHeightMap->setVisible(arg1);
}

void frmMain::on_chkKeyboardControl_toggled(bool checked)
{
    ui->grpJog->setProperty("overrided", checked);
    style()->unpolish(ui->grpJog);
    ui->grpJog->ensurePolished();

    // Store/restore coordinate system
    if (checked) {
        //m_communicator->sendCommand(CommandSource::System, "$G", COMMAND_TI_UTIL1);
    } else {
        if (m_absoluteCoordinates) m_communicator->sendCommand(CommandSource::System, "G90", COMMAND_TI_UI);
    }

    if ((m_communicator->senderState() != SenderTransferring) && (m_communicator->senderState() != SenderStopping))
        m_storedKeyboardControl = checked;

    updateJogTitle();
    updateControlsState();
}

void frmMain::on_chkHeightMapBorderShow_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateControlsState();
}

void frmMain::on_chkHeightMapInterpolationShow_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateControlsState();
}

void frmMain::on_chkHeightMapUse_clicked(bool checked)
{
//    static bool fileChanged;

    // Reset table view
    QByteArray headerState = ui->tblProgram->horizontalHeader()->saveState();
    ui->tblProgram->setModel(NULL);

    CancelException cancel;

    if (checked) try {

        // Prepare progress dialog
        QProgressDialog progress(tr("Applying heightmap..."), tr("Abort"), 0, 0, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setFixedHeight(progress.sizeHint().height());
        progress.show();
        progress.setStyleSheet("QProgressBar {text-align: center; qproperty-format: \"\"}");

        // Set current model to prevent reseting heightmap cache
        updateCurrentModel(&m_programHeightmapModel);

        // Update heightmap-modificated program if not cached
        if (m_programHeightmapModel.rowCount() == 0) {

            // Modifying linesegments
            QList<LineSegment*> *list = m_viewParser.getLines();
            QRectF borderRect = borderRectFromTextboxes();
            double x, y, z;
            QVector3D point;

            progress.setLabelText(tr("Subdividing segments..."));
            progress.setMaximum(list->count() - 1);

            for (int i = 0; i < list->count(); i++) {
                if (!list->at(i)->isZMovement()) {
                    QList<LineSegment*> subSegments = subdivideSegment(list->at(i));

                    if (subSegments.count() > 0) {
                        delete list->at(i);
                        list->removeAt(i);
                        foreach (LineSegment* subSegment, subSegments) list->insert(i++, subSegment);
                        i--;
                    }
                }

                if (progress.isVisible() && (i % PROGRESSSTEP == 0)) {
                    progress.setMaximum(list->count() - 1);
                    progress.setValue(i);
                    qApp->processEvents();
                    if (progress.wasCanceled()) throw cancel;
                }
            }

            progress.setLabelText(tr("Updating Z-coordinates..."));
            progress.setMaximum(list->count() - 1);

            for (int i = 0; i < list->count(); i++) {
                if (i == 0) {
                    x = list->at(i)->getStart().x();
                    y = list->at(i)->getStart().y();
                    z = list->at(i)->getStart().z() + Interpolation::bicubicInterpolate(borderRect, &m_heightmapModel, x, y);
                    list->at(i)->setStart(QVector3D(x, y, z));
                } else list->at(i)->setStart(list->at(i - 1)->getEnd());

                x = list->at(i)->getEnd().x();
                y = list->at(i)->getEnd().y();
                z = list->at(i)->getEnd().z() + Interpolation::bicubicInterpolate(borderRect, &m_heightmapModel, x, y);
                list->at(i)->setEnd(QVector3D(x, y, z));

                if (progress.isVisible() && (i % PROGRESSSTEP == 0)) {
                    progress.setValue(i);
                    qApp->processEvents();
                    if (progress.wasCanceled()) throw cancel;
                }
            }

            progress.setLabelText(tr("Modifying G-code program..."));
            progress.setMaximum(m_programModel.rowCount() - 2);

            // Modifying g-code program
            QString command;
            QStringList args;
            int line;
            QString newCommand;
            GCodeItem item;
            int lastSegmentIndex = 0;
            int lastCommandIndex = -1;

            // Search strings
            QString coords("XxYyZzIiJjKkRr");
            QString g("Gg");
            QString m("Mm");

            char codeChar;          // Single code char G1 -> G
            float codeNum;          // Code number      G1 -> 1

            QString lastCode;
            bool isLinearMove;
            bool hasCommand;

            m_programLoading = true;
            for (int i = 0; i < m_programModel.rowCount() - 1; i++) {
                command = m_programModel.data().at(i).command;
                line = m_programModel.data().at(i).line;
                isLinearMove = false;
                hasCommand = false;

                if (line < 0 || line == lastCommandIndex || lastSegmentIndex == list->count() - 1) {
                    item.command = command;
                    m_programHeightmapModel.data().append(item);
                } else {
                    // Get commands args
                    args = m_programModel.data().at(i).args;
                    newCommand.clear();

                    // Parse command args
                    foreach (QString arg, args) {                   // arg examples: G1, G2, M3, X100...
                        codeChar = arg.at(0).toLatin1();            // codeChar: G, M, X...
                        if (!coords.contains(codeChar)) {           // Not parameter
                            codeNum = arg.mid(1).toDouble();
                            if (g.contains(codeChar)) {             // 'G'-command
                                // Store 'G0' & 'G1'
                                if (codeNum == 0.0f || codeNum == 1.0f) {
                                    lastCode = arg;
                                    isLinearMove = true;            // Store linear move
                                }

                                // Replace 'G2' & 'G3' with 'G1'
                                if (codeNum == 2.0f || codeNum == 3.0f) {
                                    newCommand.append("G1");
                                    isLinearMove = true;
                                // Drop plane command for arcs
                                } else if (codeNum != 17.0f && codeNum != 18.0f && codeNum != 19.0f) {
                                    newCommand.append(arg);
                                }

                                hasCommand = true;                  // Command has 'G'
                            } else {
                                if (m.contains(codeChar))
                                    hasCommand = true;              // Command has 'M'
                                newCommand.append(arg);       // Other commands
                            }
                        }
                    }

                    // Find first linesegment by command index
                    for (int j = lastSegmentIndex; j < list->count(); j++) {
                        if (list->at(j)->getLineNumber() == line) {
                            if (!qIsNaN(list->at(j)->getEnd().length()) && (isLinearMove || (!hasCommand && !lastCode.isEmpty()))) {
                                // Create new commands for each linesegment with given command index
                                while ((j < list->count()) && (list->at(j)->getLineNumber() == line)) {

                                    point = list->at(j)->getEnd();
                                    if (!list->at(j)->isAbsolute()) point -= list->at(j)->getStart();
                                    if (!list->at(j)->isMetric()) point /= 25.4f;

                                    item.command = newCommand + QString("X%1Y%2Z%3")
                                            .arg(point.x(), 0, 'f', 3).arg(point.y(), 0, 'f', 3).arg(point.z(), 0, 'f', 3);
                                    m_programHeightmapModel.data().append(item);

                                    if (!newCommand.isEmpty()) newCommand.clear();
                                    j++;
                                }
                            // Copy original command if not G0 or G1
                            } else {
                                item.command = command;
                                m_programHeightmapModel.data().append(item);
                            }

                            lastSegmentIndex = j;
                            break;
                        }
                    }
                }
                lastCommandIndex = line;

                if (progress.isVisible() && (i % PROGRESSSTEP == 0)) {
                    progress.setValue(i);
                    qApp->processEvents();
                    if (progress.wasCanceled()) throw cancel;
                }
            }
            m_programHeightmapModel.insertRow(m_programHeightmapModel.rowCount());
        }
        progress.close();

        ui->tblProgram->setModel(&m_programHeightmapModel);
        ui->tblProgram->horizontalHeader()->restoreState(headerState);

        connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));

        m_programLoading = false;

        // Update parser
        m_currentDrawer = m_codeDrawer;
        updateParser();

        // Select first row
        ui->tblProgram->selectRow(0);
    }
    catch (CancelException) {                       // Cancel modification
        m_programHeightmapModel.clear();
        updateCurrentModel(&m_programModel);

        ui->tblProgram->setModel(&m_programModel);
        ui->tblProgram->horizontalHeader()->restoreState(headerState);

        connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
        ui->tblProgram->selectRow(0);

        ui->chkHeightMapUse->setChecked(false);

        return;
    } else {                                        // Restore original program
        updateCurrentModel(&m_programModel);

        ui->tblProgram->setModel(&m_programModel);
        ui->tblProgram->horizontalHeader()->restoreState(headerState);

        connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));

        // Store changes flag
        bool fileChanged = m_fileChanged;

        // Update parser
        updateParser();

        // Select first row
        ui->tblProgram->selectRow(0);

        // Restore changes flag
        m_fileChanged = fileChanged;
    }

    // Update groupbox title
    ui->grpHeightMap->setProperty("overrided", checked);
    style()->unpolish(ui->grpHeightMap);
    ui->grpHeightMap->ensurePolished();

    // Update menu
    ui->actFileSaveTransformedAs->setVisible(checked);
}

void frmMain::on_chkHeightMapGridShow_toggled(bool checked)
{
    Q_UNUSED(checked)

    updateControlsState();
}

void frmMain::on_txtHeightMapBorderX_valueChanged(double arg1)
{
    updateHeightmapBorderDrawer();
    updateHeightmapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderWidth_valueChanged(double arg1)
{
    updateHeightmapBorderDrawer();
    updateHeightmapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderY_valueChanged(double arg1)
{
    updateHeightmapBorderDrawer();
    updateHeightmapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderHeight_valueChanged(double arg1)
{
    updateHeightmapBorderDrawer();
    updateHeightmapGrid(arg1);
}

void frmMain::on_txtHeightMapGridX_valueChanged(double arg1)
{
    updateHeightmapGrid(arg1);
}

void frmMain::on_txtHeightMapGridY_valueChanged(double arg1)
{
    updateHeightmapGrid(arg1);
}

void frmMain::on_txtHeightMapGridZBottom_valueChanged(double arg1)
{
    updateHeightmapGrid(arg1);
}

void frmMain::on_txtHeightMapGridZTop_valueChanged(double arg1)
{
    updateHeightmapGrid(arg1);
}

void frmMain::on_txtHeightMapInterpolationStepX_valueChanged(double arg1)
{
    Q_UNUSED(arg1)

    updateHeightMapInterpolationDrawer();
}

void frmMain::on_txtHeightMapInterpolationStepY_valueChanged(double arg1)
{
    Q_UNUSED(arg1)

    updateHeightMapInterpolationDrawer();
}

void frmMain::on_cmdHeightMapMode_toggled(bool checked)
{
    // Update flag
    m_heightmapMode = checked;

    // Reset file progress
    m_streamer->reset();
    m_lastDrawnLineIndex = 0;

    // Reset/restore g-code program modification on edit mode enter/exit
    if (ui->chkHeightMapUse->isChecked()) {
        on_chkHeightMapUse_clicked(!checked); // Update gcode program parser
    }

    if (checked) {
        ui->tblProgram->setModel(&m_probeModel);
        resizeTableHeightmapSections();
        updateCurrentModel(&m_programModel);
        m_currentDrawer = m_probeDrawer;
        updateParser();  // Update probe program parser
    } else {
        m_probeParser.reset();
        if (!ui->chkHeightMapUse->isChecked()) {
            ui->tblProgram->setModel(&m_programModel);
            connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
            ui->tblProgram->selectRow(0);

            resizeTableHeightmapSections();
            updateCurrentModel(&m_programModel);
            m_currentDrawer = m_codeDrawer;

            if (!ui->chkHeightMapUse->isChecked()) {
                ui->glwVisualizer->updateExtremes(m_codeDrawer);
                updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
            }
        }
    }

    // Shadow toolpath
    QList<LineSegment*> list = m_viewParser.getLineSegmentList();
    QList<int> indexes;
    for (int i = m_lastDrawnLineIndex; i < list.count(); i++) {
        list[i]->setDrawn(checked);
        list[i]->setIsHightlight(false);
        indexes.append(i);
    }
    // Update only vertex color.
    // If chkHeightMapUse was checked codeDrawer updated via updateParser
    if (!ui->chkHeightMapUse->isChecked()) m_codeDrawer->update(indexes);

    updateRecentFilesMenu();
    updateControlsState();
}

void frmMain::on_cmdHeightMapCreate_clicked()
{
    ui->cmdHeightMapMode->setChecked(true);
    on_actFileNew_triggered();
}

void frmMain::on_cmdHeightMapLoad_clicked()
{
    if (!saveChanges(true)) {
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), m_lastFolder, tr("Heightmap files (*.map)"));

    if (fileName != "") {
        addRecentHeightmap(fileName);
        loadHeightmap(fileName);

        // If using heightmap
        if (ui->chkHeightMapUse->isChecked() && !m_heightmapMode) {
            // Restore original file
            on_chkHeightMapUse_clicked(false);
            // Apply heightmap
            on_chkHeightMapUse_clicked(true);
        }

        updateRecentFilesMenu();
        updateControlsState(); // Enable 'cmdHeightMapMode' button
    }
}

void frmMain::on_cmdHeightMapBorderAuto_clicked()
{
    QRectF rect = borderRectFromExtremes();

    if (!qIsNaN(rect.width()) && !qIsNaN(rect.height())) {
        ui->txtHeightMapBorderX->setValue(rect.x());
        ui->txtHeightMapBorderY->setValue(rect.y());
        ui->txtHeightMapBorderWidth->setValue(rect.width());
        ui->txtHeightMapBorderHeight->setValue(rect.height());
    }
}

void frmMain::on_cmdYPlus_pressed()
{
    m_jogVector += QVector3D(0, 1, 0);
    jogStep();
}

void frmMain::on_cmdYPlus_released()
{
    m_jogVector -= QVector3D(0, 1, 0);
    jogStep();
}

void frmMain::on_cmdYMinus_pressed()
{
    m_jogVector += QVector3D(0, -1, 0);
    jogStep();
}

void frmMain::on_cmdYMinus_released()
{
    m_jogVector -= QVector3D(0, -1, 0);
    jogStep();
}

void frmMain::on_cmdXPlus_pressed()
{
    m_jogVector += QVector3D(1, 0, 0);
    jogStep();
}

void frmMain::on_cmdXPlus_released()
{
    m_jogVector -= QVector3D(1, 0, 0);
    jogStep();
}

void frmMain::on_cmdXMinus_pressed()
{
    m_jogVector += QVector3D(-1, 0, 0);
    jogStep();
}

void frmMain::on_cmdXMinus_released()
{
    m_jogVector -= QVector3D(-1, 0, 0);
    jogStep();
}

void frmMain::on_cmdZPlus_pressed()
{
    m_jogVector += QVector3D(0, 0, 1);
    jogStep();
}

void frmMain::on_cmdZPlus_released()
{
    m_jogVector -= QVector3D(0, 0, 1);
    jogStep();
}

void frmMain::on_cmdZMinus_pressed()
{
    m_jogVector += QVector3D(0, 0, -1);
    jogStep();
}

void frmMain::on_cmdZMinus_released()
{
    m_jogVector -= QVector3D(0, 0, -1);
    jogStep();
}

void frmMain::on_cmdStop_clicked()
{
    m_jogVector = QVector3D(0, 0, 0);
    m_communicator->clearQueue();
    m_connection->sendByteArray(QByteArray(1, char(0x85)));
}

void frmMain::on_tblProgram_customContextMenuRequested(const QPoint &pos)
{
    if (m_communicator->senderState() != SenderStopped) return;

    if (ui->tblProgram->selectionModel()->selectedRows().count() > 0) {
        m_tableMenu->actions().at(0)->setEnabled(true);
        m_tableMenu->actions().at(1)->setEnabled(ui->tblProgram->selectionModel()->selectedRows()[0].row() != m_currentModel->rowCount() - 1);
    } else {
        m_tableMenu->actions().at(0)->setEnabled(false);
        m_tableMenu->actions().at(1)->setEnabled(false);
    }
    m_tableMenu->popup(ui->tblProgram->viewport()->mapToGlobal(pos));
}

void frmMain::on_mnuViewWindows_aboutToShow()
{
    QAction *a;
    QList<QAction*> al;

    foreach (QDockWidget *d, findChildren<QDockWidget*>()) {
        a = new QAction(d->windowTitle(), ui->mnuViewWindows);
        a->setCheckable(true);
        a->setChecked(d->isVisible());
        connect(a, &QAction::triggered, d, &QDockWidget::setVisible);
        al.append(a);
    }

    qSort(al.begin(), al.end(), frmMain::actionTextLessThan);

    ui->mnuViewWindows->clear();
    ui->mnuViewWindows->addActions(al);
}

void frmMain::on_mnuViewPanels_aboutToShow()
{
    QAction *a;

    ui->mnuViewPanels->clear();

    QStringList panels;
    if (ui->scrollContentsDevice->isVisible()) panels << ui->scrollContentsDevice->saveState();
    if (ui->scrollContentsModification->isVisible()) panels << "\n" << ui->scrollContentsModification->saveState();
    if (ui->scrollContentsUser->isVisible()) panels << "\n" << ui->scrollContentsUser->saveState();

    foreach (QString s, panels) {
        if (s == "\n") {
            ui->mnuViewPanels->addSeparator();
        } else {
            QGroupBox *b = findChild<QGroupBox*>(s);
            if (b) {
                a = ui->mnuViewPanels->addAction(b->title());
                a->setCheckable(true);
                a->setChecked(b->isVisible());
                connect(a, &QAction::triggered, b, &QWidget::setVisible);
            }
        }
    }
}

void frmMain::on_dockVisualizer_visibilityChanged(bool visible)
{
    ui->glwVisualizer->setUpdatesEnabled(visible);
}

void frmMain::onConnectionError(QString error)
{
    // @TODO connection
    // if (error != QSerialPort::NoError && error != previousError) {
    //     previousError = error;
    //     ui->txtConsole->appendPlainText(tr("Serial port error ") + QString::number(error) + ": " + m_serialPort.errorString());
    //     if (m_serialPort.isOpen()) {
    //         m_serialPort.close();
    //         updateControlsState();
    //     }
    // }
    m_partConsole->append(tr("Connection error ") + error);
    updateControlsState();
}

void frmMain::onMachinePosChanged(QVector3D pos)
{
    this->m_partState->setMachineCoordinates(pos);
}

void frmMain::onWorkPosChanged(QVector3D pos)
{
    this->m_partState->setWorkCoordinates(pos);
}

void frmMain::onDeviceStateChanged(DeviceState state)
{
    this->m_partState->setStatusText(m_statusCaptions[state], m_statusBackColors[state], m_statusForeColors[state]);

    ui->cmdCheck->setEnabled(state != DeviceRun && (m_communicator->senderState() == SenderStopped));
    ui->cmdCheck->setChecked(state == DeviceCheck);
    ui->cmdHold->setChecked(state == DeviceHold0 || state == DeviceHold1 || state == DeviceQueue);
    ui->cmdSpindle->setEnabled(state == DeviceHold0 || ((m_communicator->senderState() != SenderTransferring) &&
                                                        (m_communicator->senderState() != SenderStopping)));
}

void frmMain::onDeviceStateReceived(DeviceState state)
{
    // Update controls state
    ui->cmdCheck->setEnabled(state != DeviceRun && (m_communicator->senderState() == SenderStopped));
    ui->cmdCheck->setChecked(state == DeviceCheck);
    ui->cmdHold->setChecked(state == DeviceHold0 || state == DeviceHold1 || state == DeviceQueue);
    ui->cmdSpindle->setEnabled(state == DeviceHold0 || ((m_communicator->senderState() != SenderTransferring) &&
                                                        (m_communicator->senderState() != SenderStopping)));
    // Update "elapsed time" timer
    if ((m_communicator->senderState() == SenderTransferring) || (m_communicator->senderState() == SenderStopping)) {
        QTime time(0, 0, 0);
        int elapsed = m_startTime.elapsed();
        ui->glwVisualizer->setSpendTime(time.addMSecs(elapsed));
    }
}

void frmMain::onSenderStateReceived(SenderState state)
{
    Q_UNUSED(state);

    updateControlsState();
}

void frmMain::onSpindleStateReceived(bool state)
{
    switch (state) {
        case true:
            m_timerToolAnimation.start(25, this);
            ui->cmdSpindle->setChecked(true);
            break;
        default:
            m_timerToolAnimation.stop();
            ui->cmdSpindle->setChecked(false);
            break;
    }
}

void frmMain::onFloodStateReceived(bool state)
{
    ui->cmdFlood->setChecked(state);
}

void frmMain::onParserStateReceived(QString state)
{
    ui->glwVisualizer->setParserState(state);
}

void frmMain::onPinStateReceived(QString state)
{
    ui->glwVisualizer->setPinState(state);
}

void frmMain::onFeedSpindleSpeedReceived(int feedRate, int spindleSpeed)
{
    ui->glwVisualizer->setSpeedState((QString(tr("F/S: %1 / %2")).arg(feedRate, spindleSpeed)));
}

void frmMain::onSpindleSpeedReceived(int spindleSpeed)
{
    ui->slbSpindle->setCurrentValue(spindleSpeed);
}

void frmMain::onOverridesReceived(int feedOverride, int spindleOverride, int rapidOverride)
{
    updateOverride(ui->slbFeedOverride, feedOverride, '\x91');
    updateOverride(ui->slbSpindleOverride, spindleOverride, '\x9a');

    ui->slbRapidOverride->setCurrentValue(rapidOverride);

    int target = ui->slbRapidOverride->isChecked() ? ui->slbRapidOverride->value() : 100;

    if (rapidOverride != target) {
        switch (target) {
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
    }
}

void frmMain::onAborted()
{
    //ui->cmdFileAbort->setEnabled(false);
    updateControlsState();
}

void frmMain::onResponseReceived(QString command, int tableIndex, QString response)
{
    Q_UNUSED(command)
    Q_UNUSED(tableIndex)
    Q_UNUSED(response)

    updateToolpathShadowingOnCheckMode();
}

void frmMain::onCommandResponseReceived(CommandAttributes commandAttributes)
{
    m_partConsole->appendResponse(commandAttributes);
}

void frmMain::onCommandSent(CommandAttributes commandAttributes)
{
    if (isCommandFromUi(commandAttributes.source) && !m_configuration.consoleModule().showUiCommands()) return;
    if (isCommandFromProgram(commandAttributes.source) && !m_configuration.consoleModule().showProgramCommands()) return;
    m_partConsole->append(commandAttributes);
}

void frmMain::onCommandProcessed(int tableIndex, QString response)
{
    if (ui->chkAutoScrollGCode->isChecked()) {
        // scroll to NEXT command (+1)
        ui->tblProgram->scrollTo(m_currentModel->index(tableIndex + 1, 0));      // TODO: Update by timer
        ui->tblProgram->setCurrentIndex(m_currentModel->index(tableIndex + 1, 1));
    }

    if (tableIndex > -1) {
        m_currentModel->setData(m_currentModel->index(tableIndex, 2), GCodeItem::Processed);
        m_currentModel->setData(m_currentModel->index(tableIndex, 3), response);
    }
}

void frmMain::onConfigurationReceived(MachineConfiguration configuration, QMap<int, double>)
{
    m_partState->setUnits(configuration.units());
}

void frmMain::onToolPositionReceived(QVector3D pos)
{
    updateToolPositionAndToolpathShadowing(pos);
}

void frmMain::onConsoleNewCommand(QString command)
{
    m_communicator->sendCommand(CommandSource::Console, command, COMMAND_TI_UI);
}

void frmMain::onTimerConnection()
{
    if (!m_connection->isConnected()) {
        openPort();

        return;
    }

    // @TODO move it completely to communicator
    m_communicator->processConnectionTimer();
}

// @todo another way to update visualizer??
// void frmMain::onTimerStateQuery()
// {
//     if (m_connection->isConnected() && m_communicator->m_resetCompleted && m_communicator->m_statusReceived) {
//         m_connection->sendByteArray(QByteArray(1, '?'));
//         m_communicator->m_statusReceived = false;
//     }

//     ui->glwVisualizer->setBufferState(QString(tr("Buffer: %1 / %2 / %3")).arg(bufferLength()).arg(m_communicator->m_commands.length()).arg(m_communicator->m_queue.length()));
// }

void frmMain::onTableInsertLine()
{
    if (ui->tblProgram->selectionModel()->selectedRows().count() == 0 || 
        (m_communicator->senderState() == SenderTransferring) || (m_communicator->senderState() == SenderStopping)) return;

    int row = ui->tblProgram->selectionModel()->selectedRows()[0].row();

    m_currentModel->insertRow(row);
    m_currentModel->setData(m_currentModel->index(row, 2), GCodeItem::InQueue);

    updateParser();

    ui->tblProgram->selectRow(row);
}

void frmMain::onTableDeleteLines()
{
    if (ui->tblProgram->selectionModel()->selectedRows().count() == 0 || 
        (m_communicator->senderState() == SenderTransferring) || (m_communicator->senderState() == SenderStopping) ||
        QMessageBox::warning(this, this->windowTitle(), tr("Delete lines?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) return;

    QModelIndex firstRow = ui->tblProgram->selectionModel()->selectedRows()[0];
    int rowsCount = ui->tblProgram->selectionModel()->selectedRows().count();
    if (ui->tblProgram->selectionModel()->selectedRows().last().row() == m_currentModel->rowCount() - 1) rowsCount--;

    if (firstRow.row() != m_currentModel->rowCount() - 1) {
        m_currentModel->removeRows(firstRow.row(), rowsCount);
    } else return;

    // Drop heightmap cache
    if (m_currentModel == &m_programModel) m_programHeightmapModel.clear();

    updateParser();

    ui->tblProgram->selectRow(firstRow.row());
}

void frmMain::onTableCellChanged(QModelIndex i1, QModelIndex i2)
{
    Q_UNUSED(i2)

    GCodeTableModel *model = (GCodeTableModel*)sender();

    if (i1.column() != 1) return;
    // Inserting new line at end
    if (i1.row() == (model->rowCount() - 1) && model->data(model->index(i1.row(), 1)).toString() != "") {
        model->setData(model->index(model->rowCount() - 1, 2), GCodeItem::InQueue);
        model->insertRow(model->rowCount());
        if (!m_programLoading) ui->tblProgram->setCurrentIndex(model->index(i1.row() + 1, 1));
    }

    if (!m_programLoading) {
        // Clear cached args
        model->setData(model->index(i1.row(), 5), QVariant());

        // Drop heightmap cache
        if (m_currentModel == &m_programModel) m_programHeightmapModel.clear();

        // Update visualizer
        updateParser();

        // Hightlight w/o current cell changed event (double hightlight on current cell changed)
        QList<LineSegment*> list = m_viewParser.getLineSegmentList();
        for (int i = 0; i < list.count() && list[i]->getLineNumber() <= m_currentModel->data(m_currentModel->index(i1.row(), 4)).toInt(); i++) {
            list[i]->setIsHightlight(true);
        }
    }
}

void frmMain::onTableCurrentChanged(QModelIndex idx1, QModelIndex idx2)
{
    // Update toolpath hightlighting
    if (idx1.row() > m_currentModel->rowCount() - 2) idx1 = m_currentModel->index(m_currentModel->rowCount() - 2, 0);
    if (idx2.row() > m_currentModel->rowCount() - 2) idx2 = m_currentModel->index(m_currentModel->rowCount() - 2, 0);

    GcodeViewParse *parser = m_currentDrawer->viewParser();
    QList<LineSegment*> list = parser->getLineSegmentList();
    QVector<QList<int>> lineIndexes = parser->getLinesIndexes();

    // Update linesegments on cell changed
    if (!m_currentDrawer->geometryUpdated()) {
        for (int i = 0; i < list.count(); i++) {
            list.at(i)->setIsHightlight(list.at(i)->getLineNumber() <= m_currentModel->data(m_currentModel->index(idx1.row(), 4)).toInt());
        }
    // Update vertices on current cell changed
    } else {

        int lineFirst = m_currentModel->data(m_currentModel->index(idx1.row(), 4)).toInt();
        int lineLast = m_currentModel->data(m_currentModel->index(idx2.row(), 4)).toInt();
        if (lineLast < lineFirst) qSwap(lineLast, lineFirst);

        QList<int> indexes;
        for (int i = lineFirst + 1; i <= lineLast; i++) {
            foreach (int l, lineIndexes.at(i)) {
                list.at(l)->setIsHightlight(idx1.row() > idx2.row());
                indexes.append(l);
            }
        }

        m_selectionDrawer.setEndPosition(indexes.isEmpty() ? QVector3D(sNan, sNan, sNan) :
            (m_codeDrawer->getIgnoreZ() ? QVector3D(list.at(indexes.last())->getEnd().x(), list.at(indexes.last())->getEnd().y(), 0)
                                        : list.at(indexes.last())->getEnd()));
        m_selectionDrawer.update();

        if (!indexes.isEmpty()) m_currentDrawer->update(indexes);
    }

    // Update selection marker
    int line = m_currentModel->data(m_currentModel->index(idx1.row(), 4)).toInt();
    if (line > 0 && line < lineIndexes.count() && !lineIndexes.at(line).isEmpty()) {
        QVector3D pos = list.at(lineIndexes.at(line).last())->getEnd();
        m_selectionDrawer.setEndPosition(m_codeDrawer->getIgnoreZ() ? QVector3D(pos.x(), pos.y(), 0) : pos);
    } else {
        m_selectionDrawer.setEndPosition(QVector3D(sNan, sNan, sNan));
    }
    m_selectionDrawer.update();
}

void frmMain::onOverridingToggled(bool checked)
{
    Q_UNUSED(checked)

    ui->grpOverriding->setProperty("overrided", ui->slbFeedOverride->isChecked()
                                   || ui->slbRapidOverride->isChecked() || ui->slbSpindleOverride->isChecked());
    style()->unpolish(ui->grpOverriding);
    ui->grpOverriding->ensurePolished();
}

void frmMain::onOverrideChanged()
{
    updateProgramEstimatedTime(m_currentDrawer->viewParser()->getLineSegmentList());
}

void frmMain::onActRecentFileTriggered()
{
    QAction *action = static_cast<QAction*>(sender());
    QString fileName = action->text();

    if (action != NULL) {
        if (!saveChanges(m_heightmapMode)) return;
        if (!m_heightmapMode) loadFile(fileName); else loadHeightmap(fileName);
    }
}

void frmMain::onActSendFromLineTriggered()
{
    if (m_currentModel->rowCount() == 1) return;

    //Line to start from
    int commandIndex = ui->tblProgram->currentIndex().row();

    // Set parser state
    if (m_configuration.senderModule().setParserStateBeforeSendingFromSelectedLine()) {
        QString commands = getLineInitCommands(commandIndex);

        QMessageBox box(this);
        box.setIcon(QMessageBox::Information);
        box.setText(tr("Following commands will be sent before selected line:\n") + commands);
        box.setWindowTitle(qApp->applicationDisplayName());
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        box.addButton(tr("Skip"), QMessageBox::DestructiveRole);

        int res = box.exec();
        if (res == QMessageBox::Cancel) return;
        else if (res == QMessageBox::Ok) {
            // foreach (QString command, commands) {
            //     sendCommand(command, COMMAND_TI_UI);
            // }
            m_communicator->sendCommands(CommandSource::ProgramAdditionalCommands, commands, COMMAND_TI_UI);
        }
    }

    m_streamer->reset(commandIndex);
    m_lastDrawnLineIndex = 0;
    m_communicator->m_probeIndex = -1;

    QList<LineSegment*> list = m_viewParser.getLineSegmentList();

    QList<int> indexes;
    for (int i = 0; i < list.count(); i++) {
        list[i]->setDrawn(list.at(i)->getLineNumber() < m_currentModel->data().at(commandIndex).line);
        indexes.append(i);
    }
    m_codeDrawer->update(indexes);

    ui->tblProgram->setUpdatesEnabled(false);

    for (int i = 0; i < m_currentModel->data().count() - 1; i++) {
        m_currentModel->data()[i].state = i < commandIndex ? GCodeItem::Skipped : GCodeItem::InQueue;
        m_currentModel->data()[i].response = QString();
    }
    ui->tblProgram->setUpdatesEnabled(true);
    ui->glwVisualizer->setSpendTime(QTime(0, 0, 0));

    m_startTime.start();

    m_communicator->setSenderStateAndEmitSignal(SenderTransferring);

    m_storedKeyboardControl = ui->chkKeyboardControl->isChecked();
    ui->chkKeyboardControl->setChecked(false);

    m_communicator->storeParserState();

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
        if (m_taskBarProgress) {
            m_taskBarProgress->setMaximum(m_currentModel->rowCount() - 2);
            m_taskBarProgress->setValue(commandIndex);
            m_taskBarProgress->show();
        }
    }
#endif

    updateControlsState();
    ui->cmdFilePause->setFocus();

    m_streamer->reset(commandIndex);
    m_communicator->sendStreamerCommandsUntilBufferIsFull();
}

void frmMain::onSlbSpindleValueUserChanged()
{
    m_communicator->m_updateSpindleSpeed = true;
}

void frmMain::onSlbSpindleValueChanged()
{
    if (!ui->grpSpindle->isChecked() && ui->cmdSpindle->isChecked())
        ui->grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(ui->slbSpindle->value()));
}

// void frmMain::onCboCommandReturnPressed()
// {
//     QString command = ui->cboCommand->currentText();
//     if (command.isEmpty()) return;

//     ui->cboCommand->setCurrentText("");
//     m_communicator->sendCommand(command, COMMAND_TI_UI);
// }

void frmMain::onDockTopLevelChanged(bool topLevel)
{
    Q_UNUSED(topLevel)
    static_cast<QWidget*>(sender())->setStyleSheet("");
}

void frmMain::onScroolBarAction(int action)
{
    Q_UNUSED(action)

    if ((m_communicator->senderState() == SenderTransferring) || (m_communicator->senderState() == SenderStopping))
        ui->chkAutoScrollGCode->setChecked(false);
}

void frmMain::onToolPos(QPointF pos)
{
    m_toolDrawer.setToolPosition(
        QVector3D(pos.x(), pos.y(), 0)
    );
    qDebug() << "Tool position" << pos;
}

void frmMain::updateHeightMapInterpolationDrawer(bool reset)
{
    if (m_settingsLoading) return;

    QRectF borderRect = borderRectFromTextboxes();
    m_heightmapInterpolationDrawer.setBorderRect(borderRect);

    QVector<QVector<double>> *interpolationData = new QVector<QVector<double>>;

    int interpolationPointsX = ui->txtHeightMapInterpolationStepX->value();// * (ui->txtHeightMapGridX->value() - 1) + 1;
    int interpolationPointsY = ui->txtHeightMapInterpolationStepY->value();// * (ui->txtHeightMapGridY->value() - 1) + 1;

    double interpolationStepX = interpolationPointsX > 1 ? borderRect.width() / (interpolationPointsX - 1) : 0;
    double interpolationStepY = interpolationPointsY > 1 ? borderRect.height() / (interpolationPointsY - 1) : 0;

    for (int i = 0; i < interpolationPointsY; i++) {
        QVector<double> row;
        for (int j = 0; j < interpolationPointsX; j++) {

            double x = interpolationStepX * j + borderRect.x();
            double y = interpolationStepY * i + borderRect.y();

            row.append(reset ? qQNaN() : Interpolation::bicubicInterpolate(borderRect, &m_heightmapModel, x, y));
        }
        interpolationData->append(row);
    }

    if (m_heightmapInterpolationDrawer.data() != NULL) {
        delete m_heightmapInterpolationDrawer.data();
    }
    m_heightmapInterpolationDrawer.setData(interpolationData);

    // Update grid drawer
    m_heightmapGridDrawer.update();

    // Heightmap changed by table user input
    if (sender() == &m_heightmapModel) m_heightmapChanged = true;

    // Reset heightmapped program model
    m_programHeightmapModel.clear();
}

void frmMain::placeVisualizerButtons()
{
    ui->cmdIsometric->move(ui->glwVisualizer->width() - ui->cmdIsometric->width() - 8, 8);
    ui->cmdTop->move(ui->cmdIsometric->geometry().left() - ui->cmdTop->width() - 8, 8);
    ui->cmdLeft->move(ui->glwVisualizer->width() - ui->cmdLeft->width() - 8, ui->cmdIsometric->geometry().bottom() + 8);
    ui->cmdFront->move(ui->cmdLeft->geometry().left() - ui->cmdFront->width() - 8, ui->cmdIsometric->geometry().bottom() + 8);
    ui->cmdFit->move(ui->glwVisualizer->width() - ui->cmdFit->width() - 8, ui->cmdLeft->geometry().bottom() + 8);
    ui->cmdToggleProjection->move(ui->cmdFit->geometry().left() - ui->cmdToggleProjection->width() - 8, ui->cmdLeft->geometry().bottom() + 8);
}

void frmMain::preloadSettings()
{
    ConfigurationUI &uiConfiguration = m_configuration.uiModule();
    ConfigurationVisualizer &visualizerConfiguration = m_configuration.visualizerModule();

    qApp->setStyleSheet(
        QString(qApp->styleSheet()).replace(QRegExp("font-size:[^;^\\}]+"), QString("font-size: %1pt").arg(uiConfiguration.fontSize()))
    );

    // Update v-sync in glformat
    QGLFormat fmt = QGLFormat::defaultFormat();
    fmt.setSwapInterval(visualizerConfiguration.vsync() ? 1 : 0);
    QGLFormat::setDefaultFormat(fmt);
}

void frmMain::applyHeightmapConfiguration(ConfigurationHeightmap &configurationHeightmap)
{
    ui->txtHeightMapBorderX->setValue(configurationHeightmap.borderX());
    ui->txtHeightMapBorderY->setValue(configurationHeightmap.borderY());
    ui->txtHeightMapBorderWidth->setValue(configurationHeightmap.borderWidth());
    ui->txtHeightMapBorderHeight->setValue(configurationHeightmap.borderHeight());
    ui->chkHeightMapBorderShow->setChecked(configurationHeightmap.borderShow());

    ui->txtHeightMapGridX->setValue(configurationHeightmap.gridX());
    ui->txtHeightMapGridY->setValue(configurationHeightmap.gridY());
    ui->txtHeightMapGridZTop->setValue(configurationHeightmap.gridZTop());
    ui->txtHeightMapGridZBottom->setValue(configurationHeightmap.gridZBottom());
    ui->txtHeightMapProbeFeed->setValue(configurationHeightmap.probeFeed());
    ui->chkHeightMapGridShow->setChecked(configurationHeightmap.gridShow());

    ui->txtHeightMapInterpolationStepX->setValue(configurationHeightmap.interpolationStepX());
    ui->txtHeightMapInterpolationStepY->setValue(configurationHeightmap.interpolationStepY());
    ui->cboHeightMapInterpolationType->setCurrentIndex(configurationHeightmap.interpolationType());
    ui->chkHeightMapInterpolationShow->setChecked(configurationHeightmap.interpolationShow());
}

void frmMain::applyOverridesConfiguration(ConfigurationMachine &machineConfiguration)
{
    ui->slbFeedOverride->setChecked(machineConfiguration.overrideFeed());
    ui->slbFeedOverride->setValue(machineConfiguration.overrideFeedValue());

    ui->slbRapidOverride->setChecked(machineConfiguration.overrideRapid());
    ui->slbRapidOverride->setValue(machineConfiguration.overrideRapidValue());

    ui->slbSpindleOverride->setChecked(machineConfiguration.overrideSpindleSpeed());
    ui->slbSpindleOverride->setValue(machineConfiguration.overrideSpindleSpeedValue());
}

void frmMain::applySpindleConfiguration(ConfigurationMachine &machineConfiguration)
{
    ui->slbSpindle->setRatio(machineConfiguration.spindleSpeedRatio());
    ui->slbSpindle->setMinimum(machineConfiguration.spindleSpeedRange().min);
    ui->slbSpindle->setMaximum(machineConfiguration.spindleSpeedRange().max);
    ui->slbSpindle->setValue(machineConfiguration.spindleSpeed());
}

void frmMain::applyRecentFilesConfiguration(QSettings &set)
{
    m_recentFiles = set.value("recentFiles", QStringList()).toStringList();
    m_recentHeightmaps = set.value("recentHeightmaps", QStringList()).toStringList();
    m_lastFolder = set.value("lastFolder", QDir::homePath()).toString();
}

void frmMain::loadSettings()
{
    QSettings set(m_settingsFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    m_settingsLoading = true;

    emit settingsAboutToLoad();

    applyRecentFilesConfiguration(set);

    this->restoreGeometry(set.value("formGeometry", QByteArray()).toByteArray());

    // ui->cboCommand->setMinimumHeight(ui->cboCommand->height());
    // ui->cmdClearConsole->setFixedHeight(ui->cboCommand->height());
    // ui->cmdCommandSend->setFixedHeight(ui->cboCommand->height());

    m_storedKeyboardControl = set.value("keyboardControl", false).toBool();

    // m_settings->setAutoCompletion(set.value("autoCompletion", true).toBool());

    QStringList steps = set.value("jogSteps").toStringList();
    if (steps.count() > 0) {
        steps.insert(0, ui->cboJogStep->items().first());
        ui->cboJogStep->setItems(steps);
    }
    ui->cboJogStep->setCurrentIndex(ui->cboJogStep->findText(set.value("jogStep").toString()));
    ui->cboJogFeed->setItems(set.value("jogFeeds").toStringList());
    ui->cboJogFeed->setCurrentIndex(ui->cboJogFeed->findText(set.value("jogFeed").toString()));

    // foreach (ColorPicker* pick, m_settings->colors()) {
    //     pick->setColor(QColor(set.value(pick->objectName().mid(3), "black").toString()));
    // }

    updateRecentFilesMenu();

    ui->tblProgram->horizontalHeader()->restoreState(set.value("header", QByteArray()).toByteArray());

    // Apply settings
    applySettings();

    // Restore last commands list
    // ui->cboCommand->addItems(set.value("recentCommands", QStringList()).toStringList());
    // ui->cboCommand->setCurrentIndex(-1);

    // Adjust docks width 
    int w = qMax(ui->dockDevice->widget()->sizeHint().width(), 
        ui->dockModification->widget()->sizeHint().width());
    ui->dockDevice->setMinimumWidth(w);
    ui->dockDevice->setMaximumWidth(w + ui->scrollArea->verticalScrollBar()->width());
    ui->dockModification->setMinimumWidth(w);
    ui->dockModification->setMaximumWidth(w + ui->scrollArea->verticalScrollBar()->width());
    ui->dockUser->setMinimumWidth(w);
    ui->dockUser->setMaximumWidth(w + ui->scrollArea->verticalScrollBar()->width());

    // Buttons
    int b = (w - ui->grpControl->layout()->margin() * 2 - ui->grpControl->layout()->spacing() * 3) / 4 * 0.8;
    int c = b * 0.8;
    setStyleSheet(styleSheet() + QString("\nStyledToolButton[adjustSize='true'] {\n\
	    min-width: %1px;\n\
	    min-height: %1px;\n\
	    qproperty-iconSize: %2px;\n\
        }").arg(b).arg(c));
    ensurePolished();

    foreach (QDockWidget *w, findChildren<QDockWidget*>()) w->setStyleSheet("");

    // Restore docks
    // Signals/slots
    foreach (QDockWidget *w, findChildren<QDockWidget*>())
        connect(w, &QDockWidget::topLevelChanged, this, &frmMain::onDockTopLevelChanged);

    // Panels
    ui->scrollContentsDevice->restoreState(this, set.value("panelsDevice").toStringList());
    ui->scrollContentsModification->restoreState(this, set.value("panelsModification").toStringList());
    ui->scrollContentsUser->restoreState(this, set.value("panelsUser").toStringList());

    QStringList hiddenPanels = set.value("hiddenPanels").toStringList();
    foreach (QString s, hiddenPanels) {
        QGroupBox *b = findChild<QGroupBox*>(s);
        if (b) b->setHidden(true);
    }    

    QStringList collapsedPanels = set.value("collapsedPanels").toStringList();
    foreach (QString s, collapsedPanels) {
        QGroupBox *b = findChild<QGroupBox*>(s);
        if (b) b->setChecked(false);
    }

        // Normal window state
    restoreState(set.value("formMainState").toByteArray());

        // Maximized window state
    show();
    qApp->processEvents();    
    restoreState(set.value("formMainState").toByteArray());

    // Setup coords textboxes
    // @TODO do we need this here?
    // setupCoordsTextboxes();

    // Settings form geometry
    // m_settings->restoreGeometry(set.value("formSettingsGeometry").toByteArray());
    m_settings->ui->splitMain->restoreState(set.value("settingsSplitMain").toByteArray());

    emit settingsLoaded();

    // Shortcuts
    qRegisterMetaTypeStreamOperators<ShortcutsMap>("ShortcutsMap");
    
    ShortcutsMap m;
    QByteArray ba = set.value("shortcuts").toByteArray();
    QDataStream s(&ba, QIODevice::ReadOnly);
    
    s >> m;
    for (int i = 0; i < m.count(); i++) {
        QAction *a = findChild<QAction*>(m.keys().at(i));
        if (a) a->setShortcuts(m.values().at(i));
    }

    // Menu
    ui->actViewLockWindows->setChecked(set.value("lockWindows").toBool());
    ui->actViewLockPanels->setChecked(set.value("lockPanels").toBool());

    m_settings->restoreGeometry(set.value("formSettingsGeometry", m_settings->saveGeometry()).toByteArray());

    set.endGroup();

    m_settingsLoading = false;
}

void frmMain::saveSettings()
{
    QSettings set(m_settingsFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    emit settingsAboutToSave();

    ConfigurationUI &uiConfiguration = m_configuration.uiModule();

    m_configuration.machineModule().setSpindleSpeed(ui->slbSpindle->value());
    uiConfiguration.setAutoScrollGCode(ui->chkAutoScrollGCode->isChecked());
    uiConfiguration.setLastFileOpenDir(m_lastFolder);

    set.setValue("header", ui->tblProgram->horizontalHeader()->saveState());
    set.setValue("settingsSplitMain", m_settings->ui->splitMain->saveState());
    set.setValue("formGeometry", this->saveGeometry());
    set.setValue("formSettingsGeometry", m_settings->saveGeometry()); 

    set.setValue("recentFiles", m_recentFiles);
    set.setValue("recentHeightmaps", m_recentHeightmaps);
//    set.setValue("lastFolder", m_lastFolder);

    set.setValue("jogSteps", (QStringList)ui->cboJogStep->items().mid(1, ui->cboJogStep->items().count() - 1));
    set.setValue("jogStep", ui->cboJogStep->currentText());
    set.setValue("jogFeeds", ui->cboJogFeed->items());
    set.setValue("jogFeed", ui->cboJogFeed->currentText());

    QStringList list;

    // for (int i = 0; i < ui->cboCommand->count(); i++) list.append(ui->cboCommand->itemText(i));
    // set.setValue("recentCommands", list);

    // Docks
    set.setValue("formMainState", saveState());

    // Shortcuts
    ShortcutsMap m;
    QByteArray ba;
    QDataStream s(&ba, QIODevice::WriteOnly);
    QList<QAction*> acts = findChildren<QAction*>(QRegExp("act.*"));

    foreach (QAction *a, acts) m[a->objectName()] = a->shortcuts();
    s << m;
    set.setValue("shortcuts", ba);

    // Panels
    set.setValue("panelsDevice", QVariant::fromValue(ui->scrollContentsDevice->saveState()));
    set.setValue("panelsModification", QVariant::fromValue(ui->scrollContentsModification->saveState()));
    set.setValue("panelsUser", QVariant::fromValue(ui->scrollContentsUser->saveState()));

    QStringList panels;
    QStringList hiddenPanels;
    QStringList collapsedPanels;
    if (ui->scrollContentsDevice->isVisible()) panels << ui->scrollContentsDevice->saveState();
    if (ui->scrollContentsModification->isVisible()) panels << ui->scrollContentsModification->saveState();
    if (ui->scrollContentsUser->isVisible()) panels << ui->scrollContentsUser->saveState();
    foreach (QString s, panels) {
        QGroupBox *b = findChild<QGroupBox*>(s);
        if (b && b->isHidden()) hiddenPanels << s;
        if (b && b->isCheckable() && !b->isChecked()) collapsedPanels << s;
    }    
    set.setValue("hiddenPanels", hiddenPanels);
    set.setValue("collapsedPanels", collapsedPanels);

    // Menu
    set.setValue("lockWindows", ui->actViewLockWindows->isChecked());
    set.setValue("lockPanels", ui->actViewLockPanels->isChecked());

    emit settingsSaved();
}

void frmMain::initializeConnection(ConnectionMode mode)
{
    switch (mode) {
        case ConnectionMode::SERIAL:
            m_connection = new SerialConnection(this);
            break;
        case ConnectionMode::RAW_TCP:
            m_connection = new RawTcpConnection(this);
            break;
        case ConnectionMode::VIRTUAL:
            m_connection = new VirtualUCNCConnection(this);
            break;
    }

    //connect(m_connection, SIGNAL(lineReceived(QString)), this, SLOT(onConnectionLineReceived(QString)));
    connect(m_connection, SIGNAL(error(QString)), this, SLOT(onConnectionError(QString)));
}

void frmMain::applyVisualizerConfiguration(ConfigurationVisualizer &visualizerConfiguration)
{
    ui->glwVisualizer->setLineWidth(visualizerConfiguration.lineWidth());
    ui->glwVisualizer->setAntialiasing(visualizerConfiguration.antialiasing());
    ui->glwVisualizer->setMsaa(visualizerConfiguration.msaa());
    ui->glwVisualizer->setZBuffer(visualizerConfiguration.zBuffer());
    ui->glwVisualizer->setFov(visualizerConfiguration.fieldOfView());
    ui->glwVisualizer->setNearPlane(visualizerConfiguration.nearPlane());
    ui->glwVisualizer->setFarPlane(visualizerConfiguration.farPlane());
    ui->glwVisualizer->setVsync(visualizerConfiguration.vsync());
    ui->glwVisualizer->setFps(visualizerConfiguration.fpsLock());
    ui->glwVisualizer->setColorBackground(visualizerConfiguration.backgroundColor());
    ui->glwVisualizer->setColorText(visualizerConfiguration.textColor());

    // Adapt visualizer buttons colors
    const int LIGHTBOUND = 127;
    const int NORMALSHIFT = 40;
    const int HIGHLIGHTSHIFT = 80;

    QColor base = visualizerConfiguration.backgroundColor();
    bool light = base.value() > LIGHTBOUND;

    ui->cmdToggleProjection->setIcon(QIcon(":/images/toggle.png"));
    ui->cmdFit->setIcon(QIcon(":/images/fit_1.png"));
    ui->cmdIsometric->setIcon(QIcon(":/images/cube.png"));
    ui->cmdFront->setIcon(QIcon(":/images/cubeFront.png"));
    ui->cmdLeft->setIcon(QIcon(":/images/cubeLeft.png"));
    ui->cmdTop->setIcon(QIcon(":/images/cubeTop.png"));

    if (!light) {
        Util::invertButtonIconColors(ui->cmdToggleProjection);
        Util::invertButtonIconColors(ui->cmdFit);
        Util::invertButtonIconColors(ui->cmdIsometric);
        Util::invertButtonIconColors(ui->cmdFront);
        Util::invertButtonIconColors(ui->cmdLeft);
        Util::invertButtonIconColors(ui->cmdTop);
    }

    QColor normal, highlight;

    normal.setHsv(base.hue(), base.saturation(), base.value() + (light ? -NORMALSHIFT : NORMALSHIFT));
    highlight.setHsv(base.hue(), base.saturation(), base.value() + (light ? -HIGHLIGHTSHIFT : HIGHLIGHTSHIFT));

    ui->glwVisualizer->setStyleSheet(QString("QToolButton {border: 1px solid %1; \
                background-color: %3} QToolButton:hover {border: 1px solid %2;}")
                .arg(normal.name()).arg(highlight.name())
                .arg(base.name()));
}

void frmMain::applyCodeDrawerConfiguration(ConfigurationVisualizer &visualizerConfiguration)
{
    m_codeDrawer->setLineWidth(visualizerConfiguration.lineWidth());
    m_codeDrawer->setSimplify(visualizerConfiguration.simplifyGeometry());
    m_codeDrawer->setSimplifyPrecision(visualizerConfiguration.simplifyGeometryPrecision());
    m_codeDrawer->setColorNormal(visualizerConfiguration.normalToolpathColor());
    m_codeDrawer->setColorDrawn(visualizerConfiguration.drawnToolpathColor());
    m_codeDrawer->setColorHighlight(visualizerConfiguration.hightlightToolpathColor());
    m_codeDrawer->setColorZMovement(visualizerConfiguration.zMovementColor());
    m_codeDrawer->setColorStart(visualizerConfiguration.startPointColor());
    m_codeDrawer->setColorEnd(visualizerConfiguration.endPointColor());
    m_codeDrawer->setIgnoreZ(visualizerConfiguration.grayscaleSegments() || visualizerConfiguration.programDrawMode() != ConfigurationVisualizer::ProgramDrawMode::Vectors);
    m_codeDrawer->setGrayscaleSegments(visualizerConfiguration.grayscaleSegments());
    m_codeDrawer->setGrayscaleCode(visualizerConfiguration.grayscaleSegmentsBySCode() ? GcodeDrawer::S : GcodeDrawer::Z);
    m_codeDrawer->setDrawMode(
        visualizerConfiguration.programDrawMode() == ConfigurationVisualizer::ProgramDrawMode::Vectors
            ? GcodeDrawer::Vectors
            : GcodeDrawer::Raster
        );
    m_codeDrawer->setGrayscaleMin(m_configuration.machineModule().laserPowerRange().min);
    m_codeDrawer->setGrayscaleMax(m_configuration.machineModule().laserPowerRange().max);
    m_codeDrawer->update();
}

void frmMain::applyToolDrawerConfiguration(ConfigurationVisualizer &visualizerConfiguration)
{
    m_toolDrawer.setToolDiameter(visualizerConfiguration.toolDiameter());
    m_toolDrawer.setToolLength(visualizerConfiguration.toolLength());
    m_toolDrawer.setLineWidth(visualizerConfiguration.lineWidth());
    m_toolDrawer.setToolAngle(visualizerConfiguration.toolType() == ConfigurationVisualizer::ToolType::Conic ? 180 : visualizerConfiguration.toolAngle());
    m_toolDrawer.setColor(visualizerConfiguration.toolColor());
    m_toolDrawer.update();
}

void frmMain::appleTableSurfaceDrawerConfiguration(ConfigurationVisualizer &visualizerConfiguration)
{
    m_tableSurfaceDrawer.setGridColor(visualizerConfiguration.tableSurfaceGridColor());
    m_tableSurfaceDrawer.update();
}

void frmMain::applyHeightmapDrawerConfiguration(ConfigurationVisualizer &visualizerConfiguration)
{
    m_heightmapBorderDrawer.setLineWidth(visualizerConfiguration.lineWidth());
    m_heightmapGridDrawer.setLineWidth(0.1);
    m_heightmapInterpolationDrawer.setLineWidth(visualizerConfiguration.lineWidth());
}

void frmMain::applyUIConfiguration(ConfigurationUI &uiConfiguration)
{
    ui->chkAutoScrollGCode->setChecked(uiConfiguration.autoScrollGCode());
    qApp->setStyleSheet(
        QString(qApp->styleSheet()).replace(QRegExp("font-size:[^;^\\}]+"), QString("font-size: %1pt").arg(uiConfiguration.fontSize()))
    );
    qDebug() << QString(qApp->styleSheet());
}

void frmMain::applySettings()
{
    ConfigurationVisualizer &visualizerConfiguration = m_configuration.visualizerModule();
    ConfigurationHeightmap &heightmapConfiguration = m_configuration.heightmapModule();
    ConfigurationMachine &machineConfiguration = m_configuration.machineModule();
    ConfigurationUI &uiConfiguration = m_configuration.uiModule();

    m_originDrawer->setLineWidth(visualizerConfiguration.lineWidth());

    // @TODO watch for changes is communicator?
    m_communicator->stopUpdatingState();
    m_communicator->startUpdatingState(m_configuration.connectionModule().queryStateInterval());

    applyToolDrawerConfiguration(visualizerConfiguration);
    applyCodeDrawerConfiguration(visualizerConfiguration);
    applyVisualizerConfiguration(visualizerConfiguration);
    appleTableSurfaceDrawerConfiguration(visualizerConfiguration);
    applyHeightmapConfiguration(heightmapConfiguration);
    applyHeightmapDrawerConfiguration(visualizerConfiguration);
    applySpindleConfiguration(machineConfiguration);
    applyOverridesConfiguration(machineConfiguration);
    applyUIConfiguration(uiConfiguration);

    m_selectionDrawer.setColor(visualizerConfiguration.hightlightToolpathColor());

    if (m_connection->getSupportedMode() != m_configuration.connectionModule().connectionMode()) {
        initializeConnection(m_configuration.connectionModule().connectionMode());
        m_communicator->replaceConnection(m_connection);
    }
}

void frmMain::openPort()
{
    if (m_connection->openConnection()) {
        m_partState->setStatusText(tr("Port opened"), "palette(button)", "palette(text)");
        m_communicator->reset();
        //grblReset();
    }
}

void frmMain::updateParser()
{
    GcodeViewParse *parser = m_currentDrawer->viewParser();

    GcodeParser gp;
    gp.setTraverseSpeed(m_communicator->machineConfiguration().maxRate().x()); // uses only x axis speed
    if (m_codeDrawer->getIgnoreZ()) gp.reset(QVector3D(qQNaN(), qQNaN(), 0));

    ui->tblProgram->setUpdatesEnabled(false);

    QString stripped;
    QList<QString> args;

    QProgressDialog progress(tr("Updating..."), tr("Abort"), 0, m_currentModel->rowCount() - 2, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedSize(progress.sizeHint());

    if (m_currentModel->rowCount() > PROGRESSMINLINES) {
        progress.show();
        progress.setStyleSheet("QProgressBar {text-align: center; qproperty-format: \"\"}");
    }

    for (int i = 0; i < m_currentModel->rowCount() - 1; i++) {
        // Get stored args
        args = m_currentModel->data().at(i).args;

        // Store args if none
        if (args.isEmpty()) {
            stripped = GcodePreprocessorUtils::removeComment(m_currentModel->data().at(i).command);
            args = GcodePreprocessorUtils::splitCommand(stripped);
            m_currentModel->data()[i].args = args;
        }

        // Add command to parser
        gp.addCommand(args);

        // Update table model
        m_currentModel->data()[i].state = GCodeItem::InQueue;
        m_currentModel->data()[i].response = QString();
        m_currentModel->data()[i].line = gp.getCommandNumber();

        if (progress.isVisible() && (i % PROGRESSSTEP == 0)) {
            progress.setValue(i);
            qApp->processEvents();
            if (progress.wasCanceled()) break;
        }
    }
    progress.close();

    ui->tblProgram->setUpdatesEnabled(true);

    parser->reset();

    ConfigurationParser &configurationParser = m_configuration.parserModule();

    updateProgramEstimatedTime(
        parser->getLinesFromParser(
            &gp,
            configurationParser.arcApproximationValue(),
            configurationParser.arcApproximationMode() == ConfigurationParser::ParserArcApproximationMode::ByAngle
        )
    );
    m_currentDrawer->update();
    ui->glwVisualizer->updateExtremes(m_currentDrawer);
    updateControlsState();

    if (m_currentModel == &m_programModel) m_fileChanged = true;
}

// @TODO scripting only??
// void frmMain::storeOffsetsVars(QString response)
// {
//     static QRegExp gx("\\[(G5[4-9]|G28|G30|G92|PRB):([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)");
//     static QRegExp tx("\\[(TLO):([\\d\\.\\-]+)");

//     int p = 0;
//     while ((p = gx.indexIn(response, p)) != -1) {
//         m_storedVars.setCoords(gx.cap(1), QVector3D(
//             gx.cap(2).toDouble(),
//             gx.cap(3).toDouble(),
//             gx.cap(4).toDouble()
//         ));
            
//         p += gx.matchedLength();
//     }

//     if (tx.indexIn(response) != -1) {
//         m_storedVars.setCoords(tx.cap(1), QVector3D(
//             0,
//             0,
//             tx.cap(2).toDouble()
//         ));
//     }
// }

void frmMain::loadFile(QString fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, this->windowTitle(), tr("Can't open file:\n") + fileName);
        return;
    }

    // Set filename
    m_programFileName = fileName;

    // Prepare text stream
    QTextStream textStream(&file);

    // Read lines
    QList<std::string> data;
    while (!textStream.atEnd()) data.append(textStream.readLine().toStdString());

    qDebug() << "Lines: " << data.count();

    // Load lines
    loadFile(data);
}

void frmMain::loadFile(QList<std::string> data)
{
    assert(m_communicator->isMachineConfigurationReady());
    if (!m_communicator->isMachineConfigurationReady()) {
        return;
    }

    // Reset tables
    clearTable();
    m_probeModel.clear();
    m_programHeightmapModel.clear();
    updateCurrentModel(&m_programModel);

    // Reset parsers
    m_viewParser.reset();
    m_probeParser.reset();

    // Reset code drawer
    m_currentDrawer = m_codeDrawer;
    m_codeDrawer->update();
    ui->glwVisualizer->fitDrawable(m_codeDrawer);
    updateProgramEstimatedTime(QList<LineSegment*>());

    // Update interface
    ui->chkHeightMapUse->setChecked(false);
    ui->grpHeightMap->setProperty("overrided", false);
    style()->unpolish(ui->grpHeightMap);
    ui->grpHeightMap->ensurePolished();

    // Reset tableview
    QByteArray headerState = ui->tblProgram->horizontalHeader()->saveState();
    ui->tblProgram->setModel(NULL);

    // Prepare parser
    GcodeParser gp;
    gp.setTraverseSpeed(m_communicator->machineConfiguration().maxRate().x()); // uses only x axis speed
    if (m_codeDrawer->getIgnoreZ()) gp.reset(QVector3D(qQNaN(), qQNaN(), 0));

    // Block parser updates on table changes
    m_programLoading = true;

    // Prepare model
    m_programModel.data().clear();
    m_programModel.data().reserve(data.count());

    QProgressDialog progress(tr("Opening file..."), tr("Abort"), 0, data.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setFixedSize(progress.sizeHint());
    if (data.count() > PROGRESSMINLINES) {
        progress.show();
        progress.setStyleSheet("QProgressBar {text-align: center; qproperty-format: \"\"}");
    }

    std::string command;
    std::string stripped;
    std::string trimmed;
    QList<QString> args;
    GCodeItem item;

    QList<std::string>::iterator dataIterator = data.begin();
    int remaining = data.count();
    for (dataIterator = data.begin(); dataIterator != data.end(); ++dataIterator)
    {
        command = *dataIterator; // data.takeFirst

        // Trim command

        trimmed = GcodePreprocessorUtils::trimCommand(command);

        if (!trimmed.empty()) {
            // Split command
            stripped = GcodePreprocessorUtils::removeComment(command);
            args = GcodePreprocessorUtils::splitCommand(stripped);

            gp.addCommand(args);

            item.command = QString::fromStdString(trimmed);
            item.state = GCodeItem::InQueue;
            item.line = gp.getCommandNumber();
            item.args = args;

            m_programModel.data().append(item);
        }

        remaining--;

        if (progress.isVisible() && (remaining % PROGRESSSTEP == 0)) {
            progress.setValue(progress.maximum() - remaining);
            qApp->processEvents();
            if (progress.wasCanceled()) break;
        }
    }
    progress.close();
    qApp->processEvents();

    m_programModel.insertRow(m_programModel.rowCount());

    updateProgramEstimatedTime(
        m_viewParser.getLinesFromParser(
            &gp,
            m_configuration.parserModule().arcApproximationValue(),
            m_configuration.parserModule().arcApproximationMode() == ConfigurationParser::ParserArcApproximationMode::ByAngle
        )
    );

    m_programLoading = false;

    // Set table model
    ui->tblProgram->setModel(&m_programModel);
    ui->tblProgram->horizontalHeader()->restoreState(headerState);

    // Update tableview
    connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
    ui->tblProgram->selectRow(0);

    //  Update code drawer
    m_codeDrawer->update();
    ui->glwVisualizer->fitDrawable(m_codeDrawer);

    resetHeightmap();
    updateControlsState();
}

bool frmMain::saveChanges(bool heightMapMode)
{
    if ((!heightMapMode && m_fileChanged)) {
        int res = QMessageBox::warning(this, this->windowTitle(), tr("G-code program file was changed. Save?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (res == QMessageBox::Cancel) return false;
        else if (res == QMessageBox::Yes) on_actFileSave_triggered();
        m_fileChanged = false;
    }

    if (m_heightmapChanged) {
        int res = QMessageBox::warning(this, this->windowTitle(), tr("Heightmap file was changed. Save?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (res == QMessageBox::Cancel) return false;
        else if (res == QMessageBox::Yes) {
            m_heightmapMode = true;
            on_actFileSave_triggered();
            m_heightmapMode = heightMapMode;
            updateRecentFilesMenu(); // Restore g-code files recent menu
        }

        m_fileChanged = false;
    }

    return true;
}

bool frmMain::saveProgramToFile(QString fileName, GCodeTableModel *model)
{
    QFile file(fileName);
    QDir dir;

    if (file.exists()) dir.remove(file.fileName());
    if (!file.open(QIODevice::WriteOnly)) return false;

    QTextStream textStream(&file);

    for (int i = 0; i < model->rowCount() - 1; i++) {
        textStream << model->data(model->index(i, 1)).toString() << "\r\n";
    }

    file.close();

    return true;
}

void frmMain::loadHeightmap(QString fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, this->windowTitle(), tr("Can't open file:\n") + fileName);
        return;
    }
    QTextStream textStream(&file);

    m_settingsLoading = true;

    // Storing previous values
    ui->txtHeightMapBorderX->setValue(qQNaN());
    ui->txtHeightMapBorderY->setValue(qQNaN());
    ui->txtHeightMapBorderWidth->setValue(qQNaN());
    ui->txtHeightMapBorderHeight->setValue(qQNaN());

    ui->txtHeightMapGridX->setValue(qQNaN());
    ui->txtHeightMapGridY->setValue(qQNaN());
    ui->txtHeightMapGridZBottom->setValue(qQNaN());
    ui->txtHeightMapGridZTop->setValue(qQNaN());

    QList<QString> list = textStream.readLine().split(";");
    ui->txtHeightMapBorderX->setValue(list[0].toDouble());
    ui->txtHeightMapBorderY->setValue(list[1].toDouble());
    ui->txtHeightMapBorderWidth->setValue(list[2].toDouble());
    ui->txtHeightMapBorderHeight->setValue(list[3].toDouble());

    list = textStream.readLine().split(";");
    ui->txtHeightMapGridX->setValue(list[0].toDouble());
    ui->txtHeightMapGridY->setValue(list[1].toDouble());
    ui->txtHeightMapGridZBottom->setValue(list[2].toDouble());
    ui->txtHeightMapGridZTop->setValue(list[3].toDouble());

    m_settingsLoading = false;

    updateHeightmapBorderDrawer();

    m_heightmapModel.clear();   // To avoid probe data wipe message
    updateHeightmapGrid();

    list = textStream.readLine().split(";");

    for (int i = 0; i < m_heightmapModel.rowCount(); i++) {
        QList<QString> row = textStream.readLine().split(";");
        for (int j = 0; j < m_heightmapModel.columnCount(); j++) {
            m_heightmapModel.setData(m_heightmapModel.index(i, j), row[j].toDouble(), Qt::UserRole);
        }
    }

    file.close();

    ui->txtHeightMap->setText(fileName.mid(fileName.lastIndexOf("/") + 1));
    m_heightmapFileName = fileName;
    m_heightmapChanged = false;

    ui->cboHeightMapInterpolationType->setCurrentIndex(list[0].toInt());
    ui->txtHeightMapInterpolationStepX->setValue(list[1].toDouble());
    ui->txtHeightMapInterpolationStepY->setValue(list[2].toDouble());

    updateHeightMapInterpolationDrawer();
}

bool frmMain::saveHeightmap(QString fileName)
{
    QFile file(fileName);
    QDir dir;

    if (file.exists()) dir.remove(file.fileName());
    if (!file.open(QIODevice::WriteOnly)) return false;

    QTextStream textStream(&file);
    textStream << ui->txtHeightMapBorderX->text() << ";"
               << ui->txtHeightMapBorderY->text() << ";"
               << ui->txtHeightMapBorderWidth->text() << ";"
               << ui->txtHeightMapBorderHeight->text() << "\r\n";
    textStream << ui->txtHeightMapGridX->text() << ";"
               << ui->txtHeightMapGridY->text() << ";"
               << ui->txtHeightMapGridZBottom->text() << ";"
               << ui->txtHeightMapGridZTop->text() << "\r\n";
    textStream << ui->cboHeightMapInterpolationType->currentIndex() << ";"
               << ui->txtHeightMapInterpolationStepX->text() << ";"
                << ui->txtHeightMapInterpolationStepY->text() << "\r\n";

    for (int i = 0; i < m_heightmapModel.rowCount(); i++) {
        for (int j = 0; j < m_heightmapModel.columnCount(); j++) {
            textStream << m_heightmapModel.data(m_heightmapModel.index(i, j), Qt::UserRole).toString() << ((j == m_heightmapModel.columnCount() - 1) ? "" : ";");
        }
        textStream << "\r\n";
    }

    file.close();

    m_heightmapChanged = false;

    return true;
}

void frmMain::clearTable()
{
    m_programModel.clear();
    m_programModel.insertRow(0);
}

void frmMain::resetHeightmap()
{
    delete m_heightmapInterpolationDrawer.data();
    m_heightmapInterpolationDrawer.setData(NULL);

    ui->tblHeightMap->setModel(NULL);
    m_heightmapModel.resize(1, 1);

    ui->txtHeightMap->clear();
    m_heightmapFileName.clear();
    m_heightmapChanged = false;
}

void frmMain::newFile()
{
    // Reset tables
    clearTable();
    m_probeModel.clear();
    m_programHeightmapModel.clear();
    updateCurrentModel(&m_programModel);

    // Reset parsers
    m_viewParser.reset();
    m_probeParser.reset();

    // Reset code drawer
    m_codeDrawer->update();
    m_currentDrawer = m_codeDrawer;
    ui->glwVisualizer->fitDrawable();
    updateProgramEstimatedTime(QList<LineSegment*>());

    m_programFileName = "";
    ui->chkHeightMapUse->setChecked(false);
    ui->grpHeightMap->setProperty("overrided", false);
    style()->unpolish(ui->grpHeightMap);
    ui->grpHeightMap->ensurePolished();

    // Reset tableview
    QByteArray headerState = ui->tblProgram->horizontalHeader()->saveState();
    ui->tblProgram->setModel(NULL);

    // Set table model
    ui->tblProgram->setModel(&m_programModel);
    ui->tblProgram->horizontalHeader()->restoreState(headerState);

    // Update tableview
    connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
    ui->tblProgram->selectRow(0);

    // Clear selection marker
    m_selectionDrawer.setEndPosition(QVector3D(sNan, sNan, sNan));
    m_selectionDrawer.update();

    resetHeightmap();

    updateControlsState();
}

void frmMain::newHeightmap()
{
    m_heightmapModel.clear();
    on_cmdFileReset_clicked();
    ui->txtHeightMap->setText(tr("Untitled"));
    m_heightmapFileName.clear();

    updateHeightmapBorderDrawer();
    updateHeightmapGrid();

    m_heightmapChanged = false;

    updateControlsState();
}

// void frmMain::setupCoordsTextboxes()
// {
//     int prec = m_settings->units() == 0 ? 3 : 4;
//     int bound = m_settings->units() == 0 ? 9999 : 999;

//     m_partState->updateUnits();

//     ui->txtMPosX->setDecimals(prec);
//     ui->txtMPosX->setMinimum(-bound);
//     ui->txtMPosX->setMaximum(bound);
//     ui->txtMPosY->setDecimals(prec);
//     ui->txtMPosY->setMinimum(-bound);
//     ui->txtMPosY->setMaximum(bound);
//     ui->txtMPosZ->setDecimals(prec);
//     ui->txtMPosZ->setMinimum(-bound);
//     ui->txtMPosZ->setMaximum(bound);

//     ui->txtWPosX->setDecimals(prec);
//     ui->txtWPosX->setMinimum(-bound);
//     ui->txtWPosX->setMaximum(bound);
//     ui->txtWPosY->setDecimals(prec);
//     ui->txtWPosY->setMinimum(-bound);
//     ui->txtWPosY->setMaximum(bound);
//     ui->txtWPosZ->setDecimals(prec);
//     ui->txtWPosZ->setMinimum(-bound);
//     ui->txtWPosZ->setMaximum(bound);
// }

void frmMain::updateControlsState()
{
    bool portOpened = m_connection->isConnected();
    bool process = (m_communicator->senderState() == SenderTransferring) || (m_communicator->senderState() == SenderStopping);
    bool paused = (m_communicator->senderState() == SenderPausing) || (m_communicator->senderState() == SenderPausing2) || (m_communicator->senderState() == SenderPaused) || (m_communicator->senderState() == SenderChangingTool);

    ui->grpState->setEnabled(portOpened);
    ui->grpControl->setEnabled(portOpened);
    ui->widgetSpindle->setEnabled(portOpened);
    ui->widgetJog->setEnabled(portOpened && ((m_communicator->senderState() == SenderStopped)
        || (m_communicator->senderState() == SenderChangingTool)));

    m_partConsole->setEnabled(portOpened && (!ui->chkKeyboardControl->isChecked()));
    // ui->cmdCommandSend->setEnabled(portOpened);

    ui->cmdCheck->setEnabled(portOpened && !process);
    ui->cmdHome->setEnabled(!process);
    ui->cmdCheck->setEnabled(!process);
    ui->cmdUnlock->setEnabled(!process);
    ui->cmdSpindle->setEnabled(!process);
    ui->cmdSleep->setEnabled(!process);

    ui->actFileNew->setEnabled(m_communicator->senderState() == SenderStopped);
    ui->actFileOpen->setEnabled(m_communicator->senderState() == SenderStopped);
    ui->cmdFileOpen->setEnabled(m_communicator->senderState() == SenderStopped);
    ui->cmdFileReset->setEnabled((m_communicator->senderState() == SenderStopped) && m_programModel.rowCount() > 1);
    ui->cmdFileSend->setEnabled(portOpened && (m_communicator->senderState() == SenderStopped) && m_programModel.rowCount() > 1);
    switch (m_communicator->senderState()) {
        case SenderPausing:
        case SenderPausing2:
            ui->cmdFilePause->setText(tr("Pausing..."));
            break;
        case SenderPaused:
        case SenderChangingTool:
            ui->cmdFilePause->setText(tr("Resume"));
            break;
        default:
            ui->cmdFilePause->setText(tr("Pause"));
            break;
    }
    ui->cmdFilePause->setEnabled(portOpened && (process || paused) && (m_communicator->senderState() != SenderPausing) && (m_communicator->senderState() != SenderPausing2));
    ui->cmdFilePause->setChecked(paused);
    ui->cmdFileAbort->setEnabled(m_communicator->senderState() != SenderStopped && m_communicator->senderState() != SenderStopping);
    ui->mnuRecent->setEnabled((m_communicator->senderState() == SenderStopped) && ((m_recentFiles.count() > 0 && !m_heightmapMode)
                                                      || (m_recentHeightmaps.count() > 0 && m_heightmapMode)));
    ui->actFileSave->setEnabled(m_programModel.rowCount() > 1);
    ui->actFileSaveAs->setEnabled(m_programModel.rowCount() > 1);

    ui->tblProgram->setEditTriggers((m_communicator->senderState() != SenderStopped) ? QAbstractItemView::NoEditTriggers :
        QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | 
        QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    if (!portOpened) {
        this->m_partState->setStatusText(tr("Not connected"), "palette(button)", "palette(text)");
        emit deviceStateChanged(-1);
    }

    this->setWindowTitle(m_programFileName.isEmpty() ? qApp->applicationDisplayName()
                                                     : m_programFileName.mid(m_programFileName.lastIndexOf("/") + 1) + " - " + qApp->applicationDisplayName());

    if (!process) ui->chkKeyboardControl->setChecked(m_storedKeyboardControl);

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7 && m_taskBarProgress) {
        m_taskBarProgress->setPaused(paused);
        if (m_communicator->senderState() == SenderStopped) m_taskBarProgress->hide();
    }
#endif

    style()->unpolish(ui->cmdFileOpen);
    style()->unpolish(ui->cmdFileReset);
    style()->unpolish(ui->cmdFileSend);
    style()->unpolish(ui->cmdFilePause);
    style()->unpolish(ui->cmdFileAbort);
    ui->cmdFileOpen->ensurePolished();
    ui->cmdFileReset->ensurePolished();
    ui->cmdFileSend->ensurePolished();
    ui->cmdFilePause->ensurePolished();
    ui->cmdFileAbort->ensurePolished();

    // Heightmap
    m_heightmapBorderDrawer.setVisible(ui->chkHeightMapBorderShow->isChecked() && m_heightmapMode);
    m_heightmapGridDrawer.setVisible(ui->chkHeightMapGridShow->isChecked() && m_heightmapMode);
    m_heightmapInterpolationDrawer.setVisible(ui->chkHeightMapInterpolationShow->isChecked() && m_heightmapMode);

    ui->grpProgram->setText(m_heightmapMode ? tr("Heightmap") : tr("G-code program"));
    ui->grpProgram->setProperty("overrided", m_heightmapMode);
    style()->unpolish(ui->grpProgram);
    ui->grpProgram->ensurePolished();

    ui->cboJogStep->setEditable(!ui->chkKeyboardControl->isChecked());
    ui->cboJogFeed->setEditable(!ui->chkKeyboardControl->isChecked());
    ui->cboJogStep->setEnabled(!ui->chkKeyboardControl->isChecked());
    ui->cboJogFeed->setEnabled(!ui->chkKeyboardControl->isChecked());
    ui->cboJogStep->setStyleSheet(QString("font-size: %1").arg(m_configuration.uiModule().fontSize()));
    ui->cboJogFeed->setStyleSheet(ui->cboJogStep->styleSheet());

    ui->tblHeightMap->setVisible(m_heightmapMode);
    ui->tblProgram->setVisible(!m_heightmapMode);

    ui->widgetHeightMap->setEnabled(!process && m_programModel.rowCount() > 1);
    ui->cmdHeightMapMode->setEnabled(!ui->txtHeightMap->text().isEmpty());

    ui->cmdFileSend->setText(m_heightmapMode ? tr("Probe") : tr("Send"));

    ui->chkHeightMapUse->setEnabled(!m_heightmapMode && !ui->txtHeightMap->text().isEmpty());

    ui->actFileSaveTransformedAs->setVisible(ui->chkHeightMapUse->isChecked());

    ui->cmdFileSend->menu()->actions().first()->setEnabled(!ui->cmdHeightMapMode->isChecked());

    m_selectionDrawer.setVisible(!ui->cmdHeightMapMode->isChecked());    
}

void frmMain::updateLayouts()
{
    this->update();
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void frmMain::updateRecentFilesMenu()
{
    foreach (QAction * action, ui->mnuRecent->actions()) {
        if (action->text() == "") break; else {
            ui->mnuRecent->removeAction(action);
            delete action;
        }
    }

    foreach (QString file, !m_heightmapMode ? m_recentFiles : m_recentHeightmaps) {
        QAction *action = new QAction(file, this);
        connect(action, SIGNAL(triggered()), this, SLOT(onActRecentFileTriggered()));
        ui->mnuRecent->insertAction(ui->mnuRecent->actions()[0], action);
    }

    updateControlsState();
}

void frmMain::updateOverride(SliderBox *slider, int value, char command)
{
    slider->setCurrentValue(value);

    int target = slider->isChecked() ? slider->value() : 100;
    bool smallStep = abs(target - slider->currentValue()) < 10 || m_configuration.connectionModule().queryStateInterval() < 100;

    if (slider->currentValue() < target) {
        m_connection->sendByteArray(QByteArray(1, char(smallStep ? command + 2 : command)));
    } else if (slider->currentValue() > target) {
        m_connection->sendByteArray(QByteArray(1, char(smallStep ? command + 3 : command + 1)));
    }
}

void frmMain::updateJogTitle()
{
    if (ui->grpJog->isChecked() || !ui->chkKeyboardControl->isChecked()) {
        ui->grpJog->setTitle(tr("Jog"));
    } else if (ui->chkKeyboardControl->isChecked()) {
        ui->grpJog->setTitle(tr("Jog") + QString(tr(" (%1/%2)"))
                             .arg(ui->cboJogStep->currentText().toDouble() > 0 ? ui->cboJogStep->currentText() : tr("C"))
                             .arg(ui->cboJogFeed->currentText()));
    }
}

void frmMain::addRecentFile(QString fileName)
{
    m_recentFiles.removeAll(fileName);
    m_recentFiles.append(fileName);
    if (m_recentFiles.count() > 5) m_recentFiles.takeFirst();
}

void frmMain::addRecentHeightmap(QString fileName)
{
    m_recentHeightmaps.removeAll(fileName);
    m_recentHeightmaps.append(fileName);
    if (m_recentHeightmaps.count() > 5) m_recentHeightmaps.takeFirst();
}

QRectF frmMain::borderRectFromTextboxes()
{
    QRectF rect;

    rect.setX(ui->txtHeightMapBorderX->value());
    rect.setY(ui->txtHeightMapBorderY->value());
    rect.setWidth(ui->txtHeightMapBorderWidth->value());
    rect.setHeight(ui->txtHeightMapBorderHeight->value());

    return rect;
}

QRectF frmMain::borderRectFromExtremes()
{
    QRectF rect;

    rect.setX(m_codeDrawer->getMinimumExtremes().x());
    rect.setY(m_codeDrawer->getMinimumExtremes().y());
    rect.setWidth(m_codeDrawer->getSizes().x());
    rect.setHeight(m_codeDrawer->getSizes().y());

    return rect;
}

void frmMain::updateHeightmapBorderDrawer()
{
    if (m_settingsLoading) return;

    m_heightmapBorderDrawer.setBorderRect(borderRectFromTextboxes());
}

bool frmMain::updateHeightmapGrid()
{
    if (m_settingsLoading) return true;

    // Grid map changing warning
    bool nan = true;
    for (int i = 0; i < m_heightmapModel.rowCount(); i++)
        for (int j = 0; j < m_heightmapModel.columnCount(); j++)
            if (!qIsNaN(m_heightmapModel.data(m_heightmapModel.index(i, j), Qt::UserRole).toDouble())) {
                nan = false;
                break;
            }
    if (!nan && QMessageBox::warning(this, this->windowTitle(), tr("Changing grid settings will reset probe data. Continue?"),
                                                           QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) return false;

    // Update grid drawer
    QRectF borderRect = borderRectFromTextboxes();
    m_heightmapGridDrawer.setBorderRect(borderRect);
    m_heightmapGridDrawer.setGridSize(QPointF(ui->txtHeightMapGridX->value(), ui->txtHeightMapGridY->value()));
    m_heightmapGridDrawer.setZBottom(ui->txtHeightMapGridZBottom->value());
    m_heightmapGridDrawer.setZTop(ui->txtHeightMapGridZTop->value());

    // Reset model
    int gridPointsX = ui->txtHeightMapGridX->value();
    int gridPointsY = ui->txtHeightMapGridY->value();

    m_heightmapModel.resize(gridPointsX, gridPointsY);
    ui->tblHeightMap->setModel(NULL);
    ui->tblHeightMap->setModel(&m_heightmapModel);
    resizeTableHeightmapSections();

    // Update interpolation
    updateHeightMapInterpolationDrawer(true);

    // Generate probe program
    double gridStepX = gridPointsX > 1 ? borderRect.width() / (gridPointsX - 1) : 0;
    double gridStepY = gridPointsY > 1 ? borderRect.height() / (gridPointsY - 1) : 0;

    m_programLoading = true;
    m_probeModel.clear();
    m_probeModel.insertRow(0);

    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G21G90F%1G0Z%2").
                         arg(ui->txtHeightMapProbeFeed->value()).arg(ui->txtHeightMapGridZTop->value()));
    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0X0Y0"));
    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G38.2Z%1")
                         .arg(ui->txtHeightMapGridZBottom->value()));
    m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0Z%1")
                         .arg(ui->txtHeightMapGridZTop->value()));

    double x, y;

    for (int i = 0; i < gridPointsY; i++) {
        y = borderRect.top() + gridStepY * i;
        for (int j = 0; j < gridPointsX; j++) {
            x = borderRect.left() + gridStepX * (i % 2 ? gridPointsX - 1 - j : j);
            m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0X%1Y%2")
                                 .arg(x, 0, 'f', 3).arg(y, 0, 'f', 3));
            m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G38.2Z%1")
                                 .arg(ui->txtHeightMapGridZBottom->value()));
            m_probeModel.setData(m_probeModel.index(m_probeModel.rowCount() - 1, 1), QString("G0Z%1")
                                 .arg(ui->txtHeightMapGridZTop->value()));
        }
    }

    m_programLoading = false;

    if (m_currentDrawer == m_probeDrawer) updateParser();

    m_heightmapChanged = true;
    return true;
}

void frmMain::updateHeightmapGrid(double arg1)
{
    if (sender()->property("previousValue").toDouble() != arg1 && !updateHeightmapGrid())
        static_cast<QDoubleSpinBox*>(sender())->setValue(sender()->property("previousValue").toDouble());
    else sender()->setProperty("previousValue", arg1);
}

void frmMain::resizeTableHeightmapSections()
{
    if (ui->tblHeightMap->horizontalHeader()->defaultSectionSize()
            * ui->tblHeightMap->horizontalHeader()->count() < ui->glwVisualizer->width())
        ui->tblHeightMap->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); else {
        ui->tblHeightMap->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    }
}

bool frmMain::eventFilter(QObject *obj, QEvent *event)
{
    if (ui == nullptr) {
        return QMainWindow::eventFilter(obj, event);
    }

    if (obj->inherits("QWidgetWindow")) {

        // Jog on keyboard control
        QKeySequence ks;
        QKeyEvent *ev = static_cast<QKeyEvent*>(event);

        if ((event->type() == QEvent::ShortcutOverride || event->type() == QEvent::KeyRelease)) {
            if (ev->key() == Qt::Key_Shift) {
                ks = QKeySequence(Qt::ShiftModifier);
            } else if (ev->key() == Qt::Key_Control) {
                ks = QKeySequence(Qt::ControlModifier);
            } else if (ev->key() == Qt::Key_Alt) {
                ks = QKeySequence(Qt::AltModifier);
            } else {
                ks = QKeySequence(ev->key() | ev->modifiers());
            }
        }

        if ((m_communicator->senderState() != SenderTransferring) && (m_communicator->senderState() != SenderStopping)
            && ui->chkKeyboardControl->isChecked() && !ev->isAutoRepeat()) 
        {
            static QList<QAction*> acts;
            if (acts.isEmpty()) acts << ui->actJogXMinus << ui->actJogXPlus 
                                     << ui->actJogYMinus << ui->actJogYPlus
                                     << ui->actJogZMinus << ui->actJogZPlus;

            static QList<QAbstractButton*> buttons;
            if (buttons.isEmpty()) buttons << ui->cmdXMinus << ui->cmdXPlus
                                           << ui->cmdYMinus << ui->cmdYPlus
                                           << ui->cmdZMinus << ui->cmdZPlus;

            for (int i = 0; i < acts.count(); i++) {
                if ((!buttons.at(i)->isDown()) && (event->type() == QEvent::ShortcutOverride)) {
                    if (acts.at(i)->shortcut().matches(ks) == QKeySequence::ExactMatch) {
                        buttons.at(i)->pressed();
                        buttons.at(i)->setDown(true);
                    }
                } else if (buttons.at(i)->isDown() && (event->type() == QEvent::KeyRelease)) {
                    if ((acts.at(i)->shortcut().matches(ks) == QKeySequence::ExactMatch) 
                        || (acts.at(i)->shortcut().toString().contains(ks.toString()))
                        || (ks.toString().contains(acts.at(i)->shortcut().toString()))
                        ) 
                    {
                        buttons.at(i)->released();
                        buttons.at(i)->setDown(false);
                    }
                }
            }
        }
    } else if (obj == ui->tblProgram && ((m_communicator->senderState() == SenderTransferring) || (m_communicator->senderState() == SenderStopping))) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_PageDown || keyEvent->key() == Qt::Key_PageUp
                    || keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up) {
            ui->chkAutoScrollGCode->setChecked(false);
        }
    }

    // Visualizer updates
    if (obj == this && event->type() == QEvent::WindowStateChange) {
        ui->glwVisualizer->setUpdatesEnabled(!isMinimized() && ui->dockVisualizer->isVisible());
    }

    // Drag & drop panels
    if (!ui->actViewLockPanels->isChecked() && obj->inherits("QGroupBox") 
        && (obj->parent()->objectName() == "scrollContentsDevice"
        || obj->parent()->objectName() == "scrollContentsModification"
        || obj->parent()->objectName() == "scrollContentsUser")
        && obj->objectName().startsWith("grp")) {

        if (event->type() == QEvent::MouseButtonPress) {
            
            QMouseEvent *e = static_cast<QMouseEvent*>(event);
            m_mousePressPos = e->pos();

        } else if (event->type() == QEvent::MouseMove) {

            QMouseEvent *e = static_cast<QMouseEvent*>(event);
            int d = (e->pos() - m_mousePressPos).manhattanLength();

            if (e->buttons() & Qt::LeftButton && d > QApplication::startDragDistance()) {
                QDrag *drag = new QDrag(this);
                WidgetMimeData *mimeData = new WidgetMimeData();

                mimeData->setWidget(static_cast<QWidget*>(obj));

                QPixmap *pix = new QPixmap(static_cast<QWidget*>(obj)->size());
                static_cast<QWidget*>(obj)->render(pix);
                
                drag->setMimeData(mimeData);
                drag->setPixmap(*pix);
                drag->exec();
            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void frmMain::updateCurrentModel(GCodeTableModel *m_currentModel)
{
    this->m_currentModel = m_currentModel;
    m_streamer->setModel(m_currentModel);
}

void frmMain::updateToolPositionAndToolpathShadowing(QVector3D toolPosition)
{
    m_toolDrawer.setToolPosition(m_codeDrawer->getIgnoreZ() ? QVector3D(toolPosition.x(), toolPosition.y(), 0) : toolPosition);

    SenderState senderState = m_communicator->senderState();
    DeviceState deviceState = m_communicator->deviceState();
    if (((senderState == SenderTransferring) || (senderState == SenderStopping)
         || (senderState == SenderPausing) || (senderState == SenderPausing2) || (senderState == SenderPaused)) && deviceState != DeviceCheck) {
        GcodeViewParse *parser = m_currentDrawer->viewParser();

        bool toolOntoolpath = false;

        QList<int> drawnLines;
        QList<LineSegment*> list = parser->getLineSegmentList();

        for (
            int i = m_lastDrawnLineIndex;
            i < list.count() && list.at(i)->getLineNumber() <= (m_currentModel->data(m_currentModel->index(m_streamer->processedCommandIndex(), 4)).toInt() + 1);
            i++
        ) {
            if (list.at(i)->contains(toolPosition)) {
                toolOntoolpath = true;
                m_lastDrawnLineIndex = i;
                break;
            }
            drawnLines << i;
        }

        if (toolOntoolpath) {
            foreach (int i, drawnLines) {
                list.at(i)->setDrawn(true);
            }
            if (!drawnLines.isEmpty()) m_currentDrawer->update(drawnLines);
        }
    }
}

void frmMain::updateToolpathShadowingOnCheckMode()
{
    GcodeViewParse *parser = m_currentDrawer->viewParser();
    QList<LineSegment*> list = parser->getLineSegmentList();

    if ((m_communicator->m_senderState != SenderStopping) && m_streamer->processedCommandIndex() < m_currentModel->rowCount() - 1) {
        int i;
        QList<int> drawnLines;

        for (i = m_lastDrawnLineIndex; i < list.count()
                                               && list.at(i)->getLineNumber()
                                                <= (m_currentModel->data(m_currentModel->index(m_streamer->processedCommandIndex(), 4)).toInt()); i++) {
            drawnLines << i;
        }

        if (!drawnLines.isEmpty() && (i < list.count())) {
            m_lastDrawnLineIndex = i;
            QVector3D vec = list.at(i)->getEnd();
            m_toolDrawer.setToolPosition(vec);
        }

        foreach (int i, drawnLines) {
            list.at(i)->setDrawn(true);
        }
        if (!drawnLines.isEmpty()) m_currentDrawer->update(drawnLines);
    } else {
        foreach (LineSegment* s, list) {
            if (!qIsNaN(s->getEnd().length())) {
                m_toolDrawer.setToolPosition(s->getEnd());
                break;
            }
        }
    }
}



int frmMain::bufferLength()
{
    int length = 0;

    foreach (CommandAttributes ca, m_communicator->m_commands) {
        length += ca.length;
    }

    return length;
}

QTime frmMain::updateProgramEstimatedTime(QList<LineSegment*> lines)
{
    double time = 0;

    for (int i = 0; i < lines.count(); i++) {
        LineSegment *ls = lines[i];
        double length = (ls->getEnd() - ls->getStart()).length();

        if (!qIsNaN(length) && !qIsNaN(ls->getSpeed()) && ls->getSpeed() != 0) time +=
                length / ((ui->slbFeedOverride->isChecked() && !ls->isFastTraverse())
                          ? (ls->getSpeed() * ui->slbFeedOverride->value() / 100) :
                            (ui->slbRapidOverride->isChecked() && ls->isFastTraverse())
                             ? (ls->getSpeed() * ui->slbRapidOverride->value() / 100) : ls->getSpeed());
    }

    time *= 60;

    QTime t;

    t.setHMS(0, 0, 0);
    t = t.addSecs(time);

    ui->glwVisualizer->setSpendTime(QTime(0, 0, 0));
    ui->glwVisualizer->setEstimatedTime(t);

    return t;
}

QList<LineSegment*> frmMain::subdivideSegment(LineSegment* segment)
{
    QList<LineSegment*> list;

    QRectF borderRect = borderRectFromTextboxes();

    double interpolationStepX = borderRect.width() / (ui->txtHeightMapInterpolationStepX->value() - 1);
    double interpolationStepY = borderRect.height() / (ui->txtHeightMapInterpolationStepY->value() - 1);

    double length;

    QVector3D vec = segment->getEnd() - segment->getStart();

    if (qIsNaN(vec.length())) return QList<LineSegment*>();

    if (fabs(vec.x()) / fabs(vec.y()) < interpolationStepX / interpolationStepY) length = interpolationStepY / (vec.y() / vec.length());
    else length = interpolationStepX / (vec.x() / vec.length());

    length = fabs(length);

    if (qIsNaN(length)) {
        return QList<LineSegment*>();
    }

    QVector3D seg = vec.normalized() * length;
    // int count = trunc(vec.length() / length);
    int count = (vec.length() / length);

    if (count == 0) return QList<LineSegment*>();

    for (int i = 0; i < count; i++) {
        LineSegment* line = new LineSegment(segment);
        line->setStart(i == 0 ? segment->getStart() : list[i - 1]->getEnd());
        line->setEnd(line->getStart() + seg);
        list.append(line);
    }

    if (list.count() > 0 && list.last()->getEnd() != segment->getEnd()) {
        LineSegment* line = new LineSegment(segment);
        line->setStart(list.last()->getEnd());
        line->setEnd(segment->getEnd());
        list.append(line);
    }

    return list;
}

void frmMain::jogStep()
{
    if (ui->cboJogStep->currentText().toDouble() != 0) {
        QVector3D vec = m_jogVector * ui->cboJogStep->currentText().toDouble();

        if (vec.length()) {
            bool unitsInches = m_communicator->machineConfiguration().unitsInches();
            m_communicator->sendCommand(
                CommandSource::System,
                QString("$J=%5G91X%1Y%2Z%3F%4")
                    .arg(vec.x(), 0, 'f', unitsInches ? 4 : 3)
                    .arg(vec.y(), 0, 'f', unitsInches ? 4 : 3)
                    .arg(vec.z(), 0, 'f', unitsInches ? 4 : 3)
                    .arg(ui->cboJogFeed->currentText().toDouble())
                    .arg(unitsInches ? "G20" : "G21"),
                -3
            );
        }
    }
}

void frmMain::jogContinuous()
{
    static bool block = false;
    static QVector3D v;

    if ((ui->cboJogStep->currentText().toDouble() == 0) && !block) {
        bool unitsInches = m_communicator->machineConfiguration().unitsInches();

        if (m_jogVector != v) {
            // Store jog vector before block
            QVector3D j = m_jogVector;

            if (v.length()) {
                block = true;
                m_connection->sendByteArray(QByteArray(1, char(0x85)));
                while (m_communicator->deviceState() == DeviceJog) qApp->processEvents();
                block = false;
            }
            
            // Bounds
            QVector3D b = m_communicator->machineConfiguration().machineBounds();
            // Current machine coords
            // @TODO use m_communicator storedVars
            QVector3D m(
                m_communicator->toMetric(m_communicator->m_storedVars.Mx()),
                m_communicator->toMetric(m_communicator->m_storedVars.My()),
                m_communicator->toMetric(m_communicator->m_storedVars.Mz())
            );
            // Distance to bounds
            QVector3D t;
            // Minimum distance to bounds
            double d = 0;
            if (m_communicator->machineConfiguration().softLimitsEnabled()) {
                t = QVector3D(j.x() * b.x() < 0 ? 0 - m.x() : b.x() - m.x(), 
                              j.y() * b.y() < 0 ? 0 - m.y() : b.y() - m.y(),
                              j.z() * b.z() < 0 ? 0 - m.z() : b.z() - m.z());
                for (int i = 0; i < 3; i++) if ((j[i] && (qAbs(t[i]) < d)) || (j[i] && !d)) d = qAbs(t[i]);
                // Coords not aligned, add some bounds offset
                d -= unitsInches ? m_communicator->toMetric(0.0005) : 0.005;
            } else {
                for (int i = 0; i < 3; i++) if (j[i] && (qAbs(b[i]) > d)) d = qAbs(b[i]);
            }

            // Jog vector
            QVector3D vec = j * m_communicator->toInches(d);

            if (vec.length()) {
                m_communicator->sendCommand(CommandSource::System, QString("$J=%5G91X%1Y%2Z%3F%4")
                            .arg(vec.x(), 0, 'f', unitsInches ? 4 : 3)
                            .arg(vec.y(), 0, 'f', unitsInches ? 4 : 3)
                            .arg(vec.z(), 0, 'f', unitsInches ? 4 : 3)
                            .arg(ui->cboJogFeed->currentText().toDouble())
                            .arg(unitsInches ? "G20" : "G21")
                            , -2);
            }
            v = j;
        }
    }
}

bool frmMain::isGCodeFile(QString fileName)
{
    return fileName.endsWith(".txt", Qt::CaseInsensitive)
          || fileName.endsWith(".nc", Qt::CaseInsensitive)
          || fileName.endsWith(".ncc", Qt::CaseInsensitive)
          || fileName.endsWith(".ngc", Qt::CaseInsensitive)
          || fileName.endsWith(".tap", Qt::CaseInsensitive)
          || fileName.endsWith(".gc", Qt::CaseInsensitive)
          || fileName.endsWith(".gcode", Qt::CaseInsensitive);
}

bool frmMain::isHeightmapFile(QString fileName)
{
    return fileName.endsWith(".map", Qt::CaseInsensitive);
}

int frmMain::buttonSize()
{
    return ui->cmdHome->minimumWidth();
}

void frmMain::onTransferCompleted()
{
    // Shadow last segment
    GcodeViewParse *parser = m_currentDrawer->viewParser();
    QList<LineSegment*> list = parser->getLineSegmentList();
    if (m_lastDrawnLineIndex < list.count()) {
        list[m_lastDrawnLineIndex]->setDrawn(true);
        m_currentDrawer->update(QList<int>() << m_lastDrawnLineIndex);
    }

    // Update state
    m_lastDrawnLineIndex = 0;
    updateControlsState();

    // Show message box
    qApp->beep();
    m_communicator->stopUpdatingState();
    m_timerConnection.stop();

    QMessageBox::information(this, qApp->applicationDisplayName(), tr("Job done.\nTime elapsed: %1")
                                .arg(ui->glwVisualizer->spendTime().toString("hh:mm:ss")));

    m_timerConnection.start();
    m_communicator->startUpdatingState();
}

QString frmMain::getLineInitCommands(int row)
{
    int commandIndex = row;

    GcodeViewParse *parser = m_currentDrawer->viewParser();
    QList<LineSegment*> list = parser->getLineSegmentList();
    QVector<QList<int>> lineIndexes = parser->getLinesIndexes();
    QString commands;
    int lineNumber = m_currentModel->data(m_currentModel->index(commandIndex, 4)).toInt();
    
    if (lineNumber != -1) {
        LineSegment* firstSegment = list.at(lineIndexes.at(lineNumber).first());
        LineSegment* lastSegment = list.at(lineIndexes.at(lineNumber).last());
        LineSegment* feedSegment = lastSegment;
        LineSegment* plungeSegment = lastSegment;
        int segmentIndex = list.indexOf(feedSegment);
        while (feedSegment->isFastTraverse() && (segmentIndex > 0))
            feedSegment = list.at(--segmentIndex);
        while (!(plungeSegment->isZMovement() && !plungeSegment->isFastTraverse()) && (segmentIndex > 0))
            plungeSegment = list.at(--segmentIndex);


        commands.append(QString("M3 S%1\n").arg(qMax<double>(lastSegment->getSpindleSpeed(), ui->slbSpindle->value())));

        commands.append(QString("G21 G90 G0 X%1 Y%2\n")
                        .arg(firstSegment->getStart().x())
                        .arg(firstSegment->getStart().y()));
        commands.append(QString("G1 Z%1 F%2\n")
                        .arg(firstSegment->getStart().z())
                        .arg(plungeSegment->getSpeed()));

        commands.append(QString("%1 %2 %3 F%4\n")
                        .arg(lastSegment->isMetric() ? "G21" : "G20")
                        .arg(lastSegment->isAbsolute() ? "G90" : "G91")
                        .arg(lastSegment->isFastTraverse() ? "G0" : "G1")
                        .arg(lastSegment->isMetric() ? feedSegment->getSpeed() : feedSegment->getSpeed() / 25.4));

        if (lastSegment->isArc()) {
            commands.append(lastSegment->plane() == PointSegment::XY ? "G17"
            : lastSegment->plane() == PointSegment::ZX ? "G18" : "G19");
        }
    }

    return commands;
}

bool frmMain::actionLessThan(const QAction *a1, const QAction *a2)
{
    return a1->objectName() < a2->objectName();
}

bool frmMain::actionTextLessThan(const QAction *a1, const QAction *a2)
{
    return a1->text() < a2->text();
}
