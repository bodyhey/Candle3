// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATION_VISUALIZER_H
#define CONFIGURATION_VISUALIZER_H

#include <QObject>
#include "module.h"

class ConfigurationVisualizer : public ConfigurationModule
{
    friend class frmSettings;

    Q_OBJECT;
    Q_PROPERTY(int lineWidth MEMBER m_lineWidth NOTIFY changed);
    Q_PROPERTY(int fpsLock MEMBER m_fpsLock NOTIFY changed);
    Q_PROPERTY(bool smoothing MEMBER m_smoothing NOTIFY changed);
    Q_PROPERTY(bool msaa MEMBER m_msaa NOTIFY changed);
    Q_PROPERTY(bool zBuffer MEMBER m_zBuffer NOTIFY changed);
    Q_PROPERTY(bool vsync MEMBER m_vsync NOTIFY changed);
    Q_PROPERTY(ProgramDrawMode programDrawMode MEMBER m_programDrawMode NOTIFY changed);
    Q_PROPERTY(bool simplifyGeometry MEMBER m_simplifyGeometry NOTIFY changed);
    Q_PROPERTY(float simplifyGeometryPrecision MEMBER m_simplifyGeometryPrecision NOTIFY changed);
    Q_PROPERTY(bool grayscaleSegments MEMBER m_grayscaleSegments NOTIFY changed);
    Q_PROPERTY(bool grayscaleSegmentsBySCode MEMBER m_grayscaleSegmentsBySCode NOTIFY changed);
    Q_PROPERTY(bool grayscaleSegmentsByZCode MEMBER m_grayscaleSegmentsByZCode NOTIFY changed);
    Q_PROPERTY(float fieldOfView MEMBER m_fieldOfView NOTIFY changed);
    Q_PROPERTY(float nearPlane MEMBER m_nearPlane NOTIFY changed);
    Q_PROPERTY(float farPlane MEMBER m_farPlane NOTIFY changed);

    public:
        explicit ConfigurationVisualizer(QObject *parent);
        ConfigurationVisualizer& operator=(const ConfigurationVisualizer&) { return *this; }
        QString getSectionName() override { return "baseui.visualizer"; }

        enum ProgramDrawMode {
            Vectors,
            Raster
        };
        Q_ENUM(ProgramDrawMode);

        int lineWidth() const { return m_lineWidth; }
        int fpsLock() const { return m_fpsLock; }
        bool smoothing() const { return m_smoothing; }
        bool msaa() const { return m_msaa; }
        bool zBuffer() const { return m_zBuffer; }
        bool vsync() const { return m_vsync; }
        ProgramDrawMode programDrawMode() const { return m_programDrawMode; }
        bool simplifyGeometry() const { return m_simplifyGeometry; }
        float simplifyGeometryPrecision() const { return m_simplifyGeometryPrecision; }
        bool grayscaleSegments() const { return m_grayscaleSegments; }
        bool grayscaleSegmentsBySCode() const { return m_grayscaleSegmentsBySCode; }
        bool grayscaleSegmentsByZCode() const { return m_grayscaleSegmentsByZCode; }
        float fieldOfView() const { return m_fieldOfView; }
        float nearPlane() const { return m_nearPlane; }
        float farPlane() const { return m_farPlane; }

    private:
        int m_lineWidth;
        int m_fpsLock;
        bool m_smoothing;
        bool m_msaa;
        bool m_zBuffer;
        bool m_vsync;
        ProgramDrawMode m_programDrawMode;
        bool m_simplifyGeometry;
        float m_simplifyGeometryPrecision;
        bool m_grayscaleSegments;
        bool m_grayscaleSegmentsBySCode;
        bool m_grayscaleSegmentsByZCode;
        float m_fieldOfView;
        float m_nearPlane;
        float m_farPlane;
};

#endif // CONFIGURATION_VISUALIZER_H
