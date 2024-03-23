#include "control.h"
#include "ui_control.h"
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
