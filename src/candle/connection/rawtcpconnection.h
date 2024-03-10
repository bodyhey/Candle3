#ifndef RAWTCPCONNECTION_H
#define RAWTCPCONNECTION_H

#include <QObject>
#include "connection.h"

class RawTcpConnection : public Connection
{
public:
    explicit RawTcpConnection(QObject *parent = nullptr);
};

#endif // RAWTCPCONNECTION_H
