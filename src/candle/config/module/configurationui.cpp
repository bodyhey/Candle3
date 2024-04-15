// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationui.h"

const QMap<QString,QVariant> DEFAULTS = {
    {"fontSize", 12},
    {"language", "en"}    
};

ConfigurationUI::ConfigurationUI(QObject *parent) : ConfigurationModule(parent, DEFAULTS)
{
    m_recentFiles << "ab,,,,,c.txt"
                  << "text.gc\node";
}
