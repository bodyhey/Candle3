// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATION_SENDER_H
#define CONFIGURATION_SENDER_H

#include <QObject>
#include "module.h"

class ConfigurationSender : public QObject, ConfigurationModule
{
    Q_OBJECT

    public:
        explicit ConfigurationSender(QObject *parent = nullptr);
        ConfigurationSender& operator=(const ConfigurationSender&) { return *this; }
        void load() override;
        void save() override;
};

#endif // CONFIGURATION_SENDER_H
