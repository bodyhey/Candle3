#include "partmainjog.h"
#include "ui_jog.h"

void partMainJog::updateControls()
{
    ui->cboJogStep->setEditable(!ui->chkKeyboardControl->isChecked());
    ui->cboJogFeed->setEditable(!ui->chkKeyboardControl->isChecked());
    ui->cboJogStep->setEnabled(!ui->chkKeyboardControl->isChecked());
    ui->cboJogFeed->setEnabled(!ui->chkKeyboardControl->isChecked());
    //ui->cboJogStep->setStyleSheet(QString("font-size: %1").arg(m_configuration.uiModule().fontSize()));
    ui->cboJogFeed->setStyleSheet(ui->cboJogStep->styleSheet());
}

partMainJog::partMainJog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::partMainJog)
{
    ui->setupUi(this);

    QColor backgroundColor = QColor(153, 180, 209);

    ui->cmdXMinus->setBackColor(backgroundColor);
    ui->cmdXPlus->setBackColor(backgroundColor);
    ui->cmdYMinus->setBackColor(backgroundColor);
    ui->cmdYPlus->setBackColor(backgroundColor);
}

void partMainJog::configurationUpdated()
{
    m_storedKeyboardControl = m_configurationJogging->keyboardControl();

    QStringList steps = m_configurationJogging->stepChoices();
    if (steps.count() > 0) {
        //steps.insert(0, ui->cboJogStep->items().first());
        ui->cboJogStep->setItems(steps);
    }
    // ui->cboJogStep->setCurrentIndex(ui->cboJogStep->findText(set.value("jogStep").toString()));
    ui->cboJogFeed->setItems(m_configurationJogging->feedChoices());
    // ui->cboJogFeed->setCurrentIndex(ui->cboJogFeed->findText(set.value("jogFeed").toString()));
}

void partMainJog::restoreKeyboardControl()
{
    ui->chkKeyboardControl->setChecked(m_storedKeyboardControl);
}

void partMainJog::initialize(ConfigurationJogging &configurationJogging)
{
    m_configurationJogging = &configurationJogging;
    configurationUpdated();
}

partMainJog::~partMainJog()
{
    delete ui;
}

void partMainJog::storeAndResetKeyboardControl()
{
    m_storedKeyboardControl = ui->chkKeyboardControl->isChecked();
    ui->chkKeyboardControl->setChecked(false);
}

bool partMainJog::keyboardControl()
{
    return ui->chkKeyboardControl->isChecked();
}

void partMainJog::setKeyboardControl(bool value)
{
    ui->chkKeyboardControl->setChecked(value);
}

void partMainJog::onCmdYPlusPressed()
{
    m_jogVector = JoggingVector(0, 1, 0);
    emit this->jog(YPlus, m_jogVector);
}

void partMainJog::stopJogging()
{
    m_jogVector = QVector3D(0, 0, 0);
    emit stop();
    emit this->command(GRBLCommand::JogStop);
}

void partMainJog::onCmdYPlusReleased()
{
    stopJogging();
}

void partMainJog::onCmdYMinusPressed()
{
    m_jogVector = JoggingVector(0, -1, 0);
    emit this->jog(YMinus, m_jogVector);
}

void partMainJog::onCmdYMinusReleased()
{
    stopJogging();
}

void partMainJog::onCmdXPlusPressed()
{
    m_jogVector = JoggingVector(1, 0, 0);
    emit this->jog(XPlus, m_jogVector);
}

void partMainJog::onCmdXPlusReleased()
{
    stopJogging();
}

void partMainJog::onCmdXMinusPressed()
{
    m_jogVector = JoggingVector(-1, 0, 0);
    emit this->jog(XMinus, m_jogVector);
}

void partMainJog::onCmdXMinusReleased()
{
    stopJogging();
}

void partMainJog::onCmdZPlusPressed()
{
    m_jogVector = JoggingVector(0, 0, 1);
    emit this->jog(ZPlus, m_jogVector);
}

void partMainJog::onCmdZPlusReleased()
{
    stopJogging();
}

void partMainJog::onCmdZMinusPressed()
{
    m_jogVector = JoggingVector(-1, 0, 0);
    emit this->jog(ZMinus, m_jogVector);
}

void partMainJog::onCmdZMinusReleased()
{
    stopJogging();
}

void partMainJog::onCmdStopClicked()
{
    stopJogging();
}

void partMainJog::onCmdFeedRateChanged(int index)
{
    // joggingConfiguration.setJogStep(ui->cboJogStep->currentText().toDouble());
    // joggingConfiguration.setJogFeed(ui->cboJogFeed->currentText().toInt());

    qDebug() << "Feed rate changed" << index << m_configurationJogging->feedChoices().at(index);
}

void partMainJog::onCmdStepSizeChanged(int index)
{
    qDebug() << "Step size changed" << index << m_configurationJogging->stepChoices().at(index);
}
