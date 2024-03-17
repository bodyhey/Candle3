// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATION_VISUALIZER_H
#define CONFIGURATION_VISUALIZER_H

#include <QObject>
#include "module.h"

class ConfigurationVisualizer : public QObject, ConfigurationModule
{
    Q_OBJECT;

    public:
        ConfigurationVisualizer(QObject *parent);
        ConfigurationVisualizer& operator=(const ConfigurationVisualizer&) { return *this; }
        void load() override;
        void save() override;
};

#endif // CONFIGURATION_VISUALIZER_H
