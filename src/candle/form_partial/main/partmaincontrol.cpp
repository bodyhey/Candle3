#include "partmaincontrol.h"
#include "ui_partmaincontrol.h"
#include <QDebug>

partMainControl::partMainControl(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::partMainControl)
{
    ui->setupUi(this);
}

partMainControl::~partMainControl()
{
    delete ui;
}

void partMainControl::enable()
{

}

void partMainControl::disable()
{

}

void partMainControl::updateControlsState(bool portOpened, bool process)
{
    // ui->cmdCheck->setEnabled(portOpened && !process);
    // ui->cmdHome->setEnabled(!process);
    // ui->cmdCheck->setEnabled(!process);
    // ui->cmdUnlock->setEnabled(!process);
    // //ui->cmdSpindle->setEnabled(!process);
    // ui->cmdSleep->setEnabled(!process);
}

void partMainControl::updateControlsState(SenderState senderState, DeviceState deviceState)
{
    ui->cmdCheck->setEnabled(deviceState != DeviceRun && (senderState == SenderStopped));
    ui->cmdCheck->setChecked(deviceState == DeviceCheck);
    ui->cmdHold->setChecked(deviceState == DeviceHold0 || deviceState == DeviceHold1 || deviceState == DeviceQueue);
}

bool partMainControl::hold()
{
    return ui->cmdHold->isChecked();
}

void partMainControl::setFlood(bool state)
{
    ui->cmdFlood->setChecked(state);
}

void partMainControl::onCmdHomeClicked()
{
    emit this->home();
    emit this->command(GRBLCommand::Home);
}

void partMainControl::onCmdCheckClicked(bool checked)
{

}

void partMainControl::onCmdResetClicked()
{
    emit this->reset();
    emit this->command(GRBLCommand::Reset);
}

void partMainControl::onCmdUnlockClicked()
{
    emit this->unlock();
    emit this->command(GRBLCommand::Unlock);
}

void partMainControl::onCmdHoldClicked(bool checked)
{
    qDebug() << "Hold" << checked;
}

void partMainControl::onCmdSleepClicked()
{
}

void partMainControl::onCmdDoorClicked()
{
}

void partMainControl::onCmdFloodClicked(bool checked)
{
}
