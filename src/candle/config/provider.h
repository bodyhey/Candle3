#ifndef CONFIG_PROVIDER_H
#define CONFIG_PROVIDER_H

#include <QSettings>
#include <QObject>

class Provider : public QObject
{
    Q_OBJECT

    public:
        Provider(QObject *parent);
        bool open();
        void close();
        int getInt(const QString group, const QString key, int defaultValue);
        QString getString(const QString group, const QString key, QString defaultValue);
        float getFloat(const QString group, const QString key, float defaultValue);
        QVariant get(const QString group, const QString key, QVariant defaultValue);

    private:
        QSettings *m_settings;
};

#endif // CONFIG_PROVIDER_H
