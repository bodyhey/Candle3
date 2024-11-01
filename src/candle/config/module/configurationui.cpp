// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationui.h"

const QMap<QString,QVariant> DEFAULTS = {
    {"fontSize", 8},
    {"language", "en"},
    {"recentFiles", QStringList{"a.txt", "b.txt", "c.txt"}},
    {"recentHeightmaps", QStringList{"map1.txt", "map2.txt", "map3.txt"}},
    {"autoScrollGCode", true},
    {"lastFileOpenDir", ""},
    {"lockWindows", false},
    {"lockPanels", false},
    // -1 is a special value to indicate that the window should be centered
    // -1 is a special value to indicate that the window should be maximized
    {"mainFormGeometry", QVariantMap({{"x", 0}, {"y", 0}, {"width", -1}, {"height", -1}})},
    {"settingsFormGeometry", QVariantMap({{"x", -1}, {"y", -1}, {"width", 800}, {"height", 600}})},
    {"settingsFormSlicerSizes", QVariantList{20, 200}},
    {"grblConfigratorFormGeometry", QVariantMap({{"x", -1}, {"y", -1}, {"width", 800}, {"height", 600}})},
    {"spindleSpeedRange", QVariantMap({{"min", 0}, {"max", 100}})}
};

ConfigurationUI::ConfigurationUI(QObject *parent) : ConfigurationModule(parent, DEFAULTS)
{
}

void ConfigurationUI::addRecentFile(const QString &fileName)
{
    m_recentFiles.removeAll(fileName);
    m_recentFiles.append(fileName);
    m_recentFiles = m_recentFiles.mid(qMax(0, m_recentFiles.size() - MAX_RECENT_FILES));
}

void ConfigurationUI::addRecentHeightmap(const QString &fileName)
{
    m_recentHeightmaps.removeAll(fileName);
    m_recentHeightmaps.append(fileName);
    m_recentHeightmaps = m_recentHeightmaps.mid(qMax(0, m_recentHeightmaps.size() - MAX_RECENT_FILES));
}

bool ConfigurationUI::hasAnyRecentFiles() const
{
    return !m_recentFiles.empty();
}

bool ConfigurationUI::hasAnyRecentHeightmaps() const
{
    return !m_recentHeightmaps.empty();
}

void ConfigurationUI::clearRecentFiles()
{
    m_recentFiles.clear();
}

void ConfigurationUI::clearRecentHeightmaps()
{
    m_recentHeightmaps.clear();
}
