#include "provider.h"
#include "qguiapplication.h"

Provider::Provider(QObject *parent) : QObject(parent)
{
}

QSettings *Provider::open()
{
    return new QSettings(qApp->applicationDirPath() + "/settings.ini", QSettings::IniFormat);
}
