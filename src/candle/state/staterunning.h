// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef STATERUNNING_H
#define STATERUNNING_H

#include "state.h"

class StateRunning : public State
{
    public:
        explicit StateRunning(State *previous, QObject *parent = nullptr);
};

#endif // STATERUNNING_H
