// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATIONUI_H
#define CONFIGURATIONUI_H

#include <QObject>
#include "module.h"

class ConfigurationUI : public ConfigurationModule
{
    friend class frmSettings;

    Q_OBJECT
    Q_PROPERTY(int fontSize MEMBER m_fontSize NOTIFY changed)
    Q_PROPERTY(QString language MEMBER m_language NOTIFY changed)

    public:
        explicit ConfigurationUI(QObject *parent);
        QString getSectionName() override { return "baseui.main"; }

        int fontSize() const { return m_fontSize; }
        QString language() const { return m_language; }

    private:
        int m_fontSize;
        QString m_language;

};

#endif // CONFIGURATIONUI_H
