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
    Q_PROPERTY(QStringList recentHeightmaps MEMBER m_recentHeightmaps NOTIFY changed)
    Q_PROPERTY(bool autoScrollGCode MEMBER m_autoScrollGCode NOTIFY changed)
    Q_PROPERTY(QString currentWorkingDirectory MEMBER m_currentWorkingDirectory NOTIFY changed)

    public:
        explicit ConfigurationUI(QObject *parent);
        QString getSectionName() override { return "baseui.main"; }

        int fontSize() const { return m_fontSize; }
        QString language() const { return m_language; }
        QStringList recentFiles() const { return m_recentFiles; }
        QStringList recentHeightmaps() const { return m_recentHeightmaps; }
        bool autoScrollGCode() const { return m_autoScrollGCode; }
        void setAutoScrollGCode(bool autoScrollGCode) { m_autoScrollGCode = autoScrollGCode; emit changed(); }
        QString currentWorkingDirectory() const { return m_currentWorkingDirectory; }
        void currentWorkingDirectory(const QString &directory) { m_currentWorkingDirectory = directory; emit changed(); }
        void addRecentFile(const QString &fileName);
        void addRecentHeightmap(const QString &fileName);
        bool hasAnyRecentFiles() const;
        bool hasAnyRecentHeightmaps() const;
        void clearRecentFiles();
        void clearRecentHeightmaps();

    private:
        static const int MAX_RECENT_FILES = 10;
        int m_fontSize;
        QString m_language;
        QStringList m_recentFiles;
        QStringList m_recentHeightmaps;
        bool m_autoScrollGCode;
        QString m_currentWorkingDirectory;
};

#endif // CONFIGURATIONUI_H
