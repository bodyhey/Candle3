#ifndef STATE_H
#define STATE_H

#include "globals.h"
#include <QWidget>
#include <QVector3D>

namespace Ui {
class State;
}

class partMainState : public QWidget
{
    Q_OBJECT

public:
    explicit partMainState(QWidget *parent = nullptr);
    ~partMainState();
    void setStatus(QString, QString bgColor, QString fgColor);
    void setWorldCoordinates(QVector3D);
    void setMachineCoordinates(QVector3D);

private:
    Ui::State *ui;

signals:
    void grblCommand(GRBLCommand command);

};

#endif // STATE_H
