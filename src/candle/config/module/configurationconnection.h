// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATION_CONNECTION_H
#define CONFIGURATION_CONNECTION_H

#include <QObject>
#include "../../globals.h"
#include "module.h"

class ConfigurationConnection : public QObject, ConfigurationModule
{
    Q_OBJECT
    Q_PROPERTY(QString port READ serialPort WRITE setSerialPort)
    Q_PROPERTY(ConnectionMode connectionMode READ connectionMode WRITE setConnectionMode)
    Q_PROPERTY(int baud READ serialBaud WRITE setSerialBaud)

    public:
        ConfigurationConnection(QObject *parent);
        ConfigurationConnection& operator=(const ConfigurationConnection&) { return *this; }
        void load() override;
        void save() override;

    private:
        QString m_SerialPort;
        ConnectionMode m_connectionMode;
        int m_SerialBaud;
        void setSerialPort(QString);
        void setConnectionMode(ConnectionMode);
        void setSerialBaud(int);
        QString serialPort();
        ConnectionMode connectionMode();
        int serialBaud();
};

#endif // CONFIGURATION_CONNECTION_H
