// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef CONFIGURATIONMACHINE_H
#define CONFIGURATIONMACHINE_H

#include <QObject>
#include "module.h"

class ConfigurationMachine : public ConfigurationModule
{
    friend class frmSettings;

    Q_OBJECT
    Q_PROPERTY(MinMax spindleSpeedRange READ spindleSpeedRange CONSTANT)
    Q_PROPERTY(MinMax laserPowerRange READ laserPowerRange CONSTANT)
    Q_PROPERTY(ReferencePositionDir referencePositionDirX READ referencePositionDirX CONSTANT)
    Q_PROPERTY(ReferencePositionDir referencePositionDirY READ referencePositionDirY CONSTANT)
    Q_PROPERTY(ReferencePositionDir referencePositionDirZ READ referencePositionDirZ CONSTANT)

    public:
        ConfigurationMachine(QObject *parent);
        QString getSectionName() override { return "machine"; }

        enum ReferencePositionDir {
            Negative,
            Positive
        };
        Q_ENUM(ReferencePositionDir);

        MinMax spindleSpeedRange() const { return {m_spindleSpeedMin, m_spindleSpeedMax}; }
        int spindleSpeedMin() const { return m_spindleSpeedMin; }
        int spindleSpeedMax() const { return m_spindleSpeedMax; }
        MinMax laserPowerRange() const { return {m_laserPowerMin, m_laserPowerMax}; }
        int laserPowerMin() const { return m_laserPowerMin; }
        int laserPowerMax() const { return m_laserPowerMax; }
        ReferencePositionDir referencePositionDirX() const { return m_referencePositionDirX; }
        ReferencePositionDir referencePositionDirY() const { return m_referencePositionDirY; }
        ReferencePositionDir referencePositionDirZ() const { return m_referencePositionDirZ; }

    private:
        int m_spindleSpeedMin;
        int m_spindleSpeedMax;
        int m_laserPowerMin;
        int m_laserPowerMax;

        ReferencePositionDir m_referencePositionDirX;
        ReferencePositionDir m_referencePositionDirY;
        ReferencePositionDir m_referencePositionDirZ;
};

#endif // CONFIGURATIONMACHINE_H
