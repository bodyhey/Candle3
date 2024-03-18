// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef RAWTCPCONNECTION_H
#define RAWTCPCONNECTION_H

#include <QObject>
#include "connection.h"

class RawTcpConnection : public Connection
{
    Q_OBJECT

    public:
        RawTcpConnection(QObject*);
        ~RawTcpConnection();
        bool openConnection() override;
        void setHost(QString);
        void setPort(int);
        void sendByteArray(QByteArray) override;
        bool isConnected() override;
        void sendLine(QString) override;
        void closeConnection() override;
        ConnectionMode getSupportedMode() override { return ConnectionMode::RAW_TCP; };

    // private slots:

};

#endif // RAWTCPCONNECTION_H
