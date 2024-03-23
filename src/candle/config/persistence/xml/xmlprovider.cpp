#include "xmlprovider.h"
#include "../../../globals.h"
#include "qguiapplication.h"
#include <QDebug>

XmlProvider::XmlProvider(QObject *parent) : Provider(parent)
{
    m_settings = nullptr;
}

bool XmlProvider::open()
{
    if (m_settings) {
        return false;
    }

    m_settings = new QSettings(qApp->applicationDirPath() + "/" + CONFIGURATION_FILE, QSettings::CustomFormat1);

    return true;
}

void XmlProvider::close()
{
    if (!m_settings) {
        return;
    }

    delete m_settings;
    m_settings = nullptr;
}

int XmlProvider::getInt(const QString group, const QString key, int defaultValue)
{
    qDebug() << "Getting: " << group + '\\' + key << " with default: " << defaultValue << " and value: " << m_settings->value(group + '\\' + key, defaultValue).toInt();

    return m_settings->value(group + '\\' + key, defaultValue).toInt();
}

QString XmlProvider::getString(const QString group, const QString key, QString defaultValue)
{
    qDebug() << "Getting: " << group + '\\' + key << " with default: " << defaultValue << " and value: " << m_settings->value(group + '\\' + key, defaultValue).toString();

    return m_settings->value(group + '\\' + key, defaultValue).toString();
}

float XmlProvider::getFloat(const QString group, const QString key, float defaultValue)
{
    qDebug() << "Getting: " << group + '\\' + key << " with default: " << defaultValue << " and value: " << m_settings->value(group + '\\' + key, defaultValue).toFloat();

    return m_settings->value(group + '\\' + key, defaultValue).toFloat();
}

QVariant XmlProvider::get(const QString group, const QString key, QVariant defaultValue)
{
    return m_settings->value(group + '\\' + key, defaultValue);
}
