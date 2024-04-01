#ifndef CONFIG_PERSISTER_H
#define CONFIG_PERSISTER_H

#include <QObject>

class Persister : public QObject
{
    Q_OBJECT

    public:
        Persister(QObject *parent) : QObject(parent) {};
        virtual bool open() = 0;
        virtual void close() = 0;
        virtual bool setInt(const QString group, const QString key, const int value) = 0;
        virtual bool setBool(const QString group, const QString key, const bool value) = 0;
        virtual bool setString(const QString group, const QString key, const QString value) = 0;
        virtual bool setFloat(const QString group, const QString key, const float value) = 0;
};

#endif // CONFIG_PERSISTER_H
