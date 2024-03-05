#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>

class Configuration : public QObject
{
    Q_OBJECT;
public:
    Configuration();
    QString language();
    void setLanguage(QString);

private:
    QString m_language;

};

#endif // CONFIGURATION_H
