// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationvisualizer.h"

const QMap<QString, QVariant> DEFAULTS = {
    {"test", "ahugu"},
    {"test2", 421412111},
};

ConfigurationVisualizer::ConfigurationVisualizer(QObject *parent) : ConfigurationModule(parent, DEFAULTS)
{
    
}

// QVariant ConfigurationVisualizer::customGet(QString)
// {

// }

// void ConfigurationVisualizer::customSet(QString, QVariant)
// {

// }
