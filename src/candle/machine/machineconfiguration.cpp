// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "machineconfiguration.h"

MachineConfiguration::MachineConfiguration(QMap<int, double> settings, ConfigurationMachine &configuration)
{
    if (settings.contains(13)) m_units = setUnits(settings[13]);
    if (settings.contains(20)) m_softLimitsEnabled = (bool)settings[20];
    if (settings.contains(21)) m_hardLimitsEnabled = (bool)settings[21];
    if (settings.contains(22)) m_homingEnabled = (bool)settings[22];
    if (settings.contains(27)) m_homingPullOff = settings[27];
    if (settings.contains(32)) m_laserMode = (bool)settings[32];

    if (settings.contains(110)) {
        m_axisCount++;
        m_stepsPerMM.setX(settings[100]);
        m_maxRate.setX(settings[110]);
        m_acceleration.setX(settings[120]);
        m_maxTravel.setX(settings[130]);
        m_machineBounds.setX(configuration.referencePositionDirX() ? -settings[130] : settings[130]);
    }
    if (settings.contains(111)) {
        m_axisCount++;
        m_stepsPerMM.setY(settings[101]);
        m_maxRate.setY(settings[111]);
        m_acceleration.setY(settings[121]);
        m_maxTravel.setY(settings[131]);
        m_machineBounds.setY(configuration.referencePositionDirY() ? -settings[131] : settings[131]);
    }
    if (settings.contains(112)) {
        m_axisCount++;
        m_stepsPerMM.setZ(settings[102]);
        m_maxRate.setZ(settings[112]);
        m_acceleration.setZ(settings[122]);
        m_maxTravel.setZ(settings[132]);
        m_machineBounds.setZ(configuration.referencePositionDirZ() ? -settings[132] : settings[132]);
    }
}

Units MachineConfiguration::setUnits(int setting)
{
    if (setting == 1) return Units::Inches;

    return Units::Millimeters;
}
