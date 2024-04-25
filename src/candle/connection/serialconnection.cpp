#include "serialconnection.h"
#include <QDebug>
#include <QTimer>

SerialConnection::SerialConnection(QObject *parent) : Connection(parent)
{
    setupSerialPort();
}

SerialConnection::~SerialConnection()
{
    if (m_serialPort.isOpen()) m_serialPort.close();
}

bool SerialConnection::openConnection()
{
    if (m_connecting) {
        return false;
    }
    if (m_serialPort.isOpen()) {
        return true;
    }

    if (!m_serialPort.open(QIODevice::ReadWrite)) {
        return false;
    }

    m_connecting = true;
    QTimer::singleShot(100, this, [this]() {
        m_connecting = false;
        emit this->connected();
    });

    return true;
}

void SerialConnection::setPortName(QString portName)
{
    m_serialPort.setPortName(portName);
}

void SerialConnection::setBaudRate(int baudRate)
{
    m_serialPort.setBaudRate(baudRate);
}

void SerialConnection::sendByteArray(QByteArray data)
{
    m_serialPort.write(data);
}

bool SerialConnection::isConnected()
{
    return !m_connecting && m_serialPort.isOpen();
}

void SerialConnection::sendLine(QString data)
{
    if (!isConnected()) {
        return;
    }

    m_serialPort.write((data + "\n").toLatin1());
}

void SerialConnection::closeConnection()
{
    if (m_serialPort.isOpen()) m_serialPort.close();
}

void SerialConnection::setupSerialPort()
{
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort.setStopBits(QSerialPort::OneStop);

    // Signals/slots
    connect(&m_serialPort, &QIODevice::readyRead, this, &SerialConnection::onSerialPortReadyRead, Qt::QueuedConnection);
    connect(&m_serialPort, &QSerialPort::errorOccurred, this, &SerialConnection::onSerialPortError);
}

void SerialConnection::onSerialPortReadyRead()
{
    while (m_serialPort.canReadLine()) {
        QString line = m_serialPort.readLine().trimmed();
        emit this->lineReceived(line);
    }
}

void SerialConnection::onSerialPortError(QSerialPort::SerialPortError error)
{
    static QSerialPort::SerialPortError previousError;

    if (error != QSerialPort::NoError && error != previousError) {
        previousError = error;
        if (m_serialPort.isOpen()) {
            m_serialPort.close();
        }
        emit this->error(tr("Serial port error ") + QString::number(error) + ": " + m_serialPort.errorString());
    }
}
