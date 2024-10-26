// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "virtualucncconnection.h"
#include <QDebug>

extern "C" {
Q_DECL_IMPORT
    void uCNC(QString serverName);
}

VirtualUCNCConnection::VirtualUCNCConnection(QObject *parent) : Connection(parent)
{
    m_connected = false;
    m_connecting = false;
    m_socket = nullptr;
    m_server = nullptr;
}

VirtualUCNCConnection::~VirtualUCNCConnection()
{
}

void VirtualUCNCConnection::startLocalServer()
{
    m_server = new QLocalServer(this);
    connect(m_server, &QLocalServer::newConnection, this, &VirtualUCNCConnection::onNewConnection);
    m_server->listen("gpilotucnc");
}

void VirtualUCNCConnection::startWorkerThread()
{
    m_thread = new WorkerThread(m_server->serverName());
    m_thread->start();
}

bool VirtualUCNCConnection::openConnection()
{
    if (m_connecting) {
        return false;
    }
    if (m_connected) {
        return true;
    }

    m_connecting = true;

    startLocalServer();
    startWorkerThread();

    // still waiting for connection, return 'not connected'
    return false;
}

void VirtualUCNCConnection::flushOutgoingData()
{
    if (!m_socket) {
        qDebug() << "No socket connection!";
        return;
    }
    if (m_socket->bytesToWrite()) {
        m_socket->waitForBytesWritten(5);
    }
}

void VirtualUCNCConnection::sendByteArray(QByteArray byteArray)
{
    assert(m_socket != nullptr);

    flushOutgoingData();

    #ifdef DEBUG_UCNC_COMMUNICATION
        qDebug() << "uCNC (byte) >> " << byteArray.toHex();
    #endif

    m_socket->write(byteArray.data(), 1);
    m_socket->flush();
}

bool VirtualUCNCConnection::isConnected()
{
    return m_connected;
}

void VirtualUCNCConnection::sendLine(QString line)
{
    flushOutgoingData();

    #ifdef DEBUG_UCNC_COMMUNICATION
        qDebug() << "uCNC >> " << line;
    #endif

    std::string str = QString(line + "\n").toStdString();
    m_socket->write(str.c_str(), str.length());

    m_socket->flush();
}

void VirtualUCNCConnection::closeConnection()
{
    m_thread->terminate();
    m_thread->wait();
    delete m_thread;
    m_thread = nullptr;

    m_connected = false;
    if (m_socket != nullptr) {
        if (m_socket->isOpen()) {
            disconnect(m_socket, &QLocalSocket::disconnected, this, &VirtualUCNCConnection::onDisconnected);
            m_socket->abort();
        }
        delete m_socket;
        m_socket = nullptr;
    }
    if (m_server != nullptr && m_server->isListening()) {
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }
}

void VirtualUCNCConnection::onNewConnection()
{
    if (m_socket != nullptr) {
        qDebug() << "Virtual uCNC connection already exists!";
        return;
    }

    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QIODevice::readyRead, this, &VirtualUCNCConnection::onReadyRead);
    connect(m_socket, &QLocalSocket::disconnected, this, &VirtualUCNCConnection::onDisconnected);

    m_connecting = false;
    m_connected = true;

    emit connected();
}

void VirtualUCNCConnection::onDisconnected()
{
    closeConnection();
}

void VirtualUCNCConnection::onReadyRead()
{
    while (m_socket->bytesAvailable() > 0) {
        m_incoming += m_socket->readAll();
        processIncomingData();
    }
}

void VirtualUCNCConnection::processIncomingData()
{
    while (true) {
        if (m_incoming.isEmpty()) {
            return;
        }
        int pos = m_incoming.indexOf("\n");
        if (pos == -1) {
            return;
        }

        QString line = m_incoming.left(pos).trimmed();
        m_incoming.remove(0, pos + 1);

        #ifdef DEBUG_UCNC_COMMUNICATION
            qDebug() << "uCNC << " << line;
        #endif

        emit this->lineReceived(line);
    }
}

WorkerThread::WorkerThread(QString serverName) : QThread(nullptr), m_serverName(serverName) {
}

void WorkerThread::run() {
    qDebug() << "Starting virtual uCNC, server " << m_serverName;
    uCNC(m_serverName.toStdString().c_str());
    qDebug() << "uCNC stopped!";
}
