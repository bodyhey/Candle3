// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATION_CONNECTION_H
#define CONFIGURATION_CONNECTION_H

#include <QObject>
#include <QVariant>
#include "../../globals.h"
#include "configurationmodule.h"

class ConfigurationConnection : public ConfigurationModule
{
    friend class frmSettings;

    Q_OBJECT
    Q_PROPERTY(QString serialPort MEMBER m_serialPort NOTIFY changed)
    Q_PROPERTY(QString rawTcpHost MEMBER m_rawTcpHost NOTIFY changed)
    Q_PROPERTY(int rawTcpPort MEMBER m_rawTcpPort NOTIFY changed)
    Q_PROPERTY(int queryStateInterval MEMBER m_queryStateInterval NOTIFY changed)
    Q_PROPERTY(ConnectionMode connectionMode MEMBER m_connectionMode NOTIFY changed)
    Q_PROPERTY(int serialBaud MEMBER m_serialBaud NOTIFY changed)

    public:
        explicit ConfigurationConnection(QObject *parent);
        ConfigurationConnection& operator=(const ConfigurationConnection&) { return *this; }
        QVariant customGet(QString) override;
        void customSet(QString, QVariant) override;
        QString getSectionName() override { return "connection"; }

        ConnectionMode connectionMode() const { return m_connectionMode; }
        int queryStateInterval() const { return m_queryStateInterval; }
        QString serialPort() const { return m_serialPort; }
        int serialBaud() const { return m_serialBaud; }
        QString rawTcpHost() const { return m_rawTcpHost; }
        int rawTcpPort() const { return m_rawTcpPort; }

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
};

Q_DECLARE_METATYPE(ConnectionMode);

#endif // CONFIGURATION_CONNECTION_H
