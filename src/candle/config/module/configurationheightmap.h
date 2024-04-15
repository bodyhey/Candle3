// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATIONHEIGHTMAP_H
#define CONFIGURATIONHEIGHTMAP_H

#include "configurationmodule.h"

class ConfigurationHeightmap : public ConfigurationModule
{
    Q_OBJECT
    Q_PROPERTY(double heightmapBorderX MEMBER m_heightmapBorderX NOTIFY changed)
    Q_PROPERTY(double heightmapBorderY MEMBER m_heightmapBorderY NOTIFY changed)
    Q_PROPERTY(double heightmapBorderWidth MEMBER m_heightmapBorderWidth NOTIFY changed)
    Q_PROPERTY(double heightmapBorderHeight MEMBER m_heightmapBorderHeight NOTIFY changed)
    Q_PROPERTY(bool heightmapBorderShow MEMBER m_heightmapBorderShow NOTIFY changed)
    Q_PROPERTY(double heightmapGridX MEMBER m_heightmapGridX NOTIFY changed)
    Q_PROPERTY(double heightmapGridY MEMBER m_heightmapGridY NOTIFY changed)
    Q_PROPERTY(double heightmapGridZTop MEMBER m_heightmapGridZTop NOTIFY changed)
    Q_PROPERTY(double heightmapGridZBottom MEMBER m_heightmapGridZBottom NOTIFY changed)
    Q_PROPERTY(double heightmapProbeFeed MEMBER m_heightmapProbeFeed NOTIFY changed)
    Q_PROPERTY(bool heightmapGridShow MEMBER m_heightmapGridShow NOTIFY changed)
    Q_PROPERTY(double heightmapInterpolationStepX MEMBER m_heightmapInterpolationStepX NOTIFY changed)
    Q_PROPERTY(double heightmapInterpolationStepY MEMBER m_heightmapInterpolationStepY NOTIFY changed)
    Q_PROPERTY(int heightmapInterpolationType MEMBER m_heightmapInterpolationType NOTIFY changed)
    Q_PROPERTY(bool heightmapInterpolationShow MEMBER m_heightmapInterpolationShow NOTIFY changed)

    public:
        ConfigurationHeightmap(QObject *parent);

        QString getSectionName() override { return "heightmap"; }

        double heightmapBorderX() const { return m_heightmapBorderX; }
        double heightmapBorderY() const { return m_heightmapBorderY; }
        double heightmapBorderWidth() const { return m_heightmapBorderWidth; }
        double heightmapBorderHeight() const { return m_heightmapBorderHeight; }
        bool heightmapBorderShow() const { return m_heightmapBorderShow; }

        double heightmapGridX() const { return m_heightmapGridX; }
        double heightmapGridY() const { return m_heightmapGridY; }
        double heightmapGridZTop() const { return m_heightmapGridZTop; }
        double heightmapGridZBottom() const { return m_heightmapGridZBottom; }
        double heightmapProbeFeed() const { return m_heightmapProbeFeed; }
        bool heightmapGridShow() const { return m_heightmapGridShow; }

        double heightmapInterpolationStepX() const { return m_heightmapInterpolationStepX; }
        double heightmapInterpolationStepY() const { return m_heightmapInterpolationStepY; }
        int heightmapInterpolationType() const { return m_heightmapInterpolationType; }
        bool heightmapInterpolationShow() const { return m_heightmapInterpolationShow; }

    private:
        double m_heightmapBorderX;
        double m_heightmapBorderY;
        double m_heightmapBorderWidth;
        double m_heightmapBorderHeight;
        bool m_heightmapBorderShow;

        double m_heightmapGridX;
        double m_heightmapGridY;
        double m_heightmapGridZTop;
        double m_heightmapGridZBottom;
        double m_heightmapProbeFeed;
        bool m_heightmapGridShow;

        double m_heightmapInterpolationStepX;
        double m_heightmapInterpolationStepY;
        int m_heightmapInterpolationType;
        bool m_heightmapInterpolationShow;
};

#endif // CONFIGURATIONHEIGHTMAP_H
