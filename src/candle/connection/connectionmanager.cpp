// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "connectionmanager.h"
#include "../globals.h"

ConnectionManager::ConnectionManager(QObject *parent, const ConfigurationConnection &configurationConnection)
    : QObject(parent)
    , m_configurationConnection(configurationConnection)
{
}

Connection *ConnectionManager::getConnection()
{
    ConnectionMode mode = ConnectionMode::SERIAL;

    switch (mode) {
        case ConnectionMode::SERIAL:
            return initializeSerialConnection();
        case ConnectionMode::VIRTUAL:
            return initializeVirtualConnection();
        case ConnectionMode::RAW_TCP:
            return initializeRawTcpConnection();
    }
}

Connection *ConnectionManager::initializeSerialConnection()
{
    SerialConnection* serialConnection = new SerialConnection(this);
    serialConnection->setPortName(m_configurationConnection.serialPort());
    serialConnection->setBaudRate(m_configurationConnection.serialBaud());

    return serialConnection;
}

Connection *ConnectionManager::initializeVirtualConnection()
{
    VirtualUCNCConnection* virtualConnection = new VirtualUCNCConnection(this);

    return virtualConnection;
}

Connection *ConnectionManager::initializeRawTcpConnection()
{
    RawTcpConnection* rawTcpConnection = new RawTcpConnection(this);
    rawTcpConnection->setHost(m_configurationConnection.rawTcpHost());
    rawTcpConnection->setPort(m_configurationConnection.rawTcpPort());

    return rawTcpConnection;
}
