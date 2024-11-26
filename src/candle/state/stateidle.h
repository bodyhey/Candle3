// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef STATEIDLE_H
#define STATEIDLE_H

#include "state.h"

class StateIdle : public State
{    
    public:
        explicit StateIdle(State *previous, QObject *parent = nullptr);
        QString name() override { return "Idle"; }
        bool isJoggingAllowed() override { return false; }
        bool isHomingAllowed() override { return false; }
};

#endif // STATEIDLE_H
