// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "stateinitialization.h"

StateInitialization::StateInitialization(State *previous, QObject *parent)
    : State{previous, parent}
{}

void StateInitialization::onDeviceStateChanged(DeviceState state)
{

}
