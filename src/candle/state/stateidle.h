// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef STATEIDLE_H
#define STATEIDLE_H

#include "state.h"

class StateIdle : public State
{
    public:
        explicit StateIdle(QObject *parent = nullptr);
};

#endif // STATEIDLE_H
