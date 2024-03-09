#ifndef CONTROL_H
#define CONTROL_H

#include "globals.h"
#include <QWidget>

namespace Ui {
class partMainControl;
}

class partMainControl : public QWidget
{
    Q_OBJECT

public:
    explicit partMainControl(QWidget *parent = nullptr);
    ~partMainControl();
    void enable();
    void disable();

signals:
    void home();
    void reset();
    void unlock();
    void sleep();
    void door();
    void command(GRBLCommand command);

private:
    Ui::partMainControl *ui;

private slots:
    void onCmdHomeClicked();
    void onCmdCheckClicked(bool checked);
    void onCmdResetClicked();
    void onCmdUnlockClicked();
    void onCmdHoldClicked(bool checked);
    void onCmdSleepClicked();
    void onCmdDoorClicked();
    void onFloodClicked(bool checked);

};

#endif // CONTROL_H
