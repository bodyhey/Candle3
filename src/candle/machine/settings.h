#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVector3D>

class Settings
{
public:
    Settings();
    void setHomingEnabled(bool);
    void setUnits(int);
    void setSoftLimitsEnabled(bool);
    void setRapidSpeed(float);
    void setAcceleration(float);
    bool referenceXPlus();
    bool referenceZPlus();
    bool referenceYPlus();
    void setMachineBounds(QVector3D);
    bool toolChangePause();
    bool ignoreErrors();
    QString beforePauseCommands();
    QString toolChangeCommands();
};

#endif // SETTINGS_H
