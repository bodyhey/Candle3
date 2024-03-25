#ifndef MACHINECONFIGURATION_H
#define MACHINECONFIGURATION_H

#include <QVector3D>
#include <QMap>
#include "../globals.h"

class MachineConfiguration
{
    public:
        MachineConfiguration(QMap<int, double> settings);
        bool homingEnabled() { return m_homingEnabled; }
        int axisCount() { return m_axisCount; }
        Units units() { return m_units; }

        // void setHomingEnabled(bool);
        // void setUnits(int);
        // void setSoftLimitsEnabled(bool);
        // void setRapidSpeed(float);
        // void setAcceleration(float);
        // bool referenceXPlus();
        // bool referenceZPlus();
        // bool referenceYPlus();
        // void setMachineBounds(QVector3D);
        // bool toolChangePause();
        // bool ignoreErrors();
        // QString beforePauseCommands();
        // QString toolChangeCommands();

    private:
        bool m_homingEnabled = false;
        int m_axisCount = 0;
        Units m_units;
        bool m_softLimitsEnabled = false;
        bool m_hardLimitsEnabled = false;
        bool m_laserMode = false;
        double m_homingPullOff = 0;
        float m_rapidSpeed = 0;
        QVector3D m_stepsPerMM;
        QVector3D m_maxTravel;
        QVector3D m_acceleration;
        QVector3D m_maxRate;
        Units convertUnits(int setting);
};

#endif // MACHINECONFIGURATION_H
