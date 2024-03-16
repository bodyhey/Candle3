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
#include <QScriptValueIterator>
#include "frmmain.h"
#include "form_partial/main/jog.h"
#include "form_partial/main/control.h"
#include "ui_frmmain.h"
#include "ui_frmsettings.h"
#include "widgets/widgetmimedata.h"
#include "connection/serialconnection.h"
#include "connection/rawtcpconnection.h"
#include "connection/virtualucncconnection.h"

#define FILE_FILTER_TEXT "G-Code files (*.nc *.ncc *.ngc *.tap *.gc *.gcode *.txt)"

frmMain::frmMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frmMain ),
    m_scripting(&m_configuration)
{
    // Loading settings
    m_settingsFileName = qApp->applicationDirPath() + "/settings.ini";
    preloadSettings();

    m_settings = new frmSettings(this);

    initializeConnection(ConnectionMode::VIRTUAL);
    m_communicator = new Communicator(m_connection, ui, m_settings, this);

    connect(this, SIGNAL(machinePosChanged(QVector3D)), this, SLOT(onMachinePosChanged(QVector3D)));
    connect(this, SIGNAL(workPosChanged(QVector3D)), this, SLOT(onWorkPosChanged(QVector3D)));
    connect(this, SIGNAL(deviceStateReceived(DeviceState)), this, SLOT(onDeviceStateReceived(DeviceState)));
    connect(this, SIGNAL(deviceStateChanged(DeviceState)), this, SLOT(onDeviceStateChanged(DeviceState)));

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

    m_fileChanged = false;
    m_heightMapChanged = false;

    // to communicator
    // m_communicator->m_homing = false;
    m_updateSpindleSpeed = false;
    m_updateParserStatus = false;

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

    this->m_partState = new partMainState(this);
    static_cast<QVBoxLayout*>(ui->grpState->layout())->insertWidget(0, this->m_partState);

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

#ifndef UNIX
    ui->cboCommand->setStyleSheet("QComboBox {padding: 2;} QComboBox::drop-down {width: 0; border-style: none;} QComboBox::down-arrow {image: url(noimg);	border-width: 0;}");
#endif
//    ui->scrollArea->updateMinimumWidth();

    m_heightMapMode = false;
    m_lastDrawnLineIndex = 0;
    m_fileProcessedCommandIndex = 0;
    m_programLoading = false;
    m_currentModel = &m_programModel;

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

    connect(ui->cboCommand, SIGNAL(returnPressed()), this, SLOT(onCboCommandReturnPressed()));

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
    m_heightMapGridDrawer.setModel(&m_heightMapModel);
    m_currentDrawer = m_codeDrawer;
    m_toolDrawer.setToolPosition(QVector3D(0, 0, 0));

    m_tableMenu = new QMenu(this);
    m_tableMenu->addAction(tr("&Insert line"), this, SLOT(onTableInsertLine()), QKeySequence(Qt::Key_Insert));
    m_tableMenu->addAction(tr("&Delete lines"), this, SLOT(onTableDeleteLines()), QKeySequence(Qt::Key_Delete));

    ui->glwVisualizer->addDrawable(m_originDrawer);
    ui->glwVisualizer->addDrawable(m_codeDrawer);
    ui->glwVisualizer->addDrawable(m_probeDrawer);
    ui->glwVisualizer->addDrawable(&m_toolDrawer);
    ui->glwVisualizer->addDrawable(&m_heightMapBorderDrawer);
    ui->glwVisualizer->addDrawable(&m_heightMapGridDrawer);
    ui->glwVisualizer->addDrawable(&m_heightMapInterpolationDrawer);
    ui->glwVisualizer->addDrawable(&m_selectionDrawer);
    ui->glwVisualizer->addDrawable(&m_machineBoundsDrawer);
    ui->glwVisualizer->fitDrawable();

    connect(ui->glwVisualizer, SIGNAL(resized()), this, SLOT(placeVisualizerButtons()));
    connect(&m_programModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_programHeightmapModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_probeModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCellChanged(QModelIndex,QModelIndex)));
    connect(&m_heightMapModel, SIGNAL(dataChangedByUserInput()), this, SLOT(updateHeightMapInterpolationDrawer()));

    ui->tblProgram->setModel(&m_programModel);
    ui->tblProgram->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    connect(ui->tblProgram->verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(onScroolBarAction(int)));
    connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));    
    clearTable();

    m_senderErrorBox = new QMessageBox(QMessageBox::Warning, qApp->applicationDisplayName(), QString(),
                                       QMessageBox::Ignore | QMessageBox::Abort, this);
    m_senderErrorBox->setCheckBox(new QCheckBox(tr("Don't show again")));

    // Prepare script functions
    m_scriptFunctions.setFrmMain(this);
    connect(this, &frmMain::responseReceived, &m_scriptFunctions, &ScriptFunctions::responseReceived);
    connect(this, &frmMain::statusReceived, &m_scriptFunctions, &ScriptFunctions::statusReceived);
    connect(this, &frmMain::senderStateChanged, &m_scriptFunctions, &ScriptFunctions::senderStateChanged);
    // @TODO scripting, what values to use for device state?
    //connect(this, &frmMain::deviceStateChanged, &m_scriptFunctions, &ScriptFunctions::deviceStateChanged);
    connect(this, &frmMain::settingsAboutToLoad, &m_scriptFunctions, &ScriptFunctions::settingsAboutToLoad);
    connect(this, &frmMain::settingsLoaded, &m_scriptFunctions, &ScriptFunctions::settingsLoaded);
    connect(this, &frmMain::settingsAboutToSave, &m_scriptFunctions, &ScriptFunctions::settingsAboutToSave);
    connect(this, &frmMain::settingsSaved, &m_scriptFunctions, &ScriptFunctions::settingsSaved);
    connect(this, &frmMain::settingsAboutToShow, &m_scriptFunctions, &ScriptFunctions::settingsAboutToShow);
    connect(this, &frmMain::settingsAccepted, &m_scriptFunctions, &ScriptFunctions::settingsAccepted);
    connect(this, &frmMain::settingsRejected, &m_scriptFunctions, &ScriptFunctions::settingsRejected);
    connect(this, &frmMain::settingsSetByDefault, &m_scriptFunctions, &ScriptFunctions::settingsSetByDefault);
    connect(this, &frmMain::pluginsLoaded, &m_scriptFunctions, &ScriptFunctions::pluginsLoaded);

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

    if (m_settings->port() != "") {
        SerialConnection *connection = qobject_cast<SerialConnection*>(m_connection);
        if (connection != nullptr) {
            connection->setPortName(m_settings->port());
            connection->setBaudRate(m_settings->baud());
        }
    }

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
    
    // Delegate vars to script engine
    QScriptValue vars = m_scriptEngine.newQObject(&m_storedVars);
    m_scriptEngine.globalObject().setProperty("vars", vars);

    // Delegate import extension function
    QScriptValue sv = m_scriptEngine.newObject();
    sv.setProperty("importExtension", m_scriptEngine.newFunction(frmMain::importExtension));
    m_scriptEngine.globalObject().setProperty("script", sv);

    // Signals/slots
    connect(&m_timerConnection, SIGNAL(timeout()), this, SLOT(onTimerConnection()));
    connect(&m_timerStateQuery, SIGNAL(timeout()), this, SLOT(onTimerStateQuery()));
    connect(&m_scriptEngine, &QScriptEngine::signalHandlerException, this, &frmMain::onScriptException);

    // Event filter
    qApp->installEventFilter(this);

    // Start timers
    m_timerConnection.start(1000);
    m_timerStateQuery.start();
}

frmMain::~frmMain()
{
    delete m_communicator;
    delete m_connection;
    delete m_senderErrorBox;
    delete ui;
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
    resizeTableHeightMapSections();
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
    bool mode = m_heightMapMode;
    m_heightMapMode = false;

    if (!saveChanges(m_heightMapMode)) {
        ce->ignore();
        m_heightMapMode = mode;
        return;
    }

    if ((m_communicator->m_senderState != SenderStopped) &&
        QMessageBox::warning(this, this->windowTitle(), tr("File sending in progress. Terminate and exit?"),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) 
    {    
        ce->ignore();
        m_heightMapMode = mode;
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
    if (m_communicator->m_senderState != SenderStopped) return;

    if (dee->mimeData()->hasFormat("application/widget")) return;

    if (dee->mimeData()->hasFormat("text/plain") && !m_heightMapMode) dee->acceptProposedAction();
    else if (dee->mimeData()->hasFormat("text/uri-list") && dee->mimeData()->urls().count() == 1) {
        QString fileName = dee->mimeData()->urls().at(0).toLocalFile();

        if ((!m_heightMapMode && isGCodeFile(fileName))
        || (m_heightMapMode && isHeightmapFile(fileName)))
            dee->acceptProposedAction();
    }
}

void frmMain::dropEvent(QDropEvent *de)
{    
    QString fileName = de->mimeData()->urls().at(0).toLocalFile();

    if (!m_heightMapMode) {
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
        loadHeightMap(fileName);
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
    if (!saveChanges(m_heightMapMode)) return;

    if (!m_heightMapMode) {
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
    if (!m_heightMapMode) {
        // G-code saving
        if (m_programFileName.isEmpty()) on_actFileSaveAs_triggered(); else {
            saveProgramToFile(m_programFileName, &m_programModel);
            m_fileChanged = false;
        }
    } else {
        // Height map saving
        if (m_heightMapFileName.isEmpty()) on_actFileSaveAs_triggered(); else saveHeightMap(m_heightMapFileName);
    }
}

void frmMain::on_actFileSaveAs_triggered()
{
    if (!m_heightMapMode) {
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

        if (!fileName.isEmpty()) if (saveHeightMap(fileName)) {
            ui->txtHeightMap->setText(fileName.mid(fileName.lastIndexOf("/") + 1));
            m_heightMapFileName = fileName;
            m_heightMapChanged = false;

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
    if (!m_heightMapMode) m_recentFiles.clear(); else m_recentHeightmaps.clear();
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

        updateControlsState();
        applySettings();

        // Update shortcuts
        for (int i = 0; i < acts.count(); i++) {
            acts[i]->setShortcut(QKeySequence(table->item(i, 2)->data(Qt::DisplayRole).toString()));
        }

        emit settingsAccepted();

    } else {
        m_settings->undo();

        emit settingsRejected();
    }
}

void frmMain::on_actServiceConfigureGRBL_triggered()
{
    m_grblConfigurator.exec();
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
    if (!m_heightMapMode) {
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
            loadHeightMap(fileName);
        }
    }
}

void frmMain::on_cmdFileSend_clicked()
{
    if (m_currentModel->rowCount() == 1) return;

    on_cmdFileReset_clicked();

    m_startTime.start();

    m_communicator->setSenderState(SenderTransferring);

    m_storedKeyboardControl = ui->chkKeyboardControl->isChecked();
    ui->chkKeyboardControl->setChecked(false);

    storeParserState();

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

    if (m_settings->useStartCommands()) m_communicator->sendCommands(m_settings->startCommands());
    sendNextFileCommands();
}

void frmMain::on_cmdFilePause_clicked(bool checked)
{
    static SenderState s;

    if (checked) {
        //PAUSE
        s = m_communicator->m_senderState;
        // setSenderState(SenderPaused);
        m_communicator->setSenderState(SenderPausing);
        ui->cmdFilePause->setText(tr("Pausing..."));
        ui->cmdFilePause->setEnabled(false);
    } else {
        //RESUME
        if (m_communicator->m_senderState == SenderChangingTool) {
            m_communicator->setSenderState(SenderTransferring);
        } else {
            if (m_settings->usePauseCommands()) {
                m_communicator->sendCommands(m_settings->afterPauseCommands());
            }
            m_communicator->setSenderState(s);
        }
        updateControlsState();
    }
}

void frmMain::on_cmdFileAbort_clicked()
{
    ui->cmdFileAbort->setEnabled(false);

    if ((m_communicator->m_senderState == SenderPaused) || (m_communicator->m_senderState == SenderChangingTool)) {
        m_communicator->sendCommand("M2", -1, m_settings->showUICommands(), false);
    } else {
        m_communicator->sendCommand("M2", -1, m_settings->showUICommands(), true);
    }
}

void frmMain::on_cmdFileReset_clicked()
{
    m_fileCommandIndex = 0;
    m_fileProcessedCommandIndex = 0;
    m_lastDrawnLineIndex = 0;
    m_communicator->m_probeIndex = -1;

    if (!m_heightMapMode) {
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

        delete m_heightMapInterpolationDrawer.data();
        m_heightMapInterpolationDrawer.setData(NULL);

        m_heightMapModel.clear();
        updateHeightMapGrid();
    }
}

void frmMain::on_cmdCommandSend_clicked()
{
    QString command = ui->cboCommand->currentText();
    if (command.isEmpty()) return;

    ui->cboCommand->storeText();
    ui->cboCommand->setCurrentText("");
    m_communicator->sendCommand(command, -1);
}

void frmMain::on_cmdClearConsole_clicked()
{
    ui->txtConsole->clear();
}

void frmMain::on_cmdHome_clicked()
{
    m_communicator->m_homing = true;
    m_updateSpindleSpeed = true;
    m_communicator->sendCommand("$H", -1, m_settings->showUICommands());
}

void frmMain::on_cmdCheck_clicked(bool checked)
{
    if (checked) {
        storeParserState();
        m_communicator->sendCommand("$C", -1, m_settings->showUICommands());
    } else {
        m_communicator->m_aborting = true;
        grblReset();
    };
}

void frmMain::on_cmdReset_clicked()
{
    grblReset();
}

void frmMain::on_cmdUnlock_clicked()
{
    m_updateSpindleSpeed = true;
    m_communicator->sendCommand("$X", -1, m_settings->showUICommands());
}

void frmMain::on_cmdHold_clicked(bool checked)
{
    m_connection->sendByteArray(QByteArray(1, checked ? (char)'!' : (char)'~'));
}

void frmMain::on_cmdSleep_clicked()
{
    m_communicator->sendCommand("$SLP", -1, m_settings->showUICommands());
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
        m_communicator->sendCommand(checked ? QString("M3 S%1").arg(ui->slbSpindle->value()) : "M5", -1, m_settings->showUICommands());
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
        m_communicator->sendCommand("$G", -2, m_settings->showUICommands());
    } else {
        if (m_absoluteCoordinates) m_communicator->sendCommand("G90", -1, m_settings->showUICommands());
    }

    if ((m_communicator->m_senderState != SenderTransferring) && (m_communicator->m_senderState != SenderStopping))
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
        m_currentModel = &m_programHeightmapModel;

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
                    z = list->at(i)->getStart().z() + Interpolation::bicubicInterpolate(borderRect, &m_heightMapModel, x, y);
                    list->at(i)->setStart(QVector3D(x, y, z));
                } else list->at(i)->setStart(list->at(i - 1)->getEnd());

                x = list->at(i)->getEnd().x();
                y = list->at(i)->getEnd().y();
                z = list->at(i)->getEnd().z() + Interpolation::bicubicInterpolate(borderRect, &m_heightMapModel, x, y);
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
        m_currentModel = &m_programModel;

        ui->tblProgram->setModel(&m_programModel);
        ui->tblProgram->horizontalHeader()->restoreState(headerState);

        connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
        ui->tblProgram->selectRow(0);

        ui->chkHeightMapUse->setChecked(false);

        return;
    } else {                                        // Restore original program
        m_currentModel = &m_programModel;

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
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderWidth_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderY_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapBorderHeight_valueChanged(double arg1)
{
    updateHeightMapBorderDrawer();
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapGridX_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapGridY_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapGridZBottom_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
}

void frmMain::on_txtHeightMapGridZTop_valueChanged(double arg1)
{
    updateHeightMapGrid(arg1);
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
    m_heightMapMode = checked;

    // Reset file progress
    m_fileCommandIndex = 0;
    m_fileProcessedCommandIndex = 0;
    m_lastDrawnLineIndex = 0;

    // Reset/restore g-code program modification on edit mode enter/exit
    if (ui->chkHeightMapUse->isChecked()) {
        on_chkHeightMapUse_clicked(!checked); // Update gcode program parser
    }

    if (checked) {
        ui->tblProgram->setModel(&m_probeModel);
        resizeTableHeightMapSections();
        m_currentModel = &m_probeModel;
        m_currentDrawer = m_probeDrawer;
        updateParser();  // Update probe program parser
    } else {
        m_probeParser.reset();
        if (!ui->chkHeightMapUse->isChecked()) {
            ui->tblProgram->setModel(&m_programModel);
            connect(ui->tblProgram->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onTableCurrentChanged(QModelIndex,QModelIndex)));
            ui->tblProgram->selectRow(0);

            resizeTableHeightMapSections();
            m_currentModel = &m_programModel;
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
        loadHeightMap(fileName);

        // If using heightmap
        if (ui->chkHeightMapUse->isChecked() && !m_heightMapMode) {
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
    if (m_communicator->m_senderState != SenderStopped) return;

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
    ui->txtConsole->appendPlainText(tr("Connection error ") + error);
    updateControlsState();
}

void frmMain::onMachinePosChanged(QVector3D pos)
{
    this->m_partState->setMachineCoordinates(pos);

    ui->txtMPosX->setValue(pos.x());
    ui->txtMPosY->setValue(pos.y());
    ui->txtMPosZ->setValue(pos.z());

    m_storedVars.setCoords("M", pos);
}

void frmMain::onWorkPosChanged(QVector3D pos)
{
    this->m_partState->setWorkCoordinates(pos);

    ui->txtWPosX->setValue(pos.x());
    ui->txtWPosY->setValue(pos.y());
    ui->txtWPosZ->setValue(pos.z());

    m_storedVars.setCoords("W", pos);
}

void frmMain::onDeviceStateChanged(DeviceState state)
{
    this->m_partState->setStatusText(m_statusCaptions[state], m_statusBackColors[state], m_statusForeColors[state]);
    ui->txtStatus->setText(m_statusCaptions[state]);
    ui->txtStatus->setStyleSheet(QString("background-color: %1; color: %2;")
                                     .arg(m_statusBackColors[state]).arg(m_statusForeColors[state]));

    ui->cmdCheck->setEnabled(state != DeviceRun && (m_communicator->m_senderState == SenderStopped));
    ui->cmdCheck->setChecked(state == DeviceCheck);
    ui->cmdHold->setChecked(state == DeviceHold0 || state == DeviceHold1 || state == DeviceQueue);
    ui->cmdSpindle->setEnabled(state == DeviceHold0 || ((m_communicator->m_senderState != SenderTransferring) &&
                                                        (m_communicator->m_senderState != SenderStopping)));
}

void frmMain::onDeviceStateReceived(DeviceState state)
{
    // Update controls state
    ui->cmdCheck->setEnabled(state != DeviceRun && (m_communicator->m_senderState == SenderStopped));
    ui->cmdCheck->setChecked(state == DeviceCheck);
    ui->cmdHold->setChecked(state == DeviceHold0 || state == DeviceHold1 || state == DeviceQueue);
    ui->cmdSpindle->setEnabled(state == DeviceHold0 || ((m_communicator->m_senderState != SenderTransferring) &&
                                                        (m_communicator->m_senderState != SenderStopping)));
    // Update "elapsed time" timer
    if ((m_communicator->m_senderState == SenderTransferring) || (m_communicator->m_senderState == SenderStopping)) {
        QTime time(0, 0, 0);
        int elapsed = m_startTime.elapsed();
        ui->glwVisualizer->setSpendTime(time.addMSecs(elapsed));
    }
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

void frmMain::onTimerConnection()
{
    if (!m_connection->isConnected()) {
        openPort();
    } else if (!m_communicator->m_homing/* && !m_reseting*/ && !ui->cmdHold->isChecked() && m_communicator->m_queue.length() == 0) {
        if (m_updateSpindleSpeed) {
            m_updateSpindleSpeed = false;
            m_communicator->sendCommand(QString("S%1").arg(ui->slbSpindle->value()), -2, m_settings->showUICommands());
        }
        if (m_updateParserStatus) {
            m_updateParserStatus = false;
            m_communicator->sendCommand("$G", -3, false);
        }
    }
}

void frmMain::onTimerStateQuery()
{
    if (m_connection->isConnected() && m_communicator->m_resetCompleted && m_communicator->m_statusReceived) {
        m_connection->sendByteArray(QByteArray(1, '?'));
        m_communicator->m_statusReceived = false;
    }

    ui->glwVisualizer->setBufferState(QString(tr("Buffer: %1 / %2 / %3")).arg(bufferLength()).arg(m_communicator->m_commands.length()).arg(m_communicator->m_queue.length()));
}

void frmMain::onTableInsertLine()
{
    if (ui->tblProgram->selectionModel()->selectedRows().count() == 0 || 
        (m_communicator->m_senderState == SenderTransferring) || (m_communicator->m_senderState == SenderStopping)) return;

    int row = ui->tblProgram->selectionModel()->selectedRows()[0].row();

    m_currentModel->insertRow(row);
    m_currentModel->setData(m_currentModel->index(row, 2), GCodeItem::InQueue);

    updateParser();

    ui->tblProgram->selectRow(row);
}

void frmMain::onTableDeleteLines()
{
    if (ui->tblProgram->selectionModel()->selectedRows().count() == 0 || 
        (m_communicator->m_senderState == SenderTransferring) || (m_communicator->m_senderState == SenderStopping) ||
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
        if (!saveChanges(m_heightMapMode)) return;
        if (!m_heightMapMode) loadFile(fileName); else loadHeightMap(fileName);
    }
}

void frmMain::onActSendFromLineTriggered()
{
    if (m_currentModel->rowCount() == 1) return;

    //Line to start from
    int commandIndex = ui->tblProgram->currentIndex().row();

    // Set parser state
    if (m_settings->autoLine()) {
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
            //     sendCommand(command, -1, m_settings->showUICommands());
            // }
            m_communicator->sendCommands(commands, -1);
        }
    }

    m_fileCommandIndex = commandIndex;
    m_fileProcessedCommandIndex = commandIndex;
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

    m_communicator->setSenderState(SenderTransferring);

    m_storedKeyboardControl = ui->chkKeyboardControl->isChecked();
    ui->chkKeyboardControl->setChecked(false);

    storeParserState();

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

    m_fileCommandIndex = commandIndex;
    m_fileProcessedCommandIndex = commandIndex;
    sendNextFileCommands();
}

void frmMain::onSlbSpindleValueUserChanged()
{
    m_updateSpindleSpeed = true;
}

void frmMain::onSlbSpindleValueChanged()
{
    if (!ui->grpSpindle->isChecked() && ui->cmdSpindle->isChecked())
        ui->grpSpindle->setTitle(tr("Spindle") + QString(tr(" (%1)")).arg(ui->slbSpindle->value()));
}

void frmMain::onCboCommandReturnPressed()
{
    QString command = ui->cboCommand->currentText();
    if (command.isEmpty()) return;

    ui->cboCommand->setCurrentText("");
    m_communicator->sendCommand(command, -1);
}

void frmMain::onDockTopLevelChanged(bool topLevel)
{
    Q_UNUSED(topLevel)
    static_cast<QWidget*>(sender())->setStyleSheet("");
}

void frmMain::onScroolBarAction(int action)
{
    Q_UNUSED(action)

    if ((m_communicator->m_senderState == SenderTransferring) || (m_communicator->m_senderState == SenderStopping))
        ui->chkAutoScroll->setChecked(false);
}

void frmMain::onScriptException(const QScriptValue &exception)
{
    qDebug() << "Script exception:" << exception.toString();
}

void frmMain::updateHeightMapInterpolationDrawer(bool reset)
{
    if (m_settingsLoading) return;

    QRectF borderRect = borderRectFromTextboxes();
    m_heightMapInterpolationDrawer.setBorderRect(borderRect);

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

            row.append(reset ? qQNaN() : Interpolation::bicubicInterpolate(borderRect, &m_heightMapModel, x, y));
        }
        interpolationData->append(row);
    }

    if (m_heightMapInterpolationDrawer.data() != NULL) {
        delete m_heightMapInterpolationDrawer.data();
    }
    m_heightMapInterpolationDrawer.setData(interpolationData);

    // Update grid drawer
    m_heightMapGridDrawer.update();

    // Heightmap changed by table user input
    if (sender() == &m_heightMapModel) m_heightMapChanged = true;

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
}

void frmMain::preloadSettings()
{
    QSettings set(m_settingsFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    qApp->setStyleSheet(QString(qApp->styleSheet()).replace(QRegExp("font-size:\\s*\\d+"), "font-size: " + set.value("fontSize", "8").toString()));

    // Update v-sync in glformat
    QGLFormat fmt = QGLFormat::defaultFormat();
    fmt.setSwapInterval(set.value("vsync", false).toBool() ? 1 : 0);
    QGLFormat::setDefaultFormat(fmt);
}

void frmMain::loadSettings()
{
    QSettings set(m_settingsFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    m_settingsLoading = true;

    emit settingsAboutToLoad();

    m_settings->setConnectionMode(static_cast<ConnectionMode>(set.value("connectionMode").toInt()));
    m_settings->setFontSize(set.value("fontSize", 8).toInt());
    m_settings->setPort(set.value("serialPort").toString());
    m_settings->setBaud(set.value("serialBaud").toInt());
    m_settings->setIgnoreErrors(set.value("ignoreErrors", false).toBool());
    m_settings->setAutoLine(set.value("autoLine", true).toBool());
    m_settings->setToolDiameter(set.value("toolDiameter", 3).toDouble());
    m_settings->setToolLength(set.value("toolLength", 15).toDouble());
    m_settings->setAntialiasing(set.value("antialiasing", true).toBool());
    m_settings->setMsaa(set.value("msaa", true).toBool());
    m_settings->setVsync(set.value("vsync", false).toBool());
    m_settings->setZBuffer(set.value("zBuffer", false).toBool());
    m_settings->setSimplify(set.value("simplify", false).toBool());
    m_settings->setSimplifyPrecision(set.value("simplifyPrecision", 0).toDouble());
    m_settings->setGrayscaleSegments(set.value("grayscaleSegments", false).toBool());
    m_settings->setGrayscaleSCode(set.value("grayscaleSCode", true).toBool());
    m_settings->setDrawModeVectors(set.value("drawModeVectors", true).toBool());    
    m_settings->setLineWidth(set.value("lineWidth", 1).toDouble());
    m_settings->setArcLength(set.value("arcLength", 0).toDouble());
    m_settings->setArcDegree(set.value("arcDegree", 0).toDouble());
    m_settings->setArcDegreeMode(set.value("arcDegreeMode", true).toBool());
    m_settings->setShowProgramCommands(set.value("showProgramCommands", 0).toBool());
    m_settings->setShowUICommands(set.value("showUICommands", 0).toBool());
    m_settings->setSpindleSpeedMin(set.value("spindleSpeedMin", 0).toInt());
    m_settings->setSpindleSpeedMax(set.value("spindleSpeedMax", 100).toInt());
    m_settings->setLaserPowerMin(set.value("laserPowerMin", 0).toInt());
    m_settings->setLaserPowerMax(set.value("laserPowerMax", 100).toInt());
    m_settings->setRapidSpeed(set.value("rapidSpeed", 0).toInt());
    m_settings->setAcceleration(set.value("acceleration", 10).toInt());
    m_settings->setToolAngle(set.value("toolAngle", 0).toDouble());
    m_settings->setToolType(set.value("toolType", 0).toInt());
    m_settings->setFps(set.value("fps", 60).toInt());
    m_settings->setQueryStateTime(set.value("queryStateTime", 250).toInt());
    m_settings->setUseStartCommands(set.value("useStartCommands").toBool());
    m_settings->setStartCommands(set.value("startCommands").toString());
    m_settings->setUseEndCommands(set.value("useEndCommands").toBool());
    m_settings->setEndCommands(set.value("endCommands").toString());
    m_settings->setUsePauseCommands(set.value("usePauseCommands").toBool());
    m_settings->setBeforePauseCommands(set.value("beforePauseCommands").toString());
    m_settings->setAfterPauseCommands(set.value("afterPauseCommands").toString());
    m_settings->setToolChangeCommands(set.value("toolChangeCommands").toString());
    m_settings->setToolChangePause(set.value("toolChangePause").toBool());
    m_settings->setToolChangeUseCommands(set.value("toolChangeUseCommands").toBool());
    m_settings->setToolChangeUseCommandsConfirm(set.value("toolChangeUseCommandsConfirm").toBool());
    m_settings->setReferenceXPlus(set.value("referenceXPlus", false).toBool());
    m_settings->setReferenceYPlus(set.value("referenceYPlus", false).toBool());
    m_settings->setReferenceZPlus(set.value("referenceZPlus", false).toBool());
    m_settings->setLanguage(set.value("language", "en").toString());

    ui->chkAutoScroll->setChecked(set.value("autoScroll", false).toBool());

    ui->slbSpindle->setRatio(100);
    ui->slbSpindle->setMinimum(m_settings->spindleSpeedMin());
    ui->slbSpindle->setMaximum(m_settings->spindleSpeedMax());
    ui->slbSpindle->setValue(set.value("spindleSpeed", 100).toInt());

    ui->slbFeedOverride->setChecked(set.value("feedOverride", false).toBool());
    ui->slbFeedOverride->setValue(set.value("feedOverrideValue", 100).toInt());

    ui->slbRapidOverride->setChecked(set.value("rapidOverride", false).toBool());
    ui->slbRapidOverride->setValue(set.value("rapidOverrideValue", 100).toInt());

    ui->slbSpindleOverride->setChecked(set.value("spindleOverride", false).toBool());
    ui->slbSpindleOverride->setValue(set.value("spindleOverrideValue", 100).toInt());

    m_settings->setUnits(set.value("units", 0).toInt());

    m_recentFiles = set.value("recentFiles", QStringList()).toStringList();
    m_recentHeightmaps = set.value("recentHeightmaps", QStringList()).toStringList();
    m_lastFolder = set.value("lastFolder", QDir::homePath()).toString();

    this->restoreGeometry(set.value("formGeometry", QByteArray()).toByteArray());

    ui->cboCommand->setMinimumHeight(ui->cboCommand->height());
    ui->cmdClearConsole->setFixedHeight(ui->cboCommand->height());
    ui->cmdCommandSend->setFixedHeight(ui->cboCommand->height());

    m_storedKeyboardControl = set.value("keyboardControl", false).toBool();

    m_settings->setAutoCompletion(set.value("autoCompletion", true).toBool());

    QStringList steps = set.value("jogSteps").toStringList();
    if (steps.count() > 0) {
        steps.insert(0, ui->cboJogStep->items().first());
        ui->cboJogStep->setItems(steps);
    }
    ui->cboJogStep->setCurrentIndex(ui->cboJogStep->findText(set.value("jogStep").toString()));
    ui->cboJogFeed->setItems(set.value("jogFeeds").toStringList());
    ui->cboJogFeed->setCurrentIndex(ui->cboJogFeed->findText(set.value("jogFeed").toString()));

    ui->txtHeightMapBorderX->setValue(set.value("heightmapBorderX", 0).toDouble());
    ui->txtHeightMapBorderY->setValue(set.value("heightmapBorderY", 0).toDouble());
    ui->txtHeightMapBorderWidth->setValue(set.value("heightmapBorderWidth", 1).toDouble());
    ui->txtHeightMapBorderHeight->setValue(set.value("heightmapBorderHeight", 1).toDouble());
    ui->chkHeightMapBorderShow->setChecked(set.value("heightmapBorderShow", false).toBool());

    ui->txtHeightMapGridX->setValue(set.value("heightmapGridX", 1).toDouble());
    ui->txtHeightMapGridY->setValue(set.value("heightmapGridY", 1).toDouble());
    ui->txtHeightMapGridZTop->setValue(set.value("heightmapGridZTop", 1).toDouble());
    ui->txtHeightMapGridZBottom->setValue(set.value("heightmapGridZBottom", -1).toDouble());
    ui->txtHeightMapProbeFeed->setValue(set.value("heightmapProbeFeed", 10).toDouble());
    ui->chkHeightMapGridShow->setChecked(set.value("heightmapGridShow", false).toBool());

    ui->txtHeightMapInterpolationStepX->setValue(set.value("heightmapInterpolationStepX", 1).toDouble());
    ui->txtHeightMapInterpolationStepY->setValue(set.value("heightmapInterpolationStepY", 1).toDouble());
    ui->cboHeightMapInterpolationType->setCurrentIndex(set.value("heightmapInterpolationType", 0).toInt());
    ui->chkHeightMapInterpolationShow->setChecked(set.value("heightmapInterpolationShow", false).toBool());

    foreach (ColorPicker* pick, m_settings->colors()) {
        pick->setColor(QColor(set.value(pick->objectName().mid(3), "black").toString()));
    }

    updateRecentFilesMenu();

    ui->tblProgram->horizontalHeader()->restoreState(set.value("header", QByteArray()).toByteArray());

    // Apply settings
    applySettings();

    // Restore last commands list
    ui->cboCommand->addItems(set.value("recentCommands", QStringList()).toStringList());
    ui->cboCommand->setCurrentIndex(-1);

    // Load plugins
    loadPlugins();
    emit pluginsLoaded();

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
    setupCoordsTextboxes();

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


    // Loading stored script variables
    QScriptValue g = m_scriptEngine.globalObject();
    set.beginGroup("script");
    QStringList l = set.childKeys();
    foreach (const QString &k, l) {
        g.setProperty(k, m_scriptEngine.newVariant(set.value(k)));
    }
    set.endGroup();

    m_settingsLoading = false;
}

void frmMain::saveSettings()
{
    QSettings set(m_settingsFileName, QSettings::IniFormat);
    set.setIniCodec("UTF-8");

    emit settingsAboutToSave();

    set.setValue("connectionMode", m_settings->connectionMode());
    set.setValue("serialPort", m_settings->port());
    set.setValue("serialBaud", m_settings->baud());
    set.setValue("ignoreErrors", m_settings->ignoreErrors());
    set.setValue("autoLine", m_settings->autoLine());
    set.setValue("toolDiameter", m_settings->toolDiameter());
    set.setValue("toolLength", m_settings->toolLength());
    set.setValue("antialiasing", m_settings->antialiasing());
    set.setValue("msaa", m_settings->msaa());
    set.setValue("vsync", m_settings->vsync());
    set.setValue("zBuffer", m_settings->zBuffer());
    set.setValue("simplify", m_settings->simplify());
    set.setValue("simplifyPrecision", m_settings->simplifyPrecision());
    set.setValue("grayscaleSegments", m_settings->grayscaleSegments());
    set.setValue("grayscaleSCode", m_settings->grayscaleSCode());
    set.setValue("drawModeVectors", m_settings->drawModeVectors());

    set.setValue("spindleSpeed", ui->slbSpindle->value());
    set.setValue("lineWidth", m_settings->lineWidth());
    set.setValue("arcLength", m_settings->arcLength());
    set.setValue("arcDegree", m_settings->arcDegree());
    set.setValue("arcDegreeMode", m_settings->arcDegreeMode());
    set.setValue("showProgramCommands", m_settings->showProgramCommands());
    set.setValue("showUICommands", m_settings->showUICommands());
    set.setValue("spindleSpeedMin", m_settings->spindleSpeedMin());
    set.setValue("spindleSpeedMax", m_settings->spindleSpeedMax());
    set.setValue("laserPowerMin", m_settings->laserPowerMin());
    set.setValue("laserPowerMax", m_settings->laserPowerMax());
    set.setValue("rapidSpeed", m_settings->rapidSpeed());
    set.setValue("acceleration", m_settings->acceleration());
    set.setValue("toolAngle", m_settings->toolAngle());
    set.setValue("toolType", m_settings->toolType());
    set.setValue("fps", m_settings->fps());
    set.setValue("queryStateTime", m_settings->queryStateTime());
    set.setValue("autoScroll", ui->chkAutoScroll->isChecked());
    set.setValue("header", ui->tblProgram->horizontalHeader()->saveState());
    set.setValue("settingsSplitMain", m_settings->ui->splitMain->saveState());
    set.setValue("formGeometry", this->saveGeometry());
    set.setValue("formSettingsGeometry", m_settings->saveGeometry()); 

    set.setValue("autoCompletion", m_settings->autoCompletion());
    set.setValue("units", m_settings->units());
    set.setValue("recentFiles", m_recentFiles);
    set.setValue("recentHeightmaps", m_recentHeightmaps);
    set.setValue("lastFolder", m_lastFolder);
    set.setValue("fontSize", m_settings->fontSize());

    set.setValue("useStartCommands", m_settings->useStartCommands());
    set.setValue("startCommands", m_settings->startCommands());
    set.setValue("useEndCommands", m_settings->useEndCommands());
    set.setValue("endCommands", m_settings->endCommands());
    set.setValue("usePauseCommands", m_settings->usePauseCommands());
    set.setValue("afterPauseCommands", m_settings->afterPauseCommands());
    set.setValue("beforePauseCommands", m_settings->beforePauseCommands());
    set.setValue("toolChangeCommands", m_settings->toolChangeCommands());
    set.setValue("toolChangePause", m_settings->toolChangePause());
    set.setValue("toolChangeUseCommands", m_settings->toolChangeUseCommands());
    set.setValue("toolChangeUseCommandsConfirm", m_settings->toolChangeUseCommandsConfirm());
    set.setValue("referenceXPlus", m_settings->referenceXPlus());
    set.setValue("referenceYPlus", m_settings->referenceYPlus());
    set.setValue("referenceZPlus", m_settings->referenceZPlus());
    set.setValue("language", m_settings->language());

    set.setValue("feedOverride", ui->slbFeedOverride->isChecked());
    set.setValue("feedOverrideValue", ui->slbFeedOverride->value());
    set.setValue("rapidOverride", ui->slbRapidOverride->isChecked());
    set.setValue("rapidOverrideValue", ui->slbRapidOverride->value());
    set.setValue("spindleOverride", ui->slbSpindleOverride->isChecked());
    set.setValue("spindleOverrideValue", ui->slbSpindleOverride->value());

    set.setValue("jogSteps", (QStringList)ui->cboJogStep->items().mid(1, ui->cboJogStep->items().count() - 1));
    set.setValue("jogStep", ui->cboJogStep->currentText());
    set.setValue("jogFeeds", ui->cboJogFeed->items());
    set.setValue("jogFeed", ui->cboJogFeed->currentText());

    set.setValue("heightmapBorderX", ui->txtHeightMapBorderX->value());
    set.setValue("heightmapBorderY", ui->txtHeightMapBorderY->value());
    set.setValue("heightmapBorderWidth", ui->txtHeightMapBorderWidth->value());
    set.setValue("heightmapBorderHeight", ui->txtHeightMapBorderHeight->value());
    set.setValue("heightmapBorderShow", ui->chkHeightMapBorderShow->isChecked());

    set.setValue("heightmapGridX", ui->txtHeightMapGridX->value());
    set.setValue("heightmapGridY", ui->txtHeightMapGridY->value());
    set.setValue("heightmapGridZTop", ui->txtHeightMapGridZTop->value());
    set.setValue("heightmapGridZBottom", ui->txtHeightMapGridZBottom->value());
    set.setValue("heightmapProbeFeed", ui->txtHeightMapProbeFeed->value());
    set.setValue("heightmapGridShow", ui->chkHeightMapGridShow->isChecked());

    set.setValue("heightmapInterpolationStepX", ui->txtHeightMapInterpolationStepX->value());
    set.setValue("heightmapInterpolationStepY", ui->txtHeightMapInterpolationStepY->value());
    set.setValue("heightmapInterpolationType", ui->cboHeightMapInterpolationType->currentIndex());
    set.setValue("heightmapInterpolationShow", ui->chkHeightMapInterpolationShow->isChecked());

    foreach (ColorPicker* pick, m_settings->colors()) {
        set.setValue(pick->objectName().mid(3), pick->color().name());
    }

    QStringList list;

    for (int i = 0; i < ui->cboCommand->count(); i++) list.append(ui->cboCommand->itemText(i));
    set.setValue("recentCommands", list);

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

    // Save script variables
    QScriptEngine e;
    QScriptValue d = e.globalObject();
    QScriptValueIterator i(d);
    QStringList l;
    while (i.hasNext()) {
        i.next();
        l << i.name();
    }

    QScriptValue v = m_scriptEngine.globalObject();
    QScriptValueIterator it(v);
    while (it.hasNext()) {
        it.next();
        if (!l.contains(it.name())) {
            if (it.value().isNumber() || it.value().isString()) {
                set.setValue("script/" + it.name(), it.value().toVariant());
            }
        }
    }

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

    connect(m_connection, SIGNAL(lineReceived(QString)), this, SLOT(onConnectionLineReceived(QString)));
    connect(m_connection, SIGNAL(error(QString)), this, SLOT(onConnectionError(QString)));
}

void frmMain::applySettings() {
    m_originDrawer->setLineWidth(m_settings->lineWidth());
    m_toolDrawer.setToolDiameter(m_settings->toolDiameter());
    m_toolDrawer.setToolLength(m_settings->toolLength());
    m_toolDrawer.setLineWidth(m_settings->lineWidth());
    m_codeDrawer->setLineWidth(m_settings->lineWidth());
    m_heightMapBorderDrawer.setLineWidth(m_settings->lineWidth());
    m_heightMapGridDrawer.setLineWidth(0.1);
    m_heightMapInterpolationDrawer.setLineWidth(m_settings->lineWidth());
    ui->glwVisualizer->setLineWidth(m_settings->lineWidth());
    m_timerStateQuery.setInterval(m_settings->queryStateTime());

    m_toolDrawer.setToolAngle(m_settings->toolType() == 0 ? 180 : m_settings->toolAngle());
    m_toolDrawer.setColor(m_settings->colors("Tool"));
    m_toolDrawer.update();

    ui->glwVisualizer->setAntialiasing(m_settings->antialiasing());
    ui->glwVisualizer->setMsaa(m_settings->msaa());
    ui->glwVisualizer->setZBuffer(m_settings->zBuffer());
    ui->glwVisualizer->setVsync(m_settings->vsync());
    ui->glwVisualizer->setFps(m_settings->fps());
    ui->glwVisualizer->setColorBackground(m_settings->colors("VisualizerBackground"));
    ui->glwVisualizer->setColorText(m_settings->colors("VisualizerText"));

    ui->slbSpindle->setRatio((m_settings->spindleSpeedMax() - m_settings->spindleSpeedMin()) / 100);
    ui->slbSpindle->setMinimum(m_settings->spindleSpeedMin());
    ui->slbSpindle->setMaximum(m_settings->spindleSpeedMax());

    ui->cboCommand->setAutoCompletion(m_settings->autoCompletion());

    m_codeDrawer->setSimplify(m_settings->simplify());
    m_codeDrawer->setSimplifyPrecision(m_settings->simplifyPrecision());
    m_codeDrawer->setColorNormal(m_settings->colors("ToolpathNormal"));
    m_codeDrawer->setColorDrawn(m_settings->colors("ToolpathDrawn"));
    m_codeDrawer->setColorHighlight(m_settings->colors("ToolpathHighlight"));
    m_codeDrawer->setColorZMovement(m_settings->colors("ToolpathZMovement"));
    m_codeDrawer->setColorStart(m_settings->colors("ToolpathStart"));
    m_codeDrawer->setColorEnd(m_settings->colors("ToolpathEnd"));
    m_codeDrawer->setIgnoreZ(m_settings->grayscaleSegments() || !m_settings->drawModeVectors());
    m_codeDrawer->setGrayscaleSegments(m_settings->grayscaleSegments());
    m_codeDrawer->setGrayscaleCode(m_settings->grayscaleSCode() ? GcodeDrawer::S : GcodeDrawer::Z);
    m_codeDrawer->setDrawMode(m_settings->drawModeVectors() ? GcodeDrawer::Vectors : GcodeDrawer::Raster);
    m_codeDrawer->setGrayscaleMin(m_settings->laserPowerMin());
    m_codeDrawer->setGrayscaleMax(m_settings->laserPowerMax());
    m_codeDrawer->update();    

    m_selectionDrawer.setColor(m_settings->colors("ToolpathHighlight"));

    // Adapt visualizer buttons colors
    const int LIGHTBOUND = 127;
    const int NORMALSHIFT = 40;
    const int HIGHLIGHTSHIFT = 80;

    QColor base = m_settings->colors("VisualizerBackground");
    bool light = base.value() > LIGHTBOUND;
    QColor normal, highlight;

    normal.setHsv(base.hue(), base.saturation(), base.value() + (light ? -NORMALSHIFT : NORMALSHIFT));
    highlight.setHsv(base.hue(), base.saturation(), base.value() + (light ? -HIGHLIGHTSHIFT : HIGHLIGHTSHIFT));

    ui->glwVisualizer->setStyleSheet(QString("QToolButton {border: 1px solid %1; \
                background-color: %3} QToolButton:hover {border: 1px solid %2;}")
                .arg(normal.name()).arg(highlight.name())
                .arg(base.name()));

    ui->cmdFit->setIcon(QIcon(":/images/fit_1.png"));
    ui->cmdIsometric->setIcon(QIcon(":/images/cube.png"));
    ui->cmdFront->setIcon(QIcon(":/images/cubeFront.png"));
    ui->cmdLeft->setIcon(QIcon(":/images/cubeLeft.png"));
    ui->cmdTop->setIcon(QIcon(":/images/cubeTop.png"));

    if (!light) {
        Util::invertButtonIconColors(ui->cmdFit);
        Util::invertButtonIconColors(ui->cmdIsometric);
        Util::invertButtonIconColors(ui->cmdFront);
        Util::invertButtonIconColors(ui->cmdLeft);
        Util::invertButtonIconColors(ui->cmdTop);
    }

    int h = ui->cmdFileOpen->sizeHint().height();
    QSize s(h, h);
    ui->cboCommand->setMinimumHeight(h);
    ui->cmdClearConsole->setFixedSize(s);
    ui->cmdCommandSend->setFixedSize(s);

    if (m_connection->getSupportedMode() != m_settings->connectionMode()) {
        initializeConnection(m_settings->connectionMode());
        m_communicator->replaceConnection(m_connection);
    }
}

void frmMain::loadPlugins()
{
    QString pluginsDir = qApp->applicationDirPath() + "/plugins/";

    // Get plugins list
    QStringList pl = QDir(pluginsDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    qDebug() << "Loading plugins:" << pl;

    foreach (QString p, pl) {
        // Config
        QSettings set(pluginsDir + p + "/config.ini", QSettings::IniFormat);
        QString title = set.value("title").toString();
        QString name = set.value("name").toString();
        qDebug() << "Loading plugin:" << p << title << name;

        // Translation
        QString loc = m_settings->language();
        QString translationFileName = pluginsDir + p + "/translation_" + loc + ".qm";
        if(QFile::exists(translationFileName)) {
            QTranslator *translator = new QTranslator();
            if (translator->load(translationFileName)) qApp->installTranslator(translator);
            else delete translator;
        }        

        // Script
        QFile f;
        f.setFileName(pluginsDir + p + "/script.js");
        if (f.open(QFile::ReadOnly)) {
            QScriptEngine *se = new QScriptEngine();
            
            QScriptValue sv = se->newObject();
            sv.setProperty("importExtension", se->newFunction(frmMain::importExtension));
            sv.setProperty("path", pluginsDir + p);            
            se->globalObject().setProperty("script", sv);
            connect(se, &QScriptEngine::signalHandlerException, this, &frmMain::onScriptException);

            // Delegate objects
            // Main form
            QScriptValue app = se->newQObject(&m_scriptFunctions);
            app.setProperty("path", qApp->applicationDirPath());
            se->globalObject().setProperty("app", app);

            // // Settings
            QScriptValue settings = se->newQObject(m_settings);
            app.setProperty("settings", settings);

            // Stored vars
            QScriptValue vars = se->newQObject(&m_storedVars);
            se->globalObject().setProperty("vars", vars);

            // Translator
            se->installTranslatorFunctions();

            // Run script
            QString script = f.readAll();

            qDebug() << "Starting plugin:" << p;
            sv = se->evaluate(script);
            if (sv.isError()) {
                qDebug() << sv.toString();
                qDebug() << se->uncaughtExceptionBacktrace();
            }

            // Init
            sv = se->evaluate("init()");
            if (sv.isError()) {
                qDebug() << sv.toString();
                qDebug() << se->uncaughtExceptionBacktrace();
            }

            // Panel widget
            sv = se->evaluate("createPanelWidget()");
            if (sv.isError()) {
                qDebug() << sv.toString();
                qDebug() << se->uncaughtExceptionBacktrace();
            } else {
                QWidget *w = qobject_cast<QWidget*>(sv.toQObject());
                if (w) {
                    // Create panel
                    QGroupBox *box = new QGroupBox(this);
                    QVBoxLayout *layout1 = new QVBoxLayout(box);
                    QWidget *bw = new QWidget(box);
                    QVBoxLayout *layout2 = new QVBoxLayout(bw);
                    box->setObjectName("grp" + name);
                    box->setTitle(tr(title.toLatin1()));
                    box->setLayout(layout1);
                    box->setCheckable(true);
                    box->setProperty("overrided", false);
                    layout1->addWidget(bw);
                    bw->setLayout(layout2);
                    layout2->addWidget(w);
                    layout2->setMargin(0);
                    connect(box, &QGroupBox::toggled, bw, &QWidget::setVisible);

                    // Add panel to user window
                    static_cast<QVBoxLayout*>(ui->scrollContentsUser->layout())->insertWidget(0, box);                
                }
            }

            // Window widget
            sv = se->evaluate("createWindowWidget()");
            if (sv.isError()) {
                qDebug() << sv.toString();
                qDebug() << se->uncaughtExceptionBacktrace();
            } else {
                QWidget *w = qobject_cast<QWidget*>(sv.toQObject());
                if (w) {
                    // Create dock widget
                    QDockWidget *dock = new QDockWidget(this);
                    QWidget *contents = new QWidget(dock);
                    QFrame *frame = new QFrame(contents);
                    QVBoxLayout *layout1 = new QVBoxLayout(contents);
                    QVBoxLayout *layout2 = new QVBoxLayout(frame);
                    dock->setObjectName("dock" + name);
                    dock->setWindowTitle(tr(title.toLatin1()));
                    dock->setWidget(contents);
                    contents->setLayout(layout1);
                    layout1->addWidget(frame);
                    QMargins m = layout1->contentsMargins();
                    m.setLeft(0);
                    m.setRight(0);
                    layout1->setContentsMargins(m);
                    frame->setLayout(layout2);
                    layout2->addWidget(w);
                    layout2->setMargin(0);

                    // Add to main form
                    this->addDockWidget(Qt::RightDockWidgetArea, dock);
                }
            }

            // Settings widget
            sv = se->evaluate("createSettingsWidget()");
            if (sv.isError()) {
                qDebug() << sv.toString();
                qDebug() << se->uncaughtExceptionBacktrace();
            } else {
                QWidget *w = qobject_cast<QWidget*>(sv.toQObject());
                if (w) {
                    // Create groupbox
                    QGroupBox *box = new QGroupBox(m_settings);
                    QVBoxLayout *layout1 = new QVBoxLayout(box);
                    box->setObjectName("grpSettings" + name);
                    box->setTitle(tr(title.toLatin1()));
                    box->setLayout(layout1);
                    layout1->addWidget(w);

                    // Add to settings form
                    m_settings->addCustomSettings(box);
                }
            }

            f.close();
        }
    }
}

void frmMain::openPort()
{
    if (m_connection->openConnection()) {
        this->m_partState->setStatusText(tr("Port opened"), "palette(button)", "palette(text)");
        ui->txtStatus->setText(tr("Port opened"));
        ui->txtStatus->setStyleSheet(QString("background-color: palette(button); color: palette(text);"));
        grblReset();
    }
}

void frmMain::grblReset()
{
    m_communicator->reset();
    m_fileCommandIndex = 0;
    m_updateSpindleSpeed = true;
    updateControlsState();
}

void frmMain::writeConsole(QString command)
{
    ui->txtConsole->appendPlainText(command);
}

// SendCommandResult frmMain::sendCommand(QString command, int tableIndex, bool showInConsole, bool wait)
// {
//     // tableIndex:
//     // 0...n - commands from g-code program
//     // -1 - ui commands
//     // -2 - utility commands
//     // -3 - utility commands

//     if (!m_connection->isConnected() || !m_communicator->m_resetCompleted) return SendDone;

//     // Check command
//     if (command.isEmpty()) return SendEmpty;

//     // Place to queue on 'wait' flag
//     if (wait) {
//         m_communicator->m_queue.append(CommandQueue(command, tableIndex, showInConsole));
//         return SendQueue;
//     }
    
//     // Evaluate scripts in command
//     if (tableIndex < 0) command = evaluateCommand(command);

//     // Check evaluated command
//     if (command.isEmpty()) return SendEmpty;

//     // Place to queue if command buffer is full
//     if ((bufferLength() + command.length() + 1) > BUFFERLENGTH) {
//         m_communicator->m_queue.append(CommandQueue(command, tableIndex, showInConsole));
//         return SendQueue;
//     }

//     command = command.toUpper();

//     CommandAttributes ca;
//     if (showInConsole) {
//         writeConsole(command);
//         ca.consoleIndex = ui->txtConsole->blockCount() - 1;
//     } else {
//         ca.consoleIndex = -1;
//     }

//     ca.command = command;
//     ca.length = command.length() + 1;
//     ca.tableIndex = tableIndex;

//     m_communicator->m_commands.append(ca);

//     QString uncomment = GcodePreprocessorUtils::removeComment(command);

//     // Processing spindle speed only from g-code program
//     static QRegExp s("[Ss]0*(\\d+)");
//     if (s.indexIn(uncomment) != -1 && ca.tableIndex > -2) {
//         int speed = s.cap(1).toInt();
//         if (ui->slbSpindle->value() != speed) {
//             ui->slbSpindle->setValue(speed);
//         }
//     }

//     // Set M2 & M30 commands sent flag
//     static QRegExp M230("(M0*2|M30|M0*6|M25)(?!\\d)");
//     static QRegExp M6("(M0*6)(?!\\d)");
//     if ((m_communicator->m_senderState == SenderTransferring) && uncomment.contains(M230)) {
//         if (!uncomment.contains(M6) || m_settings->toolChangeUseCommands() || m_settings->toolChangePause()) m_communicator->setSenderState(SenderPausing);
//     }

//     // Queue offsets request on G92, G10 commands
//     static QRegExp G92("(G92|G10)(?!\\d)");
//     if (uncomment.contains(G92)) sendCommand("$#", -3, showInConsole, true);

//     m_connection->sendLine(command);

//     return SendDone;
// }

// void frmMain::sendRealtimeCommand(QString command)
// {
//     if (command.length() != 1) return;
//     if (!m_connection->isConnected() || !m_communicator->m_resetCompleted) return;

//     m_connection->sendByteArray(QByteArray(command.toLatin1(), 1));
// }

// void frmMain::sendCommands(QString commands, int tableIndex)
// {
//     QStringList list = commands.split("\n");

//     bool q = false;
//     foreach (QString cmd, list) {
//         SendCommandResult r = sendCommand(cmd.trimmed(), tableIndex, m_settings->showUICommands(), q);
//         if (r == SendDone || r == SendQueue) q = true;
//     }
// }

void frmMain::sendNextFileCommands() {
    if (m_communicator->m_queue.length() > 0) return;

    QString command = m_currentModel->data(m_currentModel->index(m_fileCommandIndex, 1)).toString();
    static QRegExp M230("(M0*2|M30|M0*6)(?!\\d)");

    while ((bufferLength() + command.length() + 1) <= BUFFERLENGTH
        && m_fileCommandIndex < m_currentModel->rowCount() - 1
        && !(!m_communicator->m_commands.isEmpty() && GcodePreprocessorUtils::removeComment(m_communicator->m_commands.last().command).contains(M230))
        ) 
    {
        m_currentModel->setData(m_currentModel->index(m_fileCommandIndex, 2), GCodeItem::Sent);
        m_communicator->sendCommand(command, m_fileCommandIndex, m_settings->showProgramCommands());
        m_fileCommandIndex++;
        command = m_currentModel->data(m_currentModel->index(m_fileCommandIndex, 1)).toString();
    }
}

QString frmMain::evaluateCommand(QString command)
{
    // Evaluate script  
    static QRegularExpression rx("\\{(?:(?>[^\\{\\}])|(?0))*\\}");
    QRegularExpressionMatch m; 
    QScriptValue v;
    QString vs;
   
    while ((m = rx.match(command)).hasMatch()) {
        v = m_scriptEngine.evaluate(m.captured(0));
        vs = v.isUndefined() ? "" : v.isNumber() ? QString::number(v.toNumber(), 'f', 4) : v.toString();
        command.replace(m.captured(0), vs);
    }
    
    return command;
}

void frmMain::updateParser()
{
    GcodeViewParse *parser = m_currentDrawer->viewParser();

    GcodeParser gp;
    gp.setTraverseSpeed(m_settings->rapidSpeed());
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

    updateProgramEstimatedTime(parser->getLinesFromParser(&gp, m_settings->arcPrecision(), m_settings->arcDegreeMode()));
    m_currentDrawer->update();
    ui->glwVisualizer->updateExtremes(m_currentDrawer);
    updateControlsState();

    if (m_currentModel == &m_programModel) m_fileChanged = true;
}

void frmMain::storeParserState()
{    
    m_storedParserStatus = ui->glwVisualizer->parserStatus().remove(
                QRegExp("GC:|\\[|\\]|G[01234]\\s|M[0345]+\\s|\\sF[\\d\\.]+|\\sS[\\d\\.]+"));
}

void frmMain::restoreParserState()
{
    if (!m_storedParserStatus.isEmpty()) m_communicator->sendCommand(m_storedParserStatus, -1, m_settings->showUICommands());
}

void frmMain::storeOffsetsVars(QString response)
{
    static QRegExp gx("\\[(G5[4-9]|G28|G30|G92|PRB):([\\d\\.\\-]+),([\\d\\.\\-]+),([\\d\\.\\-]+)");
    static QRegExp tx("\\[(TLO):([\\d\\.\\-]+)");

    int p = 0;
    while ((p = gx.indexIn(response, p)) != -1) {
        m_storedVars.setCoords(gx.cap(1), QVector3D(
            gx.cap(2).toDouble(),
            gx.cap(3).toDouble(),
            gx.cap(4).toDouble()
        ));
            
        p += gx.matchedLength();
    }

    if (tx.indexIn(response) != -1) {
        m_storedVars.setCoords(tx.cap(1), QVector3D(
            0,
            0,
            tx.cap(2).toDouble()
        ));
    }
}

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
    // Reset tables
    clearTable();
    m_probeModel.clear();
    m_programHeightmapModel.clear();
    m_currentModel = &m_programModel;

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
    gp.setTraverseSpeed(m_settings->rapidSpeed());
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

    updateProgramEstimatedTime(m_viewParser.getLinesFromParser(&gp, m_settings->arcPrecision(), m_settings->arcDegreeMode()));

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

    if (m_heightMapChanged) {
        int res = QMessageBox::warning(this, this->windowTitle(), tr("Heightmap file was changed. Save?"),
                                       QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (res == QMessageBox::Cancel) return false;
        else if (res == QMessageBox::Yes) {
            m_heightMapMode = true;
            on_actFileSave_triggered();
            m_heightMapMode = heightMapMode;
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

void frmMain::loadHeightMap(QString fileName)
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

    updateHeightMapBorderDrawer();

    m_heightMapModel.clear();   // To avoid probe data wipe message
    updateHeightMapGrid();

    list = textStream.readLine().split(";");

    for (int i = 0; i < m_heightMapModel.rowCount(); i++) {
        QList<QString> row = textStream.readLine().split(";");
        for (int j = 0; j < m_heightMapModel.columnCount(); j++) {
            m_heightMapModel.setData(m_heightMapModel.index(i, j), row[j].toDouble(), Qt::UserRole);
        }
    }

    file.close();

    ui->txtHeightMap->setText(fileName.mid(fileName.lastIndexOf("/") + 1));
    m_heightMapFileName = fileName;
    m_heightMapChanged = false;

    ui->cboHeightMapInterpolationType->setCurrentIndex(list[0].toInt());
    ui->txtHeightMapInterpolationStepX->setValue(list[1].toDouble());
    ui->txtHeightMapInterpolationStepY->setValue(list[2].toDouble());

    updateHeightMapInterpolationDrawer();
}

bool frmMain::saveHeightMap(QString fileName)
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

    for (int i = 0; i < m_heightMapModel.rowCount(); i++) {
        for (int j = 0; j < m_heightMapModel.columnCount(); j++) {
            textStream << m_heightMapModel.data(m_heightMapModel.index(i, j), Qt::UserRole).toString() << ((j == m_heightMapModel.columnCount() - 1) ? "" : ";");
        }
        textStream << "\r\n";
    }

    file.close();

    m_heightMapChanged = false;

    return true;
}

void frmMain::clearTable()
{
    m_programModel.clear();
    m_programModel.insertRow(0);
}

void frmMain::resetHeightmap()
{
    delete m_heightMapInterpolationDrawer.data();
    m_heightMapInterpolationDrawer.setData(NULL);

    ui->tblHeightMap->setModel(NULL);
    m_heightMapModel.resize(1, 1);

    ui->txtHeightMap->clear();
    m_heightMapFileName.clear();
    m_heightMapChanged = false;
}

void frmMain::newFile()
{
    // Reset tables
    clearTable();
    m_probeModel.clear();
    m_programHeightmapModel.clear();
    m_currentModel = &m_programModel;

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
    m_heightMapModel.clear();
    on_cmdFileReset_clicked();
    ui->txtHeightMap->setText(tr("Untitled"));
    m_heightMapFileName.clear();

    updateHeightMapBorderDrawer();
    updateHeightMapGrid();

    m_heightMapChanged = false;

    updateControlsState();
}

void frmMain::setupCoordsTextboxes()
{
    int prec = m_settings->units() == 0 ? 3 : 4;
    int bound = m_settings->units() == 0 ? 9999 : 999;

    ui->txtMPosX->setDecimals(prec);
    ui->txtMPosX->setMinimum(-bound);
    ui->txtMPosX->setMaximum(bound);
    ui->txtMPosY->setDecimals(prec);
    ui->txtMPosY->setMinimum(-bound);
    ui->txtMPosY->setMaximum(bound);
    ui->txtMPosZ->setDecimals(prec);
    ui->txtMPosZ->setMinimum(-bound);
    ui->txtMPosZ->setMaximum(bound);

    ui->txtWPosX->setDecimals(prec);
    ui->txtWPosX->setMinimum(-bound);
    ui->txtWPosX->setMaximum(bound);
    ui->txtWPosY->setDecimals(prec);
    ui->txtWPosY->setMinimum(-bound);
    ui->txtWPosY->setMaximum(bound);
    ui->txtWPosZ->setDecimals(prec);
    ui->txtWPosZ->setMinimum(-bound);
    ui->txtWPosZ->setMaximum(bound);    
}

void frmMain::updateControlsState() {
    bool portOpened = m_connection->isConnected();
    bool process = (m_communicator->m_senderState == SenderTransferring) || (m_communicator->m_senderState == SenderStopping);
    bool paused = (m_communicator->m_senderState == SenderPausing) || (m_communicator->m_senderState == SenderPausing2) || (m_communicator->m_senderState == SenderPaused) || (m_communicator->m_senderState == SenderChangingTool);

    ui->grpState->setEnabled(portOpened);
    ui->grpControl->setEnabled(portOpened);
    ui->widgetSpindle->setEnabled(portOpened);
    ui->widgetJog->setEnabled(portOpened && ((m_communicator->m_senderState == SenderStopped)
        || (m_communicator->m_senderState == SenderChangingTool)));
    ui->cboCommand->setEnabled(portOpened && (!ui->chkKeyboardControl->isChecked()));
    ui->cmdCommandSend->setEnabled(portOpened);

    ui->cmdCheck->setEnabled(portOpened && !process);
    ui->cmdHome->setEnabled(!process);
    ui->cmdCheck->setEnabled(!process);
    ui->cmdUnlock->setEnabled(!process);
    ui->cmdSpindle->setEnabled(!process);
    ui->cmdSleep->setEnabled(!process);

    ui->actFileNew->setEnabled(m_communicator->m_senderState == SenderStopped);
    ui->actFileOpen->setEnabled(m_communicator->m_senderState == SenderStopped);
    ui->cmdFileOpen->setEnabled(m_communicator->m_senderState == SenderStopped);
    ui->cmdFileReset->setEnabled((m_communicator->m_senderState == SenderStopped) && m_programModel.rowCount() > 1);
    ui->cmdFileSend->setEnabled(portOpened && (m_communicator->m_senderState == SenderStopped) && m_programModel.rowCount() > 1);
    switch (m_communicator->m_senderState) {
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
    ui->cmdFilePause->setEnabled(portOpened && (process || paused) && (m_communicator->m_senderState != SenderPausing) && (m_communicator->m_senderState != SenderPausing2));
    ui->cmdFilePause->setChecked(paused);
    ui->cmdFileAbort->setEnabled(m_communicator->m_senderState != SenderStopped && m_communicator->m_senderState != SenderStopping);
    ui->mnuRecent->setEnabled((m_communicator->m_senderState == SenderStopped) && ((m_recentFiles.count() > 0 && !m_heightMapMode)
                                                      || (m_recentHeightmaps.count() > 0 && m_heightMapMode)));
    ui->actFileSave->setEnabled(m_programModel.rowCount() > 1);
    ui->actFileSaveAs->setEnabled(m_programModel.rowCount() > 1);

    ui->tblProgram->setEditTriggers((m_communicator->m_senderState != SenderStopped) ? QAbstractItemView::NoEditTriggers :
        QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | 
        QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);

    if (!portOpened) {
        this->m_partState->setStatusText(tr("Not connected"), "palette(button)", "palette(text)");
        ui->txtStatus->setText(tr("Not connected"));
        ui->txtStatus->setStyleSheet(QString("background-color: palette(button); color: palette(text);"));
        emit deviceStateChanged(-1);
    }

    this->setWindowTitle(m_programFileName.isEmpty() ? qApp->applicationDisplayName()
                                                     : m_programFileName.mid(m_programFileName.lastIndexOf("/") + 1) + " - " + qApp->applicationDisplayName());

    if (!process) ui->chkKeyboardControl->setChecked(m_storedKeyboardControl);

#ifdef WINDOWS
    if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7 && m_taskBarProgress) {
        m_taskBarProgress->setPaused(paused);
        if (m_communicator->m_senderState == SenderStopped) m_taskBarProgress->hide();
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
    m_heightMapBorderDrawer.setVisible(ui->chkHeightMapBorderShow->isChecked() && m_heightMapMode);
    m_heightMapGridDrawer.setVisible(ui->chkHeightMapGridShow->isChecked() && m_heightMapMode);
    m_heightMapInterpolationDrawer.setVisible(ui->chkHeightMapInterpolationShow->isChecked() && m_heightMapMode);

    ui->grpProgram->setText(m_heightMapMode ? tr("Heightmap") : tr("G-code program"));
    ui->grpProgram->setProperty("overrided", m_heightMapMode);
    style()->unpolish(ui->grpProgram);
    ui->grpProgram->ensurePolished();

    ui->cboJogStep->setEditable(!ui->chkKeyboardControl->isChecked());
    ui->cboJogFeed->setEditable(!ui->chkKeyboardControl->isChecked());
    ui->cboJogStep->setEnabled(!ui->chkKeyboardControl->isChecked());
    ui->cboJogFeed->setEnabled(!ui->chkKeyboardControl->isChecked());
    ui->cboJogStep->setStyleSheet(QString("font-size: %1").arg(m_settings->fontSize()));
    ui->cboJogFeed->setStyleSheet(ui->cboJogStep->styleSheet());

    ui->tblHeightMap->setVisible(m_heightMapMode);
    ui->tblProgram->setVisible(!m_heightMapMode);

    ui->widgetHeightMap->setEnabled(!process && m_programModel.rowCount() > 1);
    ui->cmdHeightMapMode->setEnabled(!ui->txtHeightMap->text().isEmpty());

    ui->cmdFileSend->setText(m_heightMapMode ? tr("Probe") : tr("Send"));

    ui->chkHeightMapUse->setEnabled(!m_heightMapMode && !ui->txtHeightMap->text().isEmpty());

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

    foreach (QString file, !m_heightMapMode ? m_recentFiles : m_recentHeightmaps) {
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
    bool smallStep = abs(target - slider->currentValue()) < 10 || m_settings->queryStateTime() < 100;

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

void frmMain::updateHeightMapBorderDrawer()
{
    if (m_settingsLoading) return;

    m_heightMapBorderDrawer.setBorderRect(borderRectFromTextboxes());
}

bool frmMain::updateHeightMapGrid()
{
    if (m_settingsLoading) return true;

    // Grid map changing warning
    bool nan = true;
    for (int i = 0; i < m_heightMapModel.rowCount(); i++)
        for (int j = 0; j < m_heightMapModel.columnCount(); j++)
            if (!qIsNaN(m_heightMapModel.data(m_heightMapModel.index(i, j), Qt::UserRole).toDouble())) {
                nan = false;
                break;
            }
    if (!nan && QMessageBox::warning(this, this->windowTitle(), tr("Changing grid settings will reset probe data. Continue?"),
                                                           QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) return false;

    // Update grid drawer
    QRectF borderRect = borderRectFromTextboxes();
    m_heightMapGridDrawer.setBorderRect(borderRect);
    m_heightMapGridDrawer.setGridSize(QPointF(ui->txtHeightMapGridX->value(), ui->txtHeightMapGridY->value()));
    m_heightMapGridDrawer.setZBottom(ui->txtHeightMapGridZBottom->value());
    m_heightMapGridDrawer.setZTop(ui->txtHeightMapGridZTop->value());

    // Reset model
    int gridPointsX = ui->txtHeightMapGridX->value();
    int gridPointsY = ui->txtHeightMapGridY->value();

    m_heightMapModel.resize(gridPointsX, gridPointsY);
    ui->tblHeightMap->setModel(NULL);
    ui->tblHeightMap->setModel(&m_heightMapModel);
    resizeTableHeightMapSections();

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

    m_heightMapChanged = true;
    return true;
}

void frmMain::updateHeightMapGrid(double arg1)
{
    if (sender()->property("previousValue").toDouble() != arg1 && !updateHeightMapGrid())
        static_cast<QDoubleSpinBox*>(sender())->setValue(sender()->property("previousValue").toDouble());
    else sender()->setProperty("previousValue", arg1);
}

void frmMain::resizeTableHeightMapSections()
{
    if (ui->tblHeightMap->horizontalHeader()->defaultSectionSize()
            * ui->tblHeightMap->horizontalHeader()->count() < ui->glwVisualizer->width())
        ui->tblHeightMap->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); else {
        ui->tblHeightMap->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    }
}

bool frmMain::eventFilter(QObject *obj, QEvent *event)
{
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

        if ((m_communicator->m_senderState != SenderTransferring) && (m_communicator->m_senderState != SenderStopping)
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
    } else if (obj == ui->tblProgram && ((m_communicator->m_senderState == SenderTransferring) || (m_communicator->m_senderState == SenderStopping))) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_PageDown || keyEvent->key() == Qt::Key_PageUp
                    || keyEvent->key() == Qt::Key_Down || keyEvent->key() == Qt::Key_Up) {
            ui->chkAutoScroll->setChecked(false);
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
            m_communicator->sendCommand(QString("$J=%5G91X%1Y%2Z%3F%4")
                        .arg(vec.x(), 0, 'f', m_settings->units() ? 4 : 3)
                        .arg(vec.y(), 0, 'f', m_settings->units() ? 4 : 3)
                        .arg(vec.z(), 0, 'f', m_settings->units() ? 4 : 3)
                        .arg(ui->cboJogFeed->currentText().toDouble())
                        .arg(m_settings->units() ? "G20" : "G21")
                        , -3, m_settings->showUICommands());
        }
    }
}

void frmMain::jogContinuous()
{
    static bool block = false;
    static QVector3D v;

    if ((ui->cboJogStep->currentText().toDouble() == 0) && !block) {

        if (m_jogVector != v) {
            // Store jog vector before block
            QVector3D j = m_jogVector;

            if (v.length()) {
                block = true;
                m_connection->sendByteArray(QByteArray(1, char(0x85)));
                while (m_communicator->m_deviceState == DeviceJog) qApp->processEvents();
                block = false;
            }
            
            // Bounds
            QVector3D b = m_settings->machineBounds();
            // Current machine coords
            QVector3D m(m_communicator->toMetric(m_storedVars.Mx()), m_communicator->toMetric(m_storedVars.My()), m_communicator->toMetric(m_storedVars.Mz()));
            // Distance to bounds
            QVector3D t;
            // Minimum distance to bounds
            double d = 0;
            if (m_settings->softLimitsEnabled()) {
                t = QVector3D(j.x() * b.x() < 0 ? 0 - m.x() : b.x() - m.x(), 
                              j.y() * b.y() < 0 ? 0 - m.y() : b.y() - m.y(),
                              j.z() * b.z() < 0 ? 0 - m.z() : b.z() - m.z());
                for (int i = 0; i < 3; i++) if ((j[i] && (qAbs(t[i]) < d)) || (j[i] && !d)) d = qAbs(t[i]);
                // Coords not aligned, add some bounds offset
                d -= m_settings->units() ? m_communicator->toMetric(0.0005) : 0.005;
            } else {
                for (int i = 0; i < 3; i++) if (j[i] && (qAbs(b[i]) > d)) d = qAbs(b[i]);
            }

            // Jog vector
            QVector3D vec = j * m_communicator->toInches(d);

            if (vec.length()) {
                m_communicator->sendCommand(QString("$J=%5G91X%1Y%2Z%3F%4")
                            .arg(vec.x(), 0, 'f', m_settings->units() ? 4 : 3)
                            .arg(vec.y(), 0, 'f', m_settings->units() ? 4 : 3)
                            .arg(vec.z(), 0, 'f', m_settings->units() ? 4 : 3)
                            .arg(ui->cboJogFeed->currentText().toDouble())
                            .arg(m_settings->units() ? "G20" : "G21")
                            , -2, m_settings->showUICommands());
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

void frmMain::completeTransfer()
{
    // Shadow last segment
    GcodeViewParse *parser = m_currentDrawer->viewParser();
    QList<LineSegment*> list = parser->getLineSegmentList();
    if (m_lastDrawnLineIndex < list.count()) {
        list[m_lastDrawnLineIndex]->setDrawn(true);
        m_currentDrawer->update(QList<int>() << m_lastDrawnLineIndex);
    }

    // Update state
    m_communicator->setSenderState(SenderStopped);
    m_fileProcessedCommandIndex = 0;
    m_lastDrawnLineIndex = 0;
    m_storedParserStatus.clear();

    updateControlsState();

    // Send end commands
    if (m_settings->useEndCommands()) m_communicator->sendCommands(m_settings->endCommands());

    // Show message box
    qApp->beep();
    m_timerStateQuery.stop();
    m_timerConnection.stop();

    QMessageBox::information(this, qApp->applicationDisplayName(), tr("Job done.\nTime elapsed: %1")
                                .arg(ui->glwVisualizer->spendTime().toString("hh:mm:ss")));

    m_timerStateQuery.setInterval(m_settings->queryStateTime());
    m_timerConnection.start();
    m_timerStateQuery.start();
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

QScriptValue frmMain::importExtension(QScriptContext *context, QScriptEngine *engine)
{
    return engine->importExtension(context->argument(0).toString());
}
