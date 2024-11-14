// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef STATEERROR_H
#define STATEERROR_H

#include "state.h"

class StateError : public State
{
    public:
        explicit StateError(QObject *parent = nullptr);
};

#endif // STATEERROR_H
