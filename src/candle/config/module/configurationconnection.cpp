// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationconnection.h"

ConfigurationConnection::ConfigurationConnection(QObject *parent) : QObject(parent)
{
}

void ConfigurationConnection::load()
{
    
}

void ConfigurationConnection::save()
{
    
}

void ConfigurationConnection::setSerialPort(QString port)
{
    m_SerialPort = port;
}

void ConfigurationConnection::setConnectionMode(ConnectionMode connectionMode)
{
    m_connectionMode = connectionMode;
}

void ConfigurationConnection::setSerialBaud(int baud)
{
    m_SerialBaud = baud;
}

QString ConfigurationConnection::serialPort()
{
    return m_SerialPort;
}

ConnectionMode ConfigurationConnection::connectionMode()
{
    return m_connectionMode;
}

int ConfigurationConnection::serialBaud()
{
    return m_SerialBaud;
}
