// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include "config/module/configurationconnection.h"

class Connection : public QObject
{
    Q_OBJECT

    public:
        Connection(QObject *parent);
        virtual ~Connection() {}

        // true = waiting for connection, false = already connected or failed to connect
        virtual bool openConnection() = 0;
        virtual void sendChar(QChar);
        virtual void sendChar(char);
        virtual void sendByteArray(QByteArray) = 0;
        virtual bool isConnected() = 0;
        virtual bool isConnecting() { return m_connecting; }
        virtual void sendLine(QString) = 0;
        virtual void closeConnection() = 0;
        virtual ConfigurationConnection::ConnectionMode getSupportedMode() = 0;

    protected:
        bool m_connecting = false;

    signals:
        void lineReceived(const QString &line);
        void connected();
        void error(const QString &text);
};

#endif
