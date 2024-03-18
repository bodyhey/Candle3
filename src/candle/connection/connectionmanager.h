// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include "connection.h"

class ConnectionManager : public QObject
{
    Q_OBJECT

    public:
        ConnectionManager(QObject *parent = nullptr);
        //~ConnectionManager();

        Connection* getConnection();

    private:
        Connection* initializeSerialConnection();
        Connection* initializeVirtualConnection();
        Connection* initializeRawTcpConnection();
};

#endif // CONNECTIONMANAGER_H
