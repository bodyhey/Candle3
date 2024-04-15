// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationmachine.h"

const QMap<QString,QVariant> DEFAULTS = {
    {"spindleSpeedRange", QVariantMap({{"min", 0}, {"max", 100}})},
    {"laserPowerRange", QVariantMap({{"min", 0}, {"max", 100}})},
    {"referencePositionDirX", ConfigurationMachine::ReferencePositionDir::Positive},
    {"referencePositionDirY", ConfigurationMachine::ReferencePositionDir::Positive},
    {"referencePositionDirZ", ConfigurationMachine::ReferencePositionDir::Positive}
};

ConfigurationMachine::ConfigurationMachine(QObject *parent) : ConfigurationModule(parent, DEFAULTS)
{
    ConfigRegistry::registerEnum("ConfigurationMachine::ReferencePositionDir");
    ConfigRegistry::registerEnum("ConnectionMode");
}
