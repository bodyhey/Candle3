// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

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
    void onCmdFloodClicked(bool checked);

};

#endif // CONTROL_H
