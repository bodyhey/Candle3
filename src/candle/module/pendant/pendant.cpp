// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "pendant.h"

#include <QTcpSocket>
//#include <CircularBuffer.hpp>
#include <QTimer>
#include <CRC.h>

enum class Axis {
    None = -1,
    X = 0,
    Y,
    Z,
};

enum class PacketType: uint8_t {
    STATE = 0,
    WIFI_CONFIG = 1,
    PING = 2,
};

enum class CommunicationMode {
    NONE = 0,
    SERIAL_,
    WIFI,
};

struct __attribute__ ((packed)) Header
{
    uint16_t start; // 0xAA55
    uint8_t size;
    uint8_t type;
};

struct __attribute__ ((packed)) Footer
{
    uint8_t crc;
};

struct __attribute__ ((packed)) StateMessage
{
    Header header;
    float x;
    float y;
    float z;
    CommunicationMode mode;
    char selectedAxis;
    Footer footer;
};

struct __attribute__ ((packed)) WifiConfigMessage
{
    Header header;
    char ssid[20];
    char password[20];
    char clientIp[16];
    Footer footer;
};

class Queue {
    private:
    //CircularBuffer<char, 256> buffer;
};

Pendant::Pendant(QObject *parent) : QObject{parent}
{
    qDebug() << "Pendant created";

    this->server = new QTcpServer(this);
    this->server->listen(QHostAddress::Any, 5555);

    connect(this->server, &QTcpServer::newConnection, [this]() {
        qDebug() << "New pendant connection";

        QTimer *timer = new QTimer(this);
        QTcpSocket *socket = this->server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, [socket, this]() {
            QByteArray data = socket->readAll();
            qDebug() << "Received data: " << data;
            socket->write(data);
        });
        connect(socket, &QTcpSocket::disconnected, [socket, timer]() {
            timer->stop();
            timer->deleteLater();
            socket->deleteLater();
            qDebug() << "Pendant disconnected";
        });

        timer->setInterval(50);
        connect(timer, &QTimer::timeout, [socket]() {
            //qDebug() << "Sending state message";
            StateMessage message;
            message.header.start = 0xAA55;
            message.header.size = sizeof(StateMessage);
            message.header.type = static_cast<uint8_t>(PacketType::STATE);
            message.x = 1.0f;
            message.y = ((float) (rand() % 1000)) / 100.0f;
            message.z = rand() % 100;
            message.footer.crc = calcCRC8((uint8_t*)&message, sizeof(StateMessage) - sizeof(Footer));

            socket->write((char*)&message, sizeof(StateMessage));

            // WifiConfigMessage wifiMessage;
            // wifiMessage.header.start = 0xAA55;
            // wifiMessage.header.size = sizeof(WifiConfigMessage);
            // wifiMessage.header.type = static_cast<uint8_t>(PacketType::WIFI_CONFIG);
            // strcpy(wifiMessage.ssid, "ssid");
            // strcpy(wifiMessage.password, "password");
            // strcpy(wifiMessage.clientIp, "127.0.0.1");
            // wifiMessage.footer.crc = calcCRC8((uint8_t*)&wifiMessage, sizeof(WifiConfigMessage) - sizeof(Footer));

            // socket->write((char*)&wifiMessage, sizeof(WifiConfigMessage));
        });
        timer->start();

        connect(socket, &QTcpSocket::disconnected, [socket]() {
            socket->deleteLater();
        });
    });
}
