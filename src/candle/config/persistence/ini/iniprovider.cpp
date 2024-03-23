#include "iniprovider.h"
#include "../../../globals.h"
#include "qguiapplication.h"
#include <QDebug>

IniProvider::IniProvider(QObject *parent) : Provider(parent)
{
    m_settings = nullptr;
}

bool IniProvider::open()
{
    if (m_settings) {
        return false;
    }

    m_settings = new QSettings(qApp->applicationDirPath() + "/" + CONFIGURATION_FILE, QSettings::IniFormat);

    return true;
}

void IniProvider::close()
{
    if (!m_settings) {
        return;
    }

    delete m_settings;
    m_settings = nullptr;
}

int IniProvider::getInt(const QString group, const QString key, int defaultValue)
{
    qDebug() << "Getting: " << group + '\\' + key << " with default: " << defaultValue << " and value: " << m_settings->value(group + '\\' + key, defaultValue).toInt();

    return m_settings->value(group + '\\' + key, defaultValue).toInt();
}

QString IniProvider::getString(const QString group, const QString key, QString defaultValue)
{
    qDebug() << "Getting: " << group + '\\' + key << " with default: " << defaultValue << " and value: " << m_settings->value(group + '\\' + key, defaultValue).toString();

    return m_settings->value(group + '\\' + key, defaultValue).toString();
}

float IniProvider::getFloat(const QString group, const QString key, float defaultValue)
{
    qDebug() << "Getting: " << group + '\\' + key << " with default: " << defaultValue << " and value: " << m_settings->value(group + '\\' + key, defaultValue).toFloat();

    return m_settings->value(group + '\\' + key, defaultValue).toFloat();
}

QVariant IniProvider::get(const QString group, const QString key, QVariant defaultValue)
{
    return m_settings->value(group + '\\' + key, defaultValue);
}
