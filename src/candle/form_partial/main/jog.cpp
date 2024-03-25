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
    JoggingVector vector(0, 1, 0);
    emit this->jog(YPlus, vector);
}

void partMainJog::onCmdYPlusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdYMinusPressed()
{
    JoggingVector vector(0, -1, 0);
    emit this->jog(YMinus, vector);
}

void partMainJog::onCmdYMinusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdXPlusPressed()
{
    JoggingVector vector(1, 0, 0);
    emit this->jog(XPlus, vector);
}

void partMainJog::onCmdXPlusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdXMinusPressed()
{
    JoggingVector vector(-1, 0, 0);
    emit this->jog(XMinus, vector);
}

void partMainJog::onCmdXMinusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdZPlusPressed()
{
    JoggingVector vector(0, 0, 1);
    emit this->jog(ZPlus, vector);
}

void partMainJog::onCmdZPlusReleased()
{
    emit this->stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdZMinusPressed()
{
    JoggingVector vector(-1, 0, 0);
    emit this->jog(ZMinus, vector);
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
