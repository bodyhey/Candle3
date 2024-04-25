#include "connection.h"

Connection::Connection(QObject *parent) : QObject(parent)
{    
}

void Connection::sendChar(QChar char_)
{
    sendChar(char_.toLatin1());
}

void Connection::sendChar(char char_)
{
    sendByteArray(QByteArray(1, char_));
}
