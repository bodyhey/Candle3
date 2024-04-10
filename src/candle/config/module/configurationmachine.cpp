// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationmachine.h"

const QMap<QString,QVariant> DEFAULTS = {};

ConfigurationMachine::ConfigurationMachine(QObject *parent) : ConfigurationModule(parent, DEFAULTS) {
}
