// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "pendant.h"

#include <QTcpSocket>
#include <CircularBuffer.hpp>
#include <QTimer>

enum class Axis {
    X = 0,
    Y,
    Z,
};

enum class PacketType: uint8_t {
    STATE = 0,
};

enum class CommunicationMode {
    NONE = 0,
    SERIAL_,
    WIFI,
};

struct __attribute__ ((packed)) HeaderMessage
{
    uint16_t start; // 0xAA55
    uint8_t size;
    uint8_t type;
};

struct __attribute__ ((packed)) StateMessage
{
    HeaderMessage header;
    float x;
    float y;
    float z;
    CommunicationMode mode;
};

class Queue {
    private:
    CircularBuffer<char, 1024> buffer;
};

Pendant::Pendant(QObject *parent) : QObject{parent}
{
    qDebug() << "Pendant created";

    this->server = new QTcpServer(this);
    this->server->listen(QHostAddress::Any, 5555);

    connect(this->server, &QTcpServer::newConnection, [this]() {
        qDebug() << "New pendant connection";

        QTcpSocket *socket = this->server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, [socket, this]() {
            QByteArray data = socket->readAll();
            qDebug() << "Received data: " << data;
            socket->write(data);
        });

        QTimer *timer = new QTimer(this);
        timer->setInterval(1000);
        connect(timer, &QTimer::timeout, [socket]() {
            qDebug() << "Sending state message";
            StateMessage message;
            message.header.start = 0xAA55;
            message.header.size = sizeof(StateMessage);
            message.header.type = static_cast<uint8_t>(PacketType::STATE);
            message.x = 1.0f;
            message.y = 2.0f;
            message.z = 3.0f;

            QByteArray data(reinterpret_cast<const char*>(&message), sizeof(StateMessage));
            socket->write(data);
        });
        timer->start();

        connect(socket, &QTcpSocket::disconnected, [socket]() {
            socket->deleteLater();
        });
    });
}
