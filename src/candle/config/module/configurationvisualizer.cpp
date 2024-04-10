// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "configurationvisualizer.h"

const QMap<QString, QVariant> DEFAULTS = {
    {"lineWidth", 1.5},
    {"fpsLock", 60},
    {"antialiasing", true},
    {"msaa", true},
    {"zBuffer", false},
    {"vsync", false},
    {"programDrawMode", ConfigurationVisualizer::ProgramDrawMode::Vectors},
    {"simplifyGeometry", false},
    {"simplifyGeometryPrecision", 0.1},
    {"grayscaleSegments", false},
    {"grayscaleSegmentsBySCode", true},
    {"grayscaleSegmentsByZCode", false},
    {"fieldOfView", 60},
    {"nearPlane", 0.5},
    {"farPlane", 10000.0},
    {"backgroundColor", QColor(0, 0, 0)},
    {"toolColor", QColor(255, 255, 255)},
    {"textColor", QColor(255, 255, 255)},
    {"normalToolpathColor", QColor(255, 255, 255)},
    {"drawnToolpathColor", QColor(0, 255, 0)},
    {"hightlightToolpathColor", QColor(255, 0, 0)},
    {"zMovementColor", QColor(0, 0, 255)},
    {"startPointColor", QColor(0, 255, 0)},
    {"endPointColor", QColor(255, 0, 0)},
    {"toolDiameter", 3.175},
    {"toolLength", 30.0},
    {"toolType", ConfigurationVisualizer::ToolType::Flat},
    {"toolAngle", 15.0}
};

ConfigurationVisualizer::ConfigurationVisualizer(QObject *parent) : ConfigurationModule(parent, DEFAULTS)
{    
}
