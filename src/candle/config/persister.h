#ifndef CONFIG_PERSISTER_H
#define CONFIG_PERSISTER_H

#include <QSettings>
#include <QObject>

class Persister : public QObject
{
    Q_OBJECT

    public:
        Persister(QObject *parent);
        bool open();
        void close();
        bool setInt(const QString group, const QString key, const int value);
        bool setString(const QString group, const QString key, const QString value);
        bool setFloat(const QString group, const QString key, const float value);

    private:
        QSettings *m_settings;
};

#endif // CONFIG_PERSISTER_H
