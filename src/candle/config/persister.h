#ifndef PERSISTER_H
#define PERSISTER_H

#include <QSettings>
#include <QObject>

class Persister : public QObject
{
    Q_OBJECT

    public:
        Persister(QObject *parent);
        bool open();
    private:
        QSettings *m_settings;
};

#endif // PERSISTER_H
