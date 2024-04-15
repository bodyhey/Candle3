// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationconsole.h"

const QMap<QString, QVariant> DEFAULTS = {
    {"showProgramCommands", false},
    {"showUiCommands", true},
    {"commandAutoCompletion", true},
    {"darkBackgroundMode", false},
};

ConfigurationConsole::ConfigurationConsole(QObject *parent) : ConfigurationModule(parent, DEFAULTS)
{
    m_commandHistory << "G0 X10 Y10 Z10"
                     << "G1 X20 Y20 Z20";
}
