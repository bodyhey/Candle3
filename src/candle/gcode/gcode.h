// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef GCODE_H
#define GCODE_H

#include <QObject>
#include "tables/gcodetablemodel.h"

enum StreamerStartResult
{
    Success = 0,
    UnacceptableCommunicatorState = 1,
    UnacceptableConnectionState = 2,
};

struct GCodeLine
{
        QString command;
        QString comment;
        QString response;
        int lineNumber;
        int state;
        QStringList args;

        enum States { InQueue, Sent, Processed, Skipped };
};

class GCode : public QObject , public QList<GCodeLine>
{
    Q_OBJECT

    public:
        explicit GCode(QObject *parent = nullptr);
        void reset(int commandIndex = 0);
        void resetProcessed(int commandIndex = 0);
        int commandIndex() { return m_commandIndex; }
        QString command();
        int processedCommandIndex() { return m_processedCommandIndex; }
        void advanceCommandIndex();
        StreamerStartResult start();
        void stop();
        void pause();        
        bool isLastCommand();
        bool noMoreCommands();
        bool hasMoreCommands();
        bool isLastCommandProcessed();
        // @TODO remove this method??
        void setModel(GCodeTableModel *model) {
            m_currentModel = model;
            m_commandsCount = model->rowCount() - 1;
            reset();
        }
        void commandSent();

    private:
        int m_commandIndex;
        int m_processedCommandIndex;
        int m_commandsCount;
        GCodeTableModel *m_currentModel;

    signals:
        void progressChanged(int progress);
        void finished();
        void paused();
        void error();
};

#endif // GCODE_H
