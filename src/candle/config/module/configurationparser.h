// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATIONPARSER_H
#define CONFIGURATIONPARSER_H

#include <QObject>
#include "module.h"

class ConfigurationParser : public ConfigurationModule
{
    friend class frmSettings;

    Q_OBJECT

    public:
        explicit ConfigurationParser(QObject *parent);
        ConfigurationParser& operator=(const ConfigurationParser&) { return *this; }
        QString getSectionName() override { return "parser"; }

        enum ApproximatationMode {
            ByLength,
            ByAngle
        };
        Q_ENUM(ApproximatationMode);

        ApproximatationMode approximatationMode() const { return m_approximatationMode; }
        float approximatationLength() const { return m_approximatationLength; }
        float approximatationAngle() const { return m_approximatationAngle; }

    private:
        ApproximatationMode m_approximatationMode;
        float m_approximatationLength;
        float m_approximatationAngle;
};

#endif // CONFIGURATIONPARSER_H
