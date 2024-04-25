#ifndef PARTMAINSTATE_H
#define PARTMAINSTATE_H

#include "globals.h"
#include "../../config/configuration.h"
#include <QWidget>
#include <QVector3D>

namespace Ui {
class partMainState;
}

class partMainState : public QWidget
{
    Q_OBJECT

    public:
        explicit partMainState(QWidget *parent, const Configuration &configuration);
        ~partMainState();
        void setStatusText(QString, QString bgColor, QString fgColor);
        void setState(DeviceState);
        void setWorkCoordinates(QVector3D);
        void setMachineCoordinates(QVector3D);
        void setUnits(Units units);

    private:
        Ui::partMainState *ui;
        const Configuration &m_configuration;

    signals:
        void grblCommand(GRBLCommand command);

};

#endif // PARTMAINSTATE_H
