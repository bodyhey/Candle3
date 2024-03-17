// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationsender.h"

const QMap<QString, QVariant> DEFAULTS = {
    {"test", "ahugu"},
    {"test2", 421412111},
};

ConfigurationSender::ConfigurationSender(QObject *parent) : ConfigurationModule(parent, DEFAULTS)
{
}

QVariant ConfigurationSender::customGet(QString)
{

}

void ConfigurationSender::customSet(QString, QVariant)
{

}
