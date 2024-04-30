// This file is a part of "Candle" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich

#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <QBasicTimer>
#include <QStringList>
#include <QList>
#include <QTime>
#include <QMenu>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QProgressDialog>
#include <QGroupBox>
#include <exception>

#include "connection/connection.h"
#include "communicator.h"
#include "connection/connectionmanager.h"
#include "drawers/cursordrawer.h"
#include "drawers/tablesurfacedrawer.h"
#include "streamer.h"
#include "globals.h"

#include "connection/connection.h"
#include "form_partial/main/partmainjog.h"
#include "form_partial/main/partmainstate.h"
#include "form_partial/main/partmainconsole.h"
#include "frmgrblconfigurator.h"
#include "parser/gcodeviewparse.h"

#include "drawers/origindrawer.h"
#include "drawers/gcodedrawer.h"
#include "drawers/tooldrawer.h"
#include "drawers/heightmapborderdrawer.h"
#include "drawers/heightmapgriddrawer.h"
#include "drawers/heightmapinterpolationdrawer.h"
#include "drawers/shaderdrawable.h"
#include "drawers/selectiondrawer.h"
#include "drawers/machineboundsdrawer.h"

#include "tables/gcodetablemodel.h"
#include "tables/heightmaptablemodel.h"

#include "utils/interpolation.h"

#include "styledtoolbutton.h"
#include "sliderbox.h"

#include "frmsettings.h"
#include "frmabout.h"

#include "scripting/scriptvars.h"

#ifdef WINDOWS
    #include <QtWinExtras/QtWinExtras>
    #include "shobjidl.h"
#endif

namespace Ui {
class frmMain;
class frmProgram;
}

class CancelException : public std::exception {
public:
#ifdef Q_OS_MAC
#undef _GLIBCXX_USE_NOEXCEPT
#define _GLIBCXX_USE_NOEXCEPT _NOEXCEPT
#endif

    const char* what() const throw()
    {
        return "Operation was cancelled by user";
    }
};

class frmMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();
    
    //void writeConsole(QString command);
    void initializeCommunicator();
        
signals:
    void responseReceived(QString command, int tableIndex, QString response);
    void statusReceived(QString status);
    void senderStateChanged(int state);
    void deviceStateChanged(int state);
    void settingsAboutToLoad();
    void settingsLoaded();
    void settingsAboutToSave();
    void settingsSaved();
    void settingsAboutToShow();
    void settingsAccepted();
    void settingsRejected();
    void settingsSetToDefault();
    void pluginsLoaded();

private slots:
    void on_actFileNew_triggered();
    void on_actFileOpen_triggered();
    void on_actFileSave_triggered();
    void on_actFileSaveAs_triggered();
    void on_actFileSaveTransformedAs_triggered();
    void on_actRecentClear_triggered();
    void on_actFileExit_triggered();
    void on_actServiceSettings_triggered();
    void on_actServiceConfigureGRBL_triggered();
    void on_actAbout_triggered();
    // void on_actJogStepNext_triggered();
    // void on_actJogStepPrevious_triggered();
    // void on_actJogFeedNext_triggered();
    // void on_actJogFeedPrevious_triggered();
    void on_actSpindleSpeedPlus_triggered();
    void on_actSpindleSpeedMinus_triggered();
    void on_actViewLockWindows_toggled(bool checked);
    void on_cmdFileOpen_clicked();
    void on_cmdFileSend_clicked();
    void on_cmdFilePause_clicked(bool checked);
    void on_cmdFileAbort_clicked();
    void on_cmdFileReset_clicked();
//    void on_cmdCommandSend_clicked();
//    void on_cmdClearConsole_clicked();
    void on_cmdHome_clicked();
    void on_cmdCheck_clicked(bool checked);
    void on_cmdReset_clicked();
    void on_cmdUnlock_clicked();
    void on_cmdHold_clicked(bool checked);
    void on_cmdSleep_clicked();
    void on_cmdDoor_clicked();
    void on_cmdFlood_clicked();
    void on_cmdSpindle_toggled(bool checked);
    void on_cmdSpindle_clicked(bool checked);
    void on_cmdTop_clicked();
    void on_cmdFront_clicked();
    void on_cmdLeft_clicked();
    void on_cmdIsometric_clicked();
    void on_cmdToggleProjection_clicked();
    void on_cmdFit_clicked();
    void on_grpOverriding_toggled(bool checked);
    void on_grpSpindle_toggled(bool checked);
    void on_grpJog_toggled(bool checked);
    void on_grpHeightMap_toggled(bool arg1);
    void on_chkKeyboardControl_toggled(bool checked);
    void on_chkHeightMapBorderShow_toggled(bool checked);
    void on_chkHeightMapInterpolationShow_toggled(bool checked);
    void on_chkHeightMapUse_clicked(bool checked);
    void on_chkHeightMapGridShow_toggled(bool checked);
    void on_txtHeightMapBorderX_valueChanged(double arg1);
    void on_txtHeightMapBorderWidth_valueChanged(double arg1);
    void on_txtHeightMapBorderY_valueChanged(double arg1);
    void on_txtHeightMapBorderHeight_valueChanged(double arg1);
    void on_txtHeightMapGridX_valueChanged(double arg1);
    void on_txtHeightMapGridY_valueChanged(double arg1);
    void on_txtHeightMapGridZBottom_valueChanged(double arg1);
    void on_txtHeightMapGridZTop_valueChanged(double arg1);
    void on_txtHeightMapInterpolationStepX_valueChanged(double arg1);
    void on_txtHeightMapInterpolationStepY_valueChanged(double arg1);
    void on_cmdHeightMapMode_toggled(bool checked);
    void on_cmdHeightMapCreate_clicked();
    void on_cmdHeightMapLoad_clicked();
    void on_cmdHeightMapBorderAuto_clicked();
    void on_tblProgram_customContextMenuRequested(const QPoint &pos);
    void on_mnuViewWindows_aboutToShow();
    void on_mnuViewPanels_aboutToShow();
    void on_dockVisualizer_visibilityChanged(bool visible);

    void onMachinePosChanged(QVector3D pos);
    void onWorkPosChanged(QVector3D pos);
    void onDeviceStateChanged(DeviceState state);
    void onDeviceStateReceived(DeviceState state);
    void onSenderStateReceived(SenderState state);
    void onSpindleStateReceived(bool state);
    void onFloodStateReceived(bool state);
    void onParserStateReceived(QString state);
    void onPinStateReceived(QString state);
    void onFeedSpindleSpeedReceived(int feedRate, int spindleSpeed);
    void onSpindleSpeedReceived(int spindleSpeed);
    void onOverridesReceived(int feedOverride, int spindleOverride, int rapidOverride);
    void onAborted();
    void onResponseReceived(QString command, int tableIndex, QString response);
    void onCommandResponseReceived(CommandAttributes commandAttributes);
    void onCommandSent(CommandAttributes commandAttributes);
    // @TODO signal does not make sense, it has to done in other way
    void onCommandProcessed(int tableIndex, QString response);
    void onConfigurationReceived(MachineConfiguration, QMap<int, double>);
    void onToolPositionReceived(QVector3D pos);
    void onTransferCompleted();
    void onConnectionError(QString error);
    void onConsoleNewCommand(QString command);

    void onTimerConnection();
    void onTableInsertLine();
    void onTableDeleteLines();
    void onTableCellChanged(QModelIndex i1, QModelIndex i2);
    void onTableCurrentChanged(QModelIndex idx1, QModelIndex idx2);
    void onOverridingToggled(bool checked);
    void onOverrideChanged();
    void onActRecentFileTriggered();
    void onActSendFromLineTriggered();
    void onSlbSpindleValueUserChanged();
    void onSlbSpindleValueChanged();
//    void onCboCommandReturnPressed();
    void onDockTopLevelChanged(bool topLevel);
    void onScroolBarAction(int action);
    void onVisualizerCursorPosChanged(QPointF);
    void updateHeightMapInterpolationDrawer(bool reset = false);
    void placeVisualizerButtons();

protected:
    void showEvent(QShowEvent *se) override;
    void hideEvent(QHideEvent *he) override;
    void resizeEvent(QResizeEvent *re) override;
    void timerEvent(QTimerEvent *) override;
    void closeEvent(QCloseEvent *ce) override;
    void dragEnterEvent(QDragEnterEvent *dee) override;
    void dropEvent(QDropEvent *de) override;
    QMenu *createPopupMenu() override;

private:
    static const int PROGRESSMINLINES = 10000;
    static const int PROGRESSSTEP = 1000;    

    // Ui
    Ui::frmMain *ui;

    QMenu *m_tableMenu;
    QMessageBox* m_senderErrorBox;
#ifdef WINDOWS
    QWinTaskbarButton *m_taskBarButton;
    QWinTaskbarProgress *m_taskBarProgress;
#endif

    // Parsers
    GcodeViewParse m_viewParser;
    GcodeViewParse m_probeParser;

    // Visualizer drawers
    // TODO: Add machine table visualizer
    TableSurfaceDrawer m_tableSurfaceDrawer;
    OriginDrawer *m_originDrawer;
    GcodeDrawer *m_codeDrawer;    
    GcodeDrawer *m_probeDrawer;
    GcodeDrawer *m_currentDrawer;
    ToolDrawer m_toolDrawer;
    CursorDrawer m_cursorDrawer;
    HeightMapBorderDrawer m_heightmapBorderDrawer;
    HeightMapGridDrawer m_heightmapGridDrawer;
    HeightMapInterpolationDrawer m_heightmapInterpolationDrawer;
    SelectionDrawer m_selectionDrawer;
    MachineBoundsDrawer m_machineBoundsDrawer;

    // Table models
    GCodeTableModel m_programModel;
    GCodeTableModel m_probeModel;
    GCodeTableModel m_programHeightmapModel;
    GCodeTableModel *m_currentModel;
    HeightmapTableModel m_heightmapModel;

    // @TODO to be moved to separate core class
    ConnectionManager m_connectionManager;
    Connection *m_connection;
    Communicator *m_communicator;
    Streamer *m_streamer;

    // Forms
    frmSettings *m_settings;
    frmAbout m_frmAbout;
    frmGrblConfigurator *m_grblConfigurator;

    // Filenames
    QString m_settingsFileName;
    QString m_programFileName;
    QString m_heightmapFileName;

    // Timers
    QTimer m_timerConnection;
    QBasicTimer m_timerToolAnimation;
    QTime m_startTime;

    // Flags
    bool m_programLoading;
    bool m_settingsLoading;
    bool m_fileChanged;
    bool m_heightmapChanged;

    // bool m_updateSpindleSpeed;
    // bool m_updateParserStatus;

    bool m_heightmapMode;

    // Current values
    int m_lastDrawnLineIndex;

    // Keyboard
    bool m_absoluteCoordinates;
    //bool m_storedKeyboardControl;

    Configuration m_configuration;
    ScriptVars m_scriptVars;

    // Drag & drop
    QPoint m_mousePressPos;
    
    // Settings
    void preloadSettings();
    void loadSettings();
    void saveSettings();
    void applySettings();

    // Plugins
    void loadPlugins();

    // Communication
    void openPort();
    QString evaluateCommand(QString command);

    // Parser
    void updateParser();

    // Files/models
    void loadFile(QString fileName);
    void loadFile(QList<std::string> data);
    bool saveChanges(bool heightmapMode);
    bool saveProgramToFile(QString fileName, GCodeTableModel *model);
    void loadHeightmap(QString fileName);
    bool saveHeightmap(QString fileName);
    void clearTable();
    void resetHeightmap();
    void newFile();
    void newHeightmap();

    // Ui
    void updateControlsState();
    void updateLayouts();
    void updateRecentFilesMenu();
    void updateOverride(SliderBox *slider, int value, char command);
    void updateJogTitle();
    void addRecentFile(QString fileName);
    void addRecentHeightmap(QString fileName);
    QRectF borderRectFromTextboxes();
    QRectF borderRectFromExtremes();
    void updateHeightmapBorderDrawer();
    bool updateHeightmapGrid();
    void updateHeightmapGrid(double arg1);
    void resizeTableHeightmapSections();
    bool eventFilter(QObject *obj, QEvent *event) override;
    void updateCurrentModel(GCodeTableModel *m_currentModel);
    void updateToolPositionAndToolpathShadowing(QVector3D toolPosition);
    void updateToolpathShadowingOnCheckMode();
    QString lastWorkingDirectory();

    // Utility
    int bufferLength();
    QTime updateProgramEstimatedTime(QList<LineSegment *> lines);
    QList<LineSegment *> subdivideSegment(LineSegment *segment);
    void jogStep();
    void jogContinuous();
    // int buttonSize();
    QString getLineInitCommands(int row);

    static bool actionLessThan(const QAction *a1, const QAction *a2);
    static bool actionTextLessThan(const QAction *a1, const QAction *a2);

    void initializeConnection(ConnectionMode mode);
    void initializeVisualizer();

    void applySpindleConfiguration(ConfigurationMachine &machineConfiguration);
    void applyRecentFilesConfiguration(ConfigurationUI &uiConfiguration);
    void applyHeightmapConfiguration(ConfigurationHeightmap &heightmapConfiguration);
    void applyOverridesConfiguration(ConfigurationMachine &machineConfiguration);
    void applyVisualizerConfiguration(ConfigurationVisualizer &visualizerConfiguration);
    void applyCodeDrawerConfiguration(ConfigurationVisualizer &visualizerConfiguration);
    void applyToolDrawerConfiguration(ConfigurationVisualizer &visualizerConfiguration);
    void applyTableSurfaceDrawerConfiguration(ConfigurationVisualizer &visualizerConfiguration);
    void applyHeightmapDrawerConfiguration(ConfigurationVisualizer &visualizerConfiguration);
    void applyUIConfiguration(ConfigurationUI &uiConfiguration);
    void applyJoggingConfiguration(ConfigurationJogging &joggingConfiguration);
};

typedef QMap<QString, QList<QKeySequence>> ShortcutsMap;

#endif // FRMMAIN_H
