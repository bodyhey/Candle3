#include "persister.h"
#include "../globals.h"
#include "qguiapplication.h"

Persister::Persister(QObject *parent) : QObject(parent)
{
    m_settings = nullptr;
}

bool Persister::open()
{
    if (m_settings) {
        return false;
    }

    m_settings = new QSettings(qApp->applicationDirPath() + "/" + CONFIGURATION_FILE, QSettings::IniFormat);
}

void Persister::close()
{
    if (!m_settings) {
        return;
    }

    delete m_settings;
    m_settings = nullptr;
}

bool Persister::setInt(const QString group, const QString key, const int value)
{
    if (!m_settings) {
        return false;
    }
    m_settings->setValue(group + '/' + key, value);

    return true;
}

bool Persister::setString(const QString group, const QString key, const QString value)
{
    if (!m_settings) {
        return false;
    }
    m_settings->setValue(group + '/' + key, value);

    return true;
}

bool Persister::setFloat(const QString group, const QString key, const float value)
{
    if (!m_settings) {
        return false;
    }
    m_settings->setValue(group + '/' + key, value);

    return true;
}
