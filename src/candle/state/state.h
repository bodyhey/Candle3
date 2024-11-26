// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef STATE_H
#define STATE_H

#include <QObject>
#include "../globals.h"
#include "../connection/connection.h"

class Communicator;

class State : public QObject
{
        Q_OBJECT
    public:
        explicit State(State *previous, QObject *parent = nullptr);
        virtual QString name() = 0;
        virtual bool isJoggingAllowed() { return false; };
        virtual bool isHomingAllowed() { return false; };
        State* previous() const { return m_previous; }
        virtual void onEntry(Communicator *communicator, State *previous = nullptr);
        virtual void onExit() {};
        virtual void onDeviceStateChanged(DeviceState state) {};

    signals:
        void transition(State &state);

    public slots:
        virtual void onConnectionStateChanged(ConnectionState state) {};

    protected:
        State *m_previous;
        Communicator *m_communicator;
};

#endif // STATE_H
