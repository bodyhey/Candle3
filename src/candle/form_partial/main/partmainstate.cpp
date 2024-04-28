#include "partmainstate.h"
#include "ui_partmainstate.h"

partMainState::partMainState(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::partMainState)
    // , m_configuration(configuration)
{
    ui->setupUi(this);

    initializeColorsAndCaptions();
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
    setStatusText(m_statusCaptions[state], m_statusBackColors[state], m_statusForeColors[state]);
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

void partMainState::initializeColorsAndCaptions()
{
    m_statusCaptions[DeviceUnknown] = tr("Unknown");
    m_statusCaptions[DeviceIdle] = tr("Idle");
    m_statusCaptions[DeviceAlarm] = tr("Alarm");
    m_statusCaptions[DeviceRun] = tr("Run");
    m_statusCaptions[DeviceHome] = tr("Home");
    m_statusCaptions[DeviceHold0] = tr("Hold") + " (0)";
    m_statusCaptions[DeviceHold1] = tr("Hold") + " (1)";
    m_statusCaptions[DeviceQueue] = tr("Queue");
    m_statusCaptions[DeviceCheck] = tr("Check");
    m_statusCaptions[DeviceDoor0] = tr("Door") + " (0)";
    m_statusCaptions[DeviceDoor1] = tr("Door") + " (1)";
    m_statusCaptions[DeviceDoor2] = tr("Door") + " (2)";
    m_statusCaptions[DeviceDoor3] = tr("Door") + " (3)";
    m_statusCaptions[DeviceJog] = tr("Jog");
    m_statusCaptions[DeviceSleep] = tr("Sleep");

    m_statusBackColors[DeviceUnknown] = "red";
    m_statusBackColors[DeviceIdle] = "palette(button)";
    m_statusBackColors[DeviceAlarm] = "red";
    m_statusBackColors[DeviceRun] = "lime";
    m_statusBackColors[DeviceHome] = "lime";
    m_statusBackColors[DeviceHold0] = "yellow";
    m_statusBackColors[DeviceHold1] = "yellow";
    m_statusBackColors[DeviceQueue] = "yellow";
    m_statusBackColors[DeviceCheck] = "palette(button)";
    m_statusBackColors[DeviceDoor0] = "red";
    m_statusBackColors[DeviceDoor1] = "red";
    m_statusBackColors[DeviceDoor2] = "red";
    m_statusBackColors[DeviceDoor3] = "red";
    m_statusBackColors[DeviceJog] = "lime";
    m_statusBackColors[DeviceSleep] = "blue";

    m_statusForeColors[DeviceUnknown] = "white";
    m_statusForeColors[DeviceIdle] = "palette(text)";
    m_statusForeColors[DeviceAlarm] = "white";
    m_statusForeColors[DeviceRun] = "black";
    m_statusForeColors[DeviceHome] = "black";
    m_statusForeColors[DeviceHold0] = "black";
    m_statusForeColors[DeviceHold1] = "black";
    m_statusForeColors[DeviceQueue] = "black";
    m_statusForeColors[DeviceCheck] = "palette(text)";
    m_statusForeColors[DeviceDoor0] = "white";
    m_statusForeColors[DeviceDoor1] = "white";
    m_statusForeColors[DeviceDoor2] = "white";
    m_statusForeColors[DeviceDoor3] = "white";
    m_statusForeColors[DeviceJog] = "black";
    m_statusForeColors[DeviceSleep] = "white";
}
