#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <QScrollBar>
#include "frmsettings.h"
#include "globals.h"
#include "streamer.h"
#include "connection/connection.h"
#include "machine/machineconfiguration.h"
#include "scripting/scriptvars.h"

#ifdef WINDOWS
    #include <QtWinExtras/QtWinExtras>
#endif

namespace Ui {
class frmMain;
}

class Communicator : public QObject
{
    friend class frmMain;

    Q_OBJECT

    public:
        Communicator(
            QObject *parent,
            Connection *connection,
            Configuration *configuration
        );
        SendCommandResult sendCommand(CommandSource source, QString command, int tableIndex = -1, bool wait = false);
        void sendRealtimeCommand(QString command);
        void sendRealtimeCommand(int command);
        void sendCommands(CommandSource source, QString commands, int tableIndex = -1);
        bool streamCommands(Streamer *streamer);
        void clearCommandsAndQueue();
        void clearQueue();
        void reset();
        // @TODO abort what?? find more self descriptive name, move to streamer??
        void abort();
        // disconnect, dispose and delete old connection, connect new connection
        void replaceConnection(Connection *);
        void stopUpdatingState();
        void startUpdatingState(int interval = -1);
        SenderState senderState() const { return m_senderState; }
        DeviceState deviceState() const { return m_deviceState; }
        void processWorkOffset(QString data);
        void sendStreamerCommandsUntilBufferIsFull();

        // @TODO to be removed!! another local timer? how it works??
        void processConnectionTimer();

    private:
        static const int BUFFERLENGTH = 127;

        Connection *m_connection;
        frmSettings *m_settings;
        Configuration *m_configuration;
        Communicator *m_communicator;
        Streamer *m_streamer = nullptr;

        // Queues
        QList<CommandAttributes> m_commands;
        QList<CommandQueue> m_queue;

        // States
        SenderState m_senderState;
        DeviceState m_deviceState;

        ScriptVars m_storedVars;

        QTime m_startTime;

        // Coordinates
        QVector3D m_machinePos;
        QVector3D m_workPos;

        // Flags
        bool m_reseting;
        bool m_resetCompleted;
        bool m_aborting;
        bool m_statusReceived;
        bool m_homing;
        bool m_spindleCW; // Spindle is rotating clockwise
        bool m_updateSpindleSpeed;
        bool m_updateParserState;

        // Indices
        int m_probeIndex;
        int m_commandIndex = 0;

        // Stored parser params
        QString m_lastParserState; // response to $G
        QString m_storedParserState; // saved by storeParserState

        // Timers
        QTimer m_timerStateQuery;

        // Dictionary
        QMap<DeviceState, QString> m_deviceStatesDictionary;

        void setSenderStateAndEmitSignal(SenderState);
        void setDeviceStateAndEmitSignal(DeviceState);
        void restoreOffsets();
        int bufferLength();
        void processOffsetsVars(QString response);
        bool dataIsFloating(QString data);
        bool dataIsEnd(QString data);
        bool dataIsReset(QString data);
        bool compareCoordinates(double x, double y, double z);
        double toMetric(double value);
        double toInches(double value);

        void processStatus(QString data);
        void processCommandResponse(QString data);
        void processUnhandledResponse(QString data);
        void processMessage(QString data);
        void processFeedSpindleSpeed(QString data);
        void processOverrides(QString data);
        void processNewToolPosition();
        void storeParserState();
        void restoreParserState();
        void completeTransfer();
    private slots:
        void onTimerStateQuery();
        void onConnectionLineReceived(QString);
        void onConnectionError(QString);

    signals:
        void responseReceived(QString command, int tableIndex, QString response);
        void statusReceived(QString status);
        void senderStateReceived(SenderState state);
        void senderStateChanged(SenderState state);
        void deviceStateChanged(DeviceState state);
        void configurationReceived(MachineConfiguration configuration, QMap<int, double> rawConfiguration);
        void machinePosChanged(QVector3D pos);
        void workPosChanged(QVector3D pos);
        // emitted after status response received, if state changed, may be emitted together with deviceStateChanged!
        void deviceStateReceived(DeviceState state);
        void spindleStateReceived(bool state);
        void pinStateReceived(QString state);
        void parserStateReceived(QString state);
        void toolPositionReceived(QVector3D pos);
        void floodStateReceived(bool state);
        void commandResponseReceived(CommandAttributes commandAttributes);
        void commandSent(CommandAttributes commandAttributes);
        // @TODO what is the difference between spindleSpeedReceived and feedSpindleSpeedReceived??
        void spindleSpeedReceived(int spindleSpeed);
        void feedSpindleSpeedReceived(int feedRate, int spindleSpeed);
        void overridesReceived(int feedOverride, int spindleOverride, int rapidOverride);
        // @TODO how to do it better?? what signal should we attach to?
        void commandProcessed(int tableIndex, QString response);
        // @TODO aborted what?? find better name
        void aborted();
        void transferCompleted();
};

#endif // COMMUNICATOR_H
