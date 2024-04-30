// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef PARTMAINCONTROL_H
#define PARTMAINCONTROL_H

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
    void updateControlsState(bool portOpened, bool process);
    void updateControlsState(SenderState senderState, DeviceState deviceState);
    bool hold();
    void setFlood(bool);

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

#endif // PARTMAINCONTROL_H
