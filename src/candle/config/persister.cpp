#include "persister.h"
#include "qguiapplication.h"

Persister::Persister(QObject *parent) : QObject(parent)
{
}

bool Persister::open()
{
    //return new QSettings(qApp->applicationDirPath() + "/settings.ini", QSettings::IniFormat);
    return true;
}
