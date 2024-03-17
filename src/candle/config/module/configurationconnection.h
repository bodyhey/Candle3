// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATION_CONNECTION_H
#define CONFIGURATION_CONNECTION_H

#include <QObject>
#include <QVariant>
#include "../../globals.h"
#include "module.h"

class ConfigurationConnection : public ConfigurationModule
{    
    Q_OBJECT
    Q_PROPERTY(QString serialPort MEMBER m_serialPort NOTIFY changed)
    Q_PROPERTY(QString rawTcpHost MEMBER m_rawTcpHost NOTIFY changed)
    Q_PROPERTY(int rawTcpPort MEMBER m_rawTcpPort NOTIFY changed)
    Q_PROPERTY(int queryStateInterval MEMBER m_queryStateInterval NOTIFY changed)
    Q_PROPERTY(ConnectionMode connectionMode MEMBER m_connectionMode NOTIFY changed)
    Q_PROPERTY(int serialBaud MEMBER m_serialBaud NOTIFY changed)

    public:
        ConfigurationConnection(QObject *parent);
        ConfigurationConnection& operator=(const ConfigurationConnection&) { return *this; }

        QVariant customGet(QString) override;
        void customSet(QString, QVariant) override;
        QString getName() override { return "connection"; }

    private:       
        // General

        ConnectionMode m_connectionMode;
        int m_queryStateInterval;

        // Serial

        QString m_serialPort;
        int m_serialBaud;

        // Raw TCP

        QString m_rawTcpHost;
        int m_rawTcpPort;

        // Virtual uCNC

    signals:
        void changed();
};

#endif // CONFIGURATION_CONNECTION_H
