// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef STATE_H
#define STATE_H

#include <QObject>
#include "../globals.h"
#include "../connection/connection.h"

class State : public QObject
{
        Q_OBJECT
    public:
        explicit State(QObject *parent = nullptr);
        virtual QString name() = 0;
        virtual bool isJoggingAllowed() = 0;
        virtual bool isHomingAllowed() = 0;

    signals:
        void transition(State &state);

    public slots:
        virtual void onDeviceStateReceived(DeviceState state) {};
        virtual void onConnectionStateChanged(ConnectionState state) {};
};

#endif // STATE_H
