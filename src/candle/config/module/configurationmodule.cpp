// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationmodule.h"
#include <QColor>

ConfigurationModule::ConfigurationModule(QObject *parent, QMap<QString, QVariant> defaults) : QObject(parent), m_defaults(defaults)
{
    ConfigurationRegistry::registerStruct(
        "ConfigurationModule::MinMax",
        [](const char* data) -> QVariantMap {
            ConfigurationModule::MinMax *minMax = (ConfigurationModule::MinMax*)data;

            return {
                {"min", minMax->min},
                {"max", minMax->max},
            };
        },
        [](QVariantMap map) -> QVariant {
            ConfigurationModule::MinMax minMax = ConfigurationModule::MinMax{
                map["min"].toInt(),
                map["max"].toInt(),
            };
            qDebug() << "min" << minMax.min << "max" << minMax.max;
            QVariant v = QVariant::fromValue(minMax);

            qDebug() << v;

            ConfigurationModule::MinMax minMax2 = v.value<ConfigurationModule::MinMax>();
            qDebug() << "min" << minMax2.min << "max" << minMax2.max;

            return QVariant::fromValue(ConfigurationModule::MinMax{
                map["min"].toInt(),
                map["max"].toInt(),
            });
        }
    );
    ConfigurationRegistry::registerStruct(
        "ConfigurationModule::MinMaxDouble",
        [](const char* data) -> QVariantMap {
            ConfigurationModule::MinMaxDouble *minMax = (ConfigurationModule::MinMaxDouble*)data;

            return {
                {"min", minMax->min},
                {"max", minMax->max},
            };
        },
        [](QVariantMap map) -> QVariant {
            MinMaxDouble minMax = ConfigurationModule::MinMaxDouble{
                map["min"].toDouble(),
                map["max"].toDouble(),
            };
            qDebug() << "min" << minMax.min << "max" << minMax.max;
            QVariant v = QVariant::fromValue(minMax);
            qDebug() << v;
            return QVariant::fromValue(ConfigurationModule::MinMaxDouble{
                map["min"].toDouble(),
                map["max"].toDouble(),
            });
        }
    );
    ConfigurationRegistry::registerValue(
        "QColor",
        [](QVariant raw) -> QVariant {
            return raw.value<QColor>().name(QColor::HexArgb);
        },
        [](QVariant serialized) -> QVariant {
            return QColor(serialized.toString());
        }
    );
}
