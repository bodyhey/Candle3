// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATION_MODULE_H
#define CONFIGURATION_MODULE_H

#include <QObject>
#include <QVariant>

class ConfigurationModule : public QObject
{
    Q_OBJECT

    public:
        ConfigurationModule(QObject *parent, QMap<QString, QVariant> defaults) : QObject(parent), m_defaults(defaults) {}

        QMap<QString, QVariant> getDefaults() { return m_defaults; }
        virtual QVariant customGet(QString) = 0;
        virtual void customSet(QString, QVariant) = 0;
        virtual QString getName() = 0;

    private:
        QMap<QString, QVariant> m_defaults;

};

#endif // CONFIGURATION_MODULE_H
