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
#include <QScriptEngine>
#include <QGroupBox>
#include <exception>

#include "connection/connection.h"
#include "communicator.h"
#include "streamer.h"
#include "globals.h"

#include "connection/connection.h"
#include "form_partial/main/jog.h"
#include "form_partial/main/state.h"
#include "form_partial/main/console.h"
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

#include "scripting/scripting.h"
#include "tables/gcodetablemodel.h"
#include "tables/heightmaptablemodel.h"

#include "utils/interpolation.h"

#include "styledtoolbutton.h"
#include "sliderbox.h"

#include "frmsettings.h"
#include "frmabout.h"

#include "scripting/scriptvars.h"
#include "scripting/scriptfunctions.h"

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

class frmMain : public QMainWindow, TempConnectionToUiProxy
{
    Q_OBJECT

    friend class ScriptFunctions;

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();
    
    void writeConsole(QString command);

    // TO BE REMOVED !!!
    MachineBoundsDrawer &machineBoundsDrawer() override { return m_machineBoundsDrawer; };
    ScriptVars &storedVars() override { return m_storedVars; };
    GCodeTableModel &programModel() override { return m_programModel; };
    GCodeTableModel &probeModel() override { return m_probeModel; };
    GCodeTableModel &programHeightmapModel() override { return m_programHeightmapModel; };
    GCodeTableModel &currentModel() override { return *m_currentModel; };
    HeightMapTableModel &heightMapModel() override { return m_heightMapModel; };
    bool &heightMapMode() override { return m_heightMapMode; };
    bool &absoluteCoordinates() override { return m_absoluteCoordinates; };
    partMainConsole &partConsole() override { return *m_partConsole; };
    QMessageBox &senderErrorBox() override { return *m_senderErrorBox; };
    // TO BE REMOVED !!!

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
    void settingsSetByDefault();
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
    void on_actJogStepNext_triggered();
    void on_actJogStepPrevious_triggered();
    void on_actJogFeedNext_triggered();
    void on_actJogFeedPrevious_triggered();
    void on_actSpindleSpeedPlus_triggered();
    void on_actSpindleSpeedMinus_triggered();
    void on_actOverrideFeedPlus_triggered();
    void on_actOverrideFeedMinus_triggered();
    void on_actOverrideRapidPlus_triggered();
    void on_actOverrideRapidMinus_triggered();
    void on_actOverrideSpindlePlus_triggered();
    void on_actOverrideSpindleMinus_triggered();
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
    void on_cmdYPlus_pressed();
    void on_cmdYPlus_released();
    void on_cmdYMinus_pressed();
    void on_cmdYMinus_released();
    void on_cmdXPlus_pressed();
    void on_cmdXPlus_released();
    void on_cmdXMinus_pressed();
    void on_cmdXMinus_released();
    void on_cmdZPlus_pressed();
    void on_cmdZPlus_released();
    void on_cmdZMinus_pressed();
    void on_cmdZMinus_released();
    void on_cmdStop_clicked();
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
    void onAborted();
    void onResponseReceived(QString command, int tableIndex, QString response);
    void onCommandResponseReceived(CommandAttributes commandAttributes);
    void onCommandSent(CommandAttributes commandAttributes);
    void onConfigurationReceived(MachineConfiguration, QMap<int, double>);
    void onToolPositionReceived(QVector3D pos);
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
    void onScriptException(const QScriptValue &exception);

    void onToolPos(QPointF);

    void updateHeightMapInterpolationDrawer(bool reset = false);
    void placeVisualizerButtons();

protected:
    void showEvent(QShowEvent *se);
    void hideEvent(QHideEvent *he);
    void resizeEvent(QResizeEvent *re);
    void timerEvent(QTimerEvent *);
    void closeEvent(QCloseEvent *ce);
    void dragEnterEvent(QDragEnterEvent *dee);
    void dropEvent(QDropEvent *de);
    QMenu *createPopupMenu() override;

private:
    static const int BUFFERLENGTH = 127;
    static const int PROGRESSMINLINES = 10000;
    static const int PROGRESSSTEP = 1000;    

    // Ui
    Ui::frmMain *ui;

    //QMap<DeviceState, QString> m_deviceStatuses;
    QMap<DeviceState, QString> m_statusCaptions;
    QMap<DeviceState, QString> m_statusBackColors;
    QMap<DeviceState, QString> m_statusForeColors;

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
    OriginDrawer *m_originDrawer;
    GcodeDrawer *m_codeDrawer;    
    GcodeDrawer *m_probeDrawer;
    GcodeDrawer *m_currentDrawer;
    ToolDrawer m_toolDrawer;
    HeightMapBorderDrawer m_heightMapBorderDrawer;
    HeightMapGridDrawer m_heightMapGridDrawer;
    HeightMapInterpolationDrawer m_heightMapInterpolationDrawer;
    SelectionDrawer m_selectionDrawer;
    MachineBoundsDrawer m_machineBoundsDrawer;

    // Table models
    GCodeTableModel m_programModel;
    GCodeTableModel m_probeModel;
    GCodeTableModel m_programHeightmapModel;
    GCodeTableModel *m_currentModel;
    HeightMapTableModel m_heightMapModel;

    // @TODO to be moved to separate core class
    Connection *m_connection;
    Communicator *m_communicator;
    Streamer *m_streamer;

    // Forms
    frmSettings *m_settings;
    frmAbout m_frmAbout;
    frmGrblConfigurator *m_grblConfigurator;

    // Partials
    partMainJog *m_partJog;
    partMainState *m_partState;
    partMainConsole *m_partConsole;

    // Filenames
    QString m_settingsFileName;
    QString m_programFileName;
    QString m_heightMapFileName;
    QString m_lastFolder;
    QStringList m_recentFiles;
    QStringList m_recentHeightmaps;

    // Timers
    QTimer m_timerConnection;
    QBasicTimer m_timerToolAnimation;
    QTime m_startTime;

    // Flags
    bool m_programLoading;
    bool m_settingsLoading;
    bool m_fileChanged;
    bool m_heightMapChanged;

    // bool m_updateSpindleSpeed;
    // bool m_updateParserStatus;

    bool m_heightMapMode;

    // Current values
    int m_lastDrawnLineIndex;

    // Keyboard
    bool m_absoluteCoordinates;
    bool m_storedKeyboardControl;

    // Jog
    QVector3D m_jogVector;

    // Script
    Configuration m_configuration;
    Scripting m_scripting;
    QScriptEngine m_scriptEngine;
    ScriptVars m_storedVars;
    ScriptFunctions m_scriptFunctions;

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
    void grblReset() override;
    //void sendNextFileCommands();
    QString evaluateCommand(QString command);

    // Parser
    void updateParser();

    // Files/models
    void loadFile(QString fileName);
    void loadFile(QList<std::string> data);
    bool saveChanges(bool heightMapMode);
    bool saveProgramToFile(QString fileName, GCodeTableModel *model);
    void loadHeightMap(QString fileName);
    bool saveHeightMap(QString fileName);
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
    void updateHeightMapBorderDrawer();
    bool updateHeightMapGrid();
    void updateHeightMapGrid(double arg1);
    void resizeTableHeightMapSections();
    bool eventFilter(QObject *obj, QEvent *event) override;

    void updateToolPositionAndToolpathShadowing(QVector3D toolPosition);
    void updateToolpathShadowingOnCheckMode();

    // Utility
    int bufferLength();
    QTime updateProgramEstimatedTime(QList<LineSegment *> lines);
    QList<LineSegment *> subdivideSegment(LineSegment *segment);
    void jogStep();
    void jogContinuous();
    bool isGCodeFile(QString fileName);
    bool isHeightmapFile(QString fileName);
    int buttonSize();
    void completeTransfer();
    QString getLineInitCommands(int row);

    static bool actionLessThan(const QAction *a1, const QAction *a2);
    static bool actionTextLessThan(const QAction *a1, const QAction *a2);
    static QScriptValue importExtension(QScriptContext *context, QScriptEngine *engine);

    void initializeConnection(ConnectionMode mode);
};

typedef QMap<QString, QList<QKeySequence>> ShortcutsMap;

#endif // FRMMAIN_H
