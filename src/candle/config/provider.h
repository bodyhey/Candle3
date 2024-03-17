#ifndef PROVIDER_H
#define PROVIDER_H

#include <QSettings>
#include <QObject>

class Provider : public QObject
{
    Q_OBJECT

public:
    Provider(QObject *parent);

private:
    QSettings* open();
};

#endif // PROVIDER_H
