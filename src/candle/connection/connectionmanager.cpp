// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "connectionmanager.h"
#include "../globals.h"

ConnectionManager::ConnectionManager(QObject *parent) : QObject(parent)
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
    serialConnection->setPortName("COM3");
    serialConnection->setBaudRate(115200);

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
    rawTcpConnection->setHost("");
    rawTcpConnection->setPort(0);

    return rawTcpConnection;
}
