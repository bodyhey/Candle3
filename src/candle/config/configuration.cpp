#include "configuration.h"
#include "registry.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QDebug>

Configuration::Configuration(QObject *parent)
    : QObject(parent),
    m_sender(parent),
    m_connection(parent),
    m_visualizer(parent),
    m_console(parent),
    m_parser(parent),
    m_ui(parent),
    m_machine(parent),
    m_heightmap(parent),
    m_persister(parent),
    m_provider(parent)
{
    m_modules << &m_sender
        << &m_connection
        << &m_visualizer
        << &m_console
        << &m_parser
        << &m_ui
        << &m_machine
        << &m_heightmap;
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
    qDebug() << "Save configurations";

    m_persister.open();
    for (ConfigurationModule* module : qAsConst(m_modules)) {
        saveModule(module);
    }
    m_persister.close();
}

bool Configuration::persistByType(QString module, QString name, QVariant value, QString type)
{
    if (type == "QString") {
        m_persister.setString(module, name, value.toString());
    } else if (type == "int") {
        m_persister.setInt(module, name, value.toInt());
    } else if (type == "bool") {
        m_persister.setBool(module, name, value.toBool());
    } else if (type == "double" || type == "float") {
        m_persister.setDouble(module, name, value.toDouble());
    } else if (type == "QStringList") {
        m_persister.setStringList(module, name, value.toStringList());
    } else {
        return false;
    }

    return true;
}

void Configuration::saveModule(ConfigurationModule *module)
{
    const QMetaObject *metaObj = module->metaObject();
    // qDebug() << "MetaObject: " << metaObj->className();
    // qDebug() << "Property Count: " << metaObj->propertyCount();
    // qDebug() << "Property Offset: " << metaObj->classInfoCount();
    for (int i = 0; i < metaObj->propertyCount(); ++i) {
        QMetaProperty prop = metaObj->property(i);

        if (QString(prop.name()) == "objectName") continue;

        QVariant value = prop.read(module);

        //auto val = value.value<"abc">();

//        qDebug() << "wartość" << prop.typeName() << prop.name() << value << value.isValid();

        if (!persistByType(module->getSectionName(), prop.name(), value, prop.typeName())) {
            auto registryItem = ConfigRegistry::getInfo(prop.typeName());
            char *pos = (char*) value.data();
            switch (registryItem.type) {
                case ConfigRegistry::Type::Unknown:
                    // qDebug() << "Custom get: " << prop.name() << prop.typeName();
                    break;
                case ConfigRegistry::Type::Struct: {
                    QVariantMap normalized = registryItem.normalizeStruct((const char*)value.constData());
                    m_persister.setVariantMap(module->getSectionName(), prop.name(), normalized);
                    // qDebug() << "Save struct: " << prop.name();
                    break;
                }
                case ConfigRegistry::Type::Value: {
                    QVariant normalized = registryItem.normalizeValue(value);
                    m_persister.setVariant(module->getSectionName(), prop.name(), normalized);
                    // qDebug() << "Save value: " << prop.name() << normalized << normalized.typeName();
                    break;
                }
                case ConfigRegistry::Type::Enum:
                    persistByType(module->getSectionName(), prop.name(), prop.read(module), "int");
                    break;
            }

            // QVariant value = module->customGet(prop.name());
            // if (!persistByType(module->getSectionName(), prop.name(), value, value.typeName())) {
            //     // something went wrong
            //     // @TODO exception??
            // }
        }
    }
}

void Configuration::setModuleDefaults(ConfigurationModule *module)
{
    QMap<QString, QVariant> defaults = module->getDefaults();

    const QMetaObject *metaObj = module->metaObject();
    for (int i = 0; i < metaObj->propertyCount(); ++i) {
        QMetaProperty prop = metaObj->property(i);

        if (QString(prop.name()) == "objectName" || QString(prop.name()) == "DEFAULTS") continue;

        QString type(prop.typeName());
        if (type == "QString") {
            prop.write(module, defaults[prop.name()].toString());
        } else if (type == "int") {
            prop.write(module, defaults[prop.name()].toInt());
        } else if (type == "bool") {
            prop.write(module, defaults[prop.name()].toBool());
        } else if (type == "float") {
            prop.write(module, defaults[prop.name()].toFloat());
        } else if (type == "double") {
            prop.write(module, defaults[prop.name()].toDouble());
        } else {
            module->customSet(prop.name(), defaults[prop.name()]);
        }
    }
}

void Configuration::load()
{
    qDebug() << "Load configurations";

    m_provider.open();
    for (ConfigurationModule* module : qAsConst(m_modules)) {
        loadModule(module);
    }
    m_provider.close();
}

void Configuration::setDefaults()
{
    for (ConfigurationModule* module : qAsConst(m_modules)) {
        setModuleDefaults(module);
    }

    emit defaultConfigurationLoaded();
}

void Configuration::loadModule(ConfigurationModule *module)
{
    QMap<QString, QVariant> defaults = module->getDefaults();

    const QMetaObject *metaObj = module->metaObject();
    // qDebug() << "MetaObject: " << metaObj->className();
    // qDebug() << "Property Count: " << metaObj->propertyCount();
    // qDebug() << "Property Offset: " << metaObj->classInfoCount();
    for (int i = 0; i < metaObj->propertyCount(); ++i) {
        QMetaProperty prop = metaObj->property(i);

        if (QString(prop.name()) == "objectName" || QString(prop.name()) == "DEFAULTS") continue;

        QString type(prop.typeName());
        if (type == "QString") {
            prop.write(module, m_provider.getString(module->getSectionName(), QString(prop.name()), defaults[prop.name()].toString()));
        } else if (type == "int") {
            prop.write(module, m_provider.getInt(module->getSectionName(), QString(prop.name()), defaults[prop.name()].toInt()));
        } else if (type == "bool") {
            prop.write(module, m_provider.getBool(module->getSectionName(), QString(prop.name()), defaults[prop.name()].toBool()));
        } else if (type == "float_") {
            //prop.write(module, m_provider.getFloat(module->getSectionName(), QString(prop.name()), defaults[prop.name()].toFloat()));
        } else if (type == "double" || type == "float") {
            prop.write(module, m_provider.getDouble(module->getSectionName(), QString(prop.name()), defaults[prop.name()].toDouble()));
        }/* else if (type == "QColor") {
        } else if (type == "QStringList") {
            //prop.write(module, m_provider.getStringList(module->getSectionName(), QString(prop.name()), defaults[prop.name()].toStringList()));
        } else if (type == "QMap<QString,QString>___") {
            //prop.write(module, m_provider.getStringStringMap(module->getSectionName(), QString(prop.name()), defaults[prop.name()].toMap()));
        }*/ else {
            qDebug() << "Ładowanie: " << prop.name() << type << "wr" << prop.isWritable();
            auto registryItem = ConfigRegistry::getInfo(prop.typeName());

            switch (registryItem.type) {
                case ConfigRegistry::Type::Unknown:

                    //module->customSet(prop.name(), m_provider.get(module->getSectionName(), QString(prop.name()), defaults[prop.name()]));
                    break;
                case ConfigRegistry::Type::Value: {
                    QVariant denormalized = registryItem.denormalizeValue(
                        m_provider.getVariant(module->getSectionName(), prop.name(), defaults[prop.name()])
                    );

                    qDebug() << denormalized;

                    prop.write(module, denormalized);

                    break;
                }
                case ConfigRegistry::Type::Struct: {
                    QVariant denormalized = registryItem.denormalizeStruct(
                        m_provider.getVariantMap(module->getSectionName(), prop.name(), defaults[prop.name()].toMap())
                    );

                    qDebug() << defaults[prop.name()].toMap()
                             << m_provider.getVariantMap(module->getSectionName(), prop.name(), defaults[prop.name()].toMap())
                             << denormalized;

                    if (QString(prop.name()) == "spindleSpeedRange") {
                        ConfigurationModule::MinMax mm = denormalized.value<ConfigurationModule::MinMax>();
                        qDebug() << mm.min << mm.max;
                    }

                    prop.write(module, denormalized);

                    break;
                }
                case ConfigRegistry::Type::Enum:
                    prop.write(module, m_provider.getInt(module->getSectionName(), QString(prop.name()), defaults[prop.name()].toInt()));
                    break;
            }
        }
    }
}

