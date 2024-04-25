#include "partmainstate.h"
#include "ui_state.h"

partMainState::partMainState(QWidget *parent, const Configuration &configuration)
    : QWidget(parent)
    , ui(new Ui::partMainState)
    , m_configuration(configuration)
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

void partMainState::setUnits(Units units)
{
    int prec = units == Units::Millimeters ? 3 : 4;
    // int bound = m_settings->units() == 0 ? 9999 : 999;

    ui->txtMPosX->setDecimals(prec);
    // @TODO what are these for?
    // ui->txtMPosX->setMinimum(-bound);
    // ui->txtMPosX->setMaximum(bound);
    ui->txtMPosY->setDecimals(prec);
    // @TODO what are these for?
    // ui->txtMPosY->setMinimum(-bound);
    // ui->txtMPosY->setMaximum(bound);
    ui->txtMPosZ->setDecimals(prec);
    // ui->txtMPosZ->setMinimum(-bound);
    // ui->txtMPosZ->setMaximum(bound);

    ui->txtWPosX->setDecimals(prec);
    // ui->txtWPosX->setMinimum(-bound);
    // ui->txtWPosX->setMaximum(bound);
    ui->txtWPosY->setDecimals(prec);
    // ui->txtWPosY->setMinimum(-bound);
    // ui->txtWPosY->setMaximum(bound);
    ui->txtWPosZ->setDecimals(prec);
    // ui->txtWPosZ->setMinimum(-bound);
    // ui->txtWPosZ->setMaximum(bound);
}
