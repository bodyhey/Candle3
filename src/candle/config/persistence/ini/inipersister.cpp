#include "inipersister.h"
#include "../../../globals.h"
#include "qguiapplication.h"

IniPersister::IniPersister(QObject *parent) : Persister(parent)
{
    m_settings = nullptr;
}

bool IniPersister::open()
{
    if (m_settings) {
        return false;
    }

    m_settings = new QSettings(qApp->applicationDirPath() + "/" + CONFIGURATION_FILE, QSettings::IniFormat);
}

void IniPersister::close()
{
    if (!m_settings) {
        return;
    }

    delete m_settings;
    m_settings = nullptr;
}

bool IniPersister::setInt(const QString group, const QString key, const int value)
{
    if (!m_settings) {
        return false;
    }
    m_settings->setValue(group + '/' + key, value);

    return true;
}

bool IniPersister::setString(const QString group, const QString key, const QString value)
{
    if (!m_settings) {
        return false;
    }
    m_settings->setValue(group + '/' + key, value);

    return true;
}

bool IniPersister::setFloat(const QString group, const QString key, const float value)
{
    if (!m_settings) {
        return false;
    }
    m_settings->setValue(group + '/' + key, value);

    return true;
}

bool IniPersister::setBool(const QString group, const QString key, const bool value)
{
    if (!m_settings) {
        return false;
    }
    m_settings->setValue(group + '/' + key, value);

    return true;
}
