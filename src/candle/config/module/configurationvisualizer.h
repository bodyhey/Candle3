// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATION_VISUALIZER_H
#define CONFIGURATION_VISUALIZER_H

#include <QObject>
#include "module.h"

class ConfigurationVisualizer : public ConfigurationModule
{
    Q_OBJECT;

    public:
        ConfigurationVisualizer(QObject *parent);
        ConfigurationVisualizer& operator=(const ConfigurationVisualizer&) { return *this; }
        QVariant customGet(QString) override;
        void customSet(QString, QVariant) override;
        QString getName() override { return "visualizer"; }

};

#endif // CONFIGURATION_VISUALIZER_H
