#ifndef INI_CONFIG_PERSISTER_H
#define INI_CONFIG_PERSISTER_H

#include <QSettings>
#include <QObject>
#include "../persister.h"

class IniPersister : public Persister
{
    public:
        IniPersister(QObject *parent);
        bool open() override;
        void close() override;
        bool setInt(const QString group, const QString key, const int value) override;
        bool setString(const QString group, const QString key, const QString value) override;
        bool setFloat(const QString group, const QString key, const float value) override;

    private:
        QSettings *m_settings;
};

#endif // INI_CONFIG_PERSISTER_H
