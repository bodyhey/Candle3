#include "configuration.h"

Configuration::Configuration(QObject *parent)
    : QObject(parent),
    m_sender(parent),
    m_connection(parent),
    m_visualizer(parent),
    m_persister(parent),
    m_provider(parent)
{
}

QString Configuration::language()
{
    return this->m_language;
}

void Configuration::setLanguage(QString language)
{
    this->m_language = language;
}

void Configuration::save()
{
    m_connection.save();
    m_visualizer.save();
    m_sender.save();
}

void Configuration::load()
{
    m_connection.load();
    m_visualizer.load();
    m_sender.load();
}

const ConfigurationConnection& Configuration::connectionModule()
{
    return m_connection;
}

const ConfigurationVisualizer& Configuration::visualizerModule()
{
    return m_visualizer;
}

const ConfigurationSender& Configuration::senderModule()
{
    return m_sender;
}
