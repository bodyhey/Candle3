#ifndef INI_CONFIG_PROVIDER_H
#define INI_CONFIG_PROVIDER_H

#include <QSettings>
#include <QObject>
#include "../provider.h"

class IniProvider : public Provider
{
    public:
        IniProvider(QObject *parent);
        bool open() override;
        void close() override;
        int getInt(const QString group, const QString key, int defaultValue) override;
        QString getString(const QString group, const QString key, QString defaultValue) override;
        float getFloat(const QString group, const QString key, float defaultValue) override;
        QVariant get(const QString group, const QString key, QVariant defaultValue) override;

    private:
        QSettings *m_settings;
};

#endif // INI_CONFIG_PROVIDER_H
