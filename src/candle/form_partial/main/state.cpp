#include "state.h"
#include "ui_state.h"

partMainState::partMainState(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::State)
{
    ui->setupUi(this);
}

partMainState::~partMainState()
{
    delete ui;
}

void partMainState::setStatusText(QString status, QString bgColor, QString fgColor)
{
    ui->txtStatus->setText(status);
    ui->txtStatus->setStyleSheet(QString("background-color: %1; color: %2;")
                                     .arg(bgColor, fgColor));

}

void partMainState::setState(DeviceState state)
{
    // @TODO
}

void partMainState::setWorkCoordinates(QVector3D pos)
{
    ui->txtWPosX->setValue(pos.x());
    ui->txtWPosY->setValue(pos.y());
    ui->txtWPosZ->setValue(pos.z());
}

void partMainState::setMachineCoordinates(QVector3D pos)
{
    ui->txtMPosX->setValue(pos.x());
    ui->txtMPosY->setValue(pos.y());
    ui->txtMPosZ->setValue(pos.z());
}
