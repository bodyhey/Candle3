// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef STREAMER_H
#define STREAMER_H

#include <QObject>

class Streamer : public QObject
{
    Q_OBJECT

    public:
        explicit Streamer(QObject *parent = nullptr);
        void reset(int commandIndex = 0);
        void resetProcessed(int commandIndex = 0);
        int commandIndex() { return m_commandIndex; }
        int processedCommandIndex() { return m_processedCommandIndex; }
        void advanceCommandIndex();

    private:
        int m_commandIndex;
        int m_processedCommandIndex;

    signals:
        void progressChanged(int progress);
};

#endif // STREAMER_H
