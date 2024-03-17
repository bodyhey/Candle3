#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "module/configurationconnection.h"
#include "module/configurationvisualizer.h"
#include "module/configurationsender.h"
#include "persister.h"
#include "provider.h"
#include <QObject>

class Configuration : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(ConfigurationConnection connection READ connectionModule CONSTANT);
    Q_PROPERTY(ConfigurationVisualizer visualizer READ visualizerModule CONSTANT);
    Q_PROPERTY(ConfigurationSender sender READ senderModule CONSTANT);
public:
    Configuration(QObject *parent);
    QString language();
    void setLanguage(QString);
    void save();
    void load();

private:
    QString m_language;
    ConfigurationSender m_sender;
    ConfigurationConnection m_connection;
    ConfigurationVisualizer m_visualizer;
    Persister m_persister;
    Provider m_provider;
    const ConfigurationConnection& connectionModule();
    const ConfigurationVisualizer& visualizerModule();
    const ConfigurationSender& senderModule();

signals:
    void configurationChanged();

};

#endif // CONFIGURATION_H
