// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATIONUI_H
#define CONFIGURATIONUI_H

#include <QObject>
#include "configurationmodule.h"

class ConfigurationUI : public ConfigurationModule
{
    friend class frmSettings;

    Q_OBJECT
    Q_PROPERTY(int fontSize MEMBER m_fontSize NOTIFY changed)
    Q_PROPERTY(QString language MEMBER m_language NOTIFY changed)
    Q_PROPERTY(QStringList recentFiles MEMBER m_recentFiles NOTIFY changed)
    Q_PROPERTY(bool autoScrollGCode MEMBER m_autoScrollGCode NOTIFY changed)
    Q_PROPERTY(QString lastFileOpenDir MEMBER m_lastFileOpenDir NOTIFY changed)

    public:
        explicit ConfigurationUI(QObject *parent);
        QString getSectionName() override { return "baseui.main"; }

        int fontSize() const { return m_fontSize; }
        QString language() const { return m_language; }
        QStringList recentFiles() const { return m_recentFiles; }
        bool autoScrollGCode() const { return m_autoScrollGCode; }
        void setAutoScrollGCode(bool autoScrollGCode) { m_autoScrollGCode = autoScrollGCode; emit changed(); }
        QString lastFileOpenDir() const { return m_lastFileOpenDir; }
        void setLastFileOpenDir(const QString &lastFileOpenDir) { m_lastFileOpenDir = lastFileOpenDir; emit changed(); }

    private:
        int m_fontSize;
        QString m_language;
        QStringList m_recentFiles;
        bool m_autoScrollGCode;
        QString m_lastFileOpenDir;
};

#endif // CONFIGURATIONUI_H
