#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "module/module.h"
#include "module/configurationconnection.h"
#include "module/configurationvisualizer.h"
#include "module/configurationsender.h"
#include "module/configurationconsole.h"
#include "persistence/ini/inipersister.h"
#include "persistence/ini/iniprovider.h"
#include <QObject>

class Configuration : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(ConfigurationConnection connection READ connectionModule CONSTANT);
    Q_PROPERTY(ConfigurationVisualizer visualizer READ visualizerModule CONSTANT);
    Q_PROPERTY(ConfigurationSender sender READ senderModule CONSTANT);
    Q_PROPERTY(ConfigurationConsole console READ consoleModule CONSTANT);

    public:
        Configuration(QObject *parent);
        QString language();
        void setLanguage(QString);
        void save();
        void load();
        void setDefaults();
        const ConfigurationConnection& connectionModule();
        const ConfigurationVisualizer& visualizerModule();
        const ConfigurationSender& senderModule();
        ConfigurationConsole& consoleModule();
    private:
        QString m_language;

        // Modules
        ConfigurationSender m_sender;
        ConfigurationConnection m_connection;
        ConfigurationVisualizer m_visualizer;
        ConfigurationConsole m_console;

        // Read/Write
        IniPersister m_persister;
        IniProvider m_provider;

        void saveModule(ConfigurationModule&);
        void setModuleDefaults(ConfigurationModule&);
        void loadModule(ConfigurationModule&);
        bool persistByType(QString module, QString name, QVariant value, QString type);

    signals:
        void configurationChanged();
        void defaultConfigurationLoaded();

};

#endif // CONFIGURATION_H
