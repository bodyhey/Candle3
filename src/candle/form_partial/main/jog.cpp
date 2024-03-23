#include "jog.h"
#include "ui_jog.h"

partMainJog::partMainJog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::widgetJog)
{
    ui->setupUi(this);
}

partMainJog::~partMainJog()
{
    delete ui;
}

void partMainJog::onCmdYPlusPressed()
{
    emit this->jog(YPlus);
}

void partMainJog::onCmdYPlusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdYMinusPressed()
{
    emit this->jog(YMinus);
}

void partMainJog::onCmdYMinusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdXPlusPressed()
{
    emit this->jog(XPlus);
}

void partMainJog::onCmdXPlusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdXMinusPressed()
{
    emit this->jog(XMinus);
}

void partMainJog::onCmdXMinusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdZPlusPressed()
{
    emit this->jog(ZPlus);
}

void partMainJog::onCmdZPlusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdZMinusPressed()
{
    emit this->jog(ZMinus);
}

void partMainJog::onCmdZMinusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdStopClicked()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onChkKeyboardControlToggled(bool checked)
{
}
