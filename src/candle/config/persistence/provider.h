#ifndef CONFIG_PROVIDER_H
#define CONFIG_PROVIDER_H

#include <QSettings>
#include <QObject>

class Provider : public QObject
{
    Q_OBJECT

    public:
        Provider(QObject *parent) : QObject(parent) {};
        virtual bool open() = 0;
        virtual void close() = 0;
        virtual int getInt(const QString group, const QString key, int defaultValue) = 0;
        virtual QString getString(const QString group, const QString key, QString defaultValue) = 0;
        virtual float getFloat(const QString group, const QString key, float defaultValue) = 0;
        virtual QVariant get(const QString group, const QString key, QVariant defaultValue) = 0;
};

#endif // CONFIG_PROVIDER_H
