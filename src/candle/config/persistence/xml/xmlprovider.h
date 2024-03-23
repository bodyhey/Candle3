#ifndef XML_CONFIG_PROVIDER_H
#define XML_CONFIG_PROVIDER_H

#include <QSettings>
#include <QObject>
#include "../provider.h"

class XmlProvider : public Provider
{
   public:
        XmlProvider(QObject *parent);
        bool open() override;
        void close() override;
        int getInt(const QString group, const QString key, int defaultValue) override;
        QString getString(const QString group, const QString key, QString defaultValue) override;
        float getFloat(const QString group, const QString key, float defaultValue) override;
        QVariant get(const QString group, const QString key, QVariant defaultValue) override;

    private:
        QSettings *m_settings;
};

#endif // XML_CONFIG_PROVIDER_H
