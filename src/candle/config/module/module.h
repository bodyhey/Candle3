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
        virtual QVariant customGet(QString) { return QVariant(); };
        virtual void customSet(QString, QVariant) {};
        virtual QString getSectionName() = 0;

        struct MinMax {
            int min;
            int max;
        };
        struct MinMaxFloat {
            float min;
            float max;
        };

    private:
        QMap<QString, QVariant> m_defaults;

    signals:
        void changed();
};

#endif // CONFIGURATION_MODULE_H
