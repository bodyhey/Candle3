// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include "../globals.h"

class Connection : public QObject
{
    Q_OBJECT

    public:
        Connection(QObject *parent = nullptr);
        virtual ~Connection() {}

        virtual bool openConnection() = 0;
        //virtual void sendChar(QChar) = 0;
        virtual void sendByteArray(QByteArray) = 0;
        virtual bool isConnected() = 0;
        virtual void sendLine(QString) = 0;
        virtual void closeConnection() = 0;
        virtual ConnectionMode getSupportedMode() = 0;

    signals:
        void lineReceived(const QString &line);
        void error(const QString &text);
};

#include "serialconnection.h"
#include "rawtcpconnection.h"
#include "virtualucncconnection.h"

#endif
