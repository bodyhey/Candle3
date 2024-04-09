// This file is a part of "Candle" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich

#include "frmsettings.h"
#include "ui_frmsettings.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QScrollBar>
#include <QColorDialog>
#include <QStyledItemDelegate>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QKeySequenceEdit>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDir>
#include <QLocale>

class CustomKeySequenceEdit : public QKeySequenceEdit
{
public:
    explicit CustomKeySequenceEdit(QWidget *parent = 0): QKeySequenceEdit(parent) {}
    ~CustomKeySequenceEdit() {}

protected:
    void keyPressEvent(QKeyEvent *pEvent) {
        QKeySequenceEdit::keyPressEvent(pEvent);
        QString s = keySequence().toString().split(", ").first();

        QString shiftedKeys = "~!@#$%^&*()_+{}|:?><\"";
        QString key = s.right(1);
        
        if (pEvent->modifiers() & Qt::KeypadModifier) s = "Num+" + s;
        else if (!key.isEmpty() && shiftedKeys.contains(key)) {
            s.remove("Shift+");
            s = s.left(s.size() - 1) + QString("Shift+%1").arg(key);
        }

        QKeySequence seq(QKeySequence::fromString(s));
        setKeySequence(seq);
    }
};

class ShortcutDelegate: public QStyledItemDelegate
{
public:
    ShortcutDelegate() {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(option);
        Q_UNUSED(index);

        return new CustomKeySequenceEdit(parent);
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        static_cast<QKeySequenceEdit*>(editor)->setKeySequence(index.data(Qt::DisplayRole).toString());
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override
    {
        model->setData(index, static_cast<QKeySequenceEdit*>(editor)->keySequence().toString());
    }
};

frmSettings::frmSettings(QWidget *parent, Configuration &configuration) :
    QDialog(parent),
    ui(new Ui::frmSettings),
    m_configuration(configuration)
{
    ui->setupUi(this);

    this->setLocale(QLocale::C);
    m_intValidator.setBottom(1);
    m_intValidator.setTop(999);
    ui->cboFps->setValidator(&m_intValidator);
    ui->cboFontSize->setValidator(&m_intValidator);

    foreach (QGroupBox *box, this->findChildren<QGroupBox*>()) {
        ui->listCategories->addItem(box->title());
        ui->listCategories->item(ui->listCategories->count() - 1)->setData(Qt::UserRole, box->objectName());
    }

    ui->listCategories->item(0)->setSelected(true);
    connect(ui->scrollSettings->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onScrollBarValueChanged(int)));
    connect(this, SIGNAL(settingsSetToDefault()), parent, SIGNAL(settingsSetToDefault()));

    // Shortcuts table
    ui->tblShortcuts->setItemDelegateForColumn(2, new ShortcutDelegate);
    ui->tblShortcuts->setTabKeyNavigation(false);
    ui->tblShortcuts->setEditTriggers(QAbstractItemView::AllEditTriggers);

    searchForSerialPorts();

    // Languages
    QDir d(qApp->applicationDirPath() + "/translations");
    QStringList fl = QStringList() << "candle_*.qm";
    QStringList tl = d.entryList(fl, QDir::Files);
    QRegExp fx("_([^\\.]+)");
    foreach (const QString &t, tl) {
        if (fx.indexIn(t) != -1) {
            QLocale l(fx.cap(1));
            ui->cboLanguage->addItem(l.nativeLanguageName(), l.name().left(2));
        }
    }

    // Connection mode
    ui->frameConnectionRawSocket->hide();
    ui->frameConnectionSimulator->hide();
    connect(ui->cboConnectionMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onConnectionModeChanged(int)));

    m_animatingScrollBox = false;
    m_scrollingManuallyScrollBox = false;
}

frmSettings::~frmSettings()
{
    delete ui;
}

void frmSettings::initializeWidgets()
{
    const ConfigurationConsole &console = m_configuration.consoleModule();
    ui->chkConsoleAutocompletion->setChecked(console.commandAutoCompletion());
    ui->chkConsoleDarkMode->setChecked(console.darkBackgroundMode());
    ui->chkConsoleShowProgramCommands->setChecked(console.showProgramCommands());
    ui->chkConsoleShowUICommands->setChecked(console.showUiCommands());

    const ConfigurationConnection &connection = m_configuration.connectionModule();
    ui->cboConnectionMode->setCurrentIndex(connection.connectionMode());
    ui->txtQueryStateInterval->setValue(connection.queryStateInterval());
    ui->cboSerialPort->setCurrentText(connection.serialPort());
    ui->cboSerialBaud->setCurrentText(QString::number(connection.serialBaud()));
    ui->txtRawTcpHost->setText(connection.rawTcpHost());
    ui->txtRawTcpPort->setText(QString::number(connection.rawTcpPort()));
}

void frmSettings::applySettings()
{
    ConfigurationConsole &console = m_configuration.consoleModule();
    console.m_commandAutoCompletion = ui->chkConsoleAutocompletion->isChecked();
    console.m_darkBackgroundMode = ui->chkConsoleDarkMode->isChecked();
    console.m_showProgramCommands = ui->chkConsoleShowProgramCommands->isChecked();
    console.m_showUiCommands = ui->chkConsoleShowUICommands->isChecked();

    ConfigurationConnection &connection = m_configuration.connectionModule();
    connection.m_connectionMode = static_cast<ConnectionMode>(ui->cboConnectionMode->currentIndex());
    connection.m_queryStateInterval = ui->txtQueryStateInterval->value();
    connection.m_serialPort = ui->cboSerialPort->currentText();
    connection.m_serialBaud = ui->cboSerialBaud->currentText().toInt();
    connection.m_rawTcpHost = ui->txtRawTcpHost->text();
    connection.m_rawTcpPort = ui->txtRawTcpPort->text().toInt();
}

int frmSettings::exec()
{
    // Store settings to undo
    m_storedValues.clear();
    m_storedChecks.clear();
    m_storedCombos.clear();
    m_storedColors.clear();
    m_storedTextBoxes.clear();
    m_storedPlainTexts.clear();

    foreach (QAbstractSpinBox* o, this->findChildren<QAbstractSpinBox*>())
        m_storedValues.append(o->property("value").toDouble());

    foreach (QAbstractButton* o, this->findChildren<QAbstractButton*>())
        m_storedChecks.append(o->isChecked());

    foreach (QComboBox* o, this->findChildren<QComboBox*>())
        m_storedCombos.append(o->currentText());

    foreach (ColorPicker* o, this->findChildren<ColorPicker*>())
        m_storedColors.append(o->color());

    foreach (QLineEdit* o, this->findChildren<QLineEdit*>())
        m_storedTextBoxes.append(o->text());

    foreach (QPlainTextEdit* o, this->findChildren<QPlainTextEdit*>())
        m_storedPlainTexts.append(o->toPlainText());

    initializeWidgets();

    return QDialog::exec();
}

void frmSettings::undo()
{
    foreach (QAbstractSpinBox* o, this->findChildren<QAbstractSpinBox*>())
        o->setProperty("value", m_storedValues.takeFirst());

    foreach (QAbstractButton* o, this->findChildren<QAbstractButton*>())
        o->setChecked(m_storedChecks.takeFirst());

    foreach (QComboBox* o, this->findChildren<QComboBox*>())
        o->setCurrentText(m_storedCombos.takeFirst());

    foreach (ColorPicker* o, this->findChildren<ColorPicker*>())
        o->setColor(m_storedColors.takeFirst());

    foreach (QLineEdit* o, this->findChildren<QLineEdit*>())
        o->setText(m_storedTextBoxes.takeFirst());

    foreach (QPlainTextEdit* o, this->findChildren<QPlainTextEdit*>())
        o->setPlainText(m_storedPlainTexts.takeFirst());
}

void frmSettings::addCustomSettings(QGroupBox *box)
{
    static_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout())->addWidget(box);
    
    ui->listCategories->addItem(box->title());
    ui->listCategories->item(ui->listCategories->count() - 1)->setData(Qt::UserRole, box->objectName());

    m_customSettings.append(box);
}

void frmSettings::on_listCategories_currentRowChanged(int currentRow)
{
    static QPropertyAnimation *animation;

    if (animation) {
        animation->stop();
        animation = nullptr;
    }

    if (m_scrollingManuallyScrollBox) {
        return;
    }

    // Scroll to selected groupbox
    QGroupBox *box = this->findChild<QGroupBox*>(ui->listCategories->item(currentRow)->data(Qt::UserRole).toString());
    if (!box) {
        return;
    }

    QPoint labelPos = box->mapToParent(QPoint(0, 0));
    animation = new QPropertyAnimation(ui->scrollSettings->verticalScrollBar(), "value");
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setEndValue(labelPos.y() - 5);


    QObject::connect(animation, &QPropertyAnimation::finished, [=]() {
        animation = nullptr;
        m_animatingScrollBox = false;
    });

    m_animatingScrollBox = true;
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void frmSettings::onScrollBarValueChanged(int value)
{
    Q_UNUSED(value)

    if (m_animatingScrollBox) {
        return;
    }

    // Search for first full visible groupbox
    for (int i = 0; i < ui->listCategories->count(); i++) {
        QGroupBox *box = this->findChild<QGroupBox*>(ui->listCategories->item(i)->data(Qt::UserRole).toString());
        if (box) {
            if (!box->visibleRegion().isEmpty() && box->visibleRegion().boundingRect().y() == 0) {
                m_scrollingManuallyScrollBox = true; // stop on_listCategories_currentRowChanged animations
                // Select corresponding item in categories list
                ui->listCategories->setCurrentRow(i);
                qApp->processEvents();
                m_scrollingManuallyScrollBox = false;
                return;
            }
        }
    }
}

double frmSettings::toolDiameter()
{
    return ui->txtToolDiameter->value();
}

void frmSettings::setToolDiameter(double diameter)
{
    ui->txtToolDiameter->setValue(diameter);
}

double frmSettings::toolLength()
{
    return ui->txtToolLength->value();
}

void frmSettings::setToolLength(double length)
{
    ui->txtToolLength->setValue(length);
}

bool frmSettings::antialiasing()
{
    return ui->chkAntialiasing->isChecked();
}

void frmSettings::setAntialiasing(bool antialiasing)
{
    ui->chkAntialiasing->setChecked(antialiasing);
}

bool frmSettings::zBuffer()
{
    return ui->chkZBuffer->isChecked();
}

void frmSettings::setZBuffer(bool zBuffer)
{
    ui->chkZBuffer->setChecked(zBuffer);
}

double frmSettings::fov() {
    return ui->txtFov->value();
}

void frmSettings::setFov(double fov) {
    ui->txtFov->setValue(fov);
}

double frmSettings::nearPlane() {
    return ui->txtNear->value();
}

void frmSettings::setNearPlane(double near) {
    ui->txtNear->setValue(near);
}

double frmSettings::farPlane() {
    return ui->txtFar->value();
}

void frmSettings::setFarPlane(double far) {
    ui->txtFar->setValue(far);
}

double frmSettings::lineWidth()
{
    return ui->txtLineWidth->value();
}

void frmSettings::setLineWidth(double lineWidth)
{
    ui->txtLineWidth->setValue(lineWidth);
}

double frmSettings::arcLength()
{
    return ui->txtArcLength->value();
}

void frmSettings::setArcLength(double arcPrecision)
{
    ui->txtArcLength->setValue(arcPrecision);
}

double frmSettings::arcDegree()
{
    return ui->txtArcDegree->value();
}

void frmSettings::setArcDegree(double arcDegree)
{
    ui->txtArcDegree->setValue(arcDegree);
}

double frmSettings::arcPrecision()
{
    return ui->radArcDegreeMode->isChecked() ? ui->txtArcDegree->value() : ui->txtArcLength->value();
}

bool frmSettings::arcDegreeMode()
{
    return ui->radArcDegreeMode->isChecked();
}

void frmSettings::setArcDegreeMode(bool arcDegreeMode)
{
    ui->radArcDegreeMode->setChecked(arcDegreeMode);
}

void frmSettings::setShowProgramCommands(bool showAllCommands)
{
    ui->chkConsoleShowProgramCommands->setChecked(showAllCommands);
}

void frmSettings::setShowUICommands(bool showUICommands)
{
    ui->chkConsoleShowUICommands->setChecked(showUICommands);
}

int frmSettings::spindleSpeedMin()
{
    return ui->txtSpindleSpeedMin->value();
}

void frmSettings::setSpindleSpeedMin(int speed)
{
    ui->txtSpindleSpeedMin->setValue(speed);
}

int frmSettings::spindleSpeedMax()
{
    return ui->txtSpindleSpeedMax->value();
}

void frmSettings::setSpindleSpeedMax(int speed)
{
    ui->txtSpindleSpeedMax->setValue(speed);
}

int frmSettings::laserPowerMin()
{
    return ui->txtLaserPowerMin->value();
}

void frmSettings::setLaserPowerMin(int value)
{
    ui->txtLaserPowerMin->setValue(value);
}

int frmSettings::laserPowerMax()
{
    return ui->txtLaserPowerMax->value();
}

void frmSettings::setLaserPowerMax(int value)
{
    ui->txtLaserPowerMax->setValue(value);
}

int frmSettings::rapidSpeed()
{
    return m_rapidSpeed;
}

void frmSettings::setRapidSpeed(int rapidSpeed)
{
    m_rapidSpeed = rapidSpeed;
}

int frmSettings::acceleration()
{
    return m_acceleration;
}

void frmSettings::setAcceleration(int acceleration)
{
    m_acceleration = acceleration;
}

int frmSettings::toolType()
{
    return ui->cboToolType->currentIndex();
}

void frmSettings::setToolType(int toolType)
{
    ui->cboToolType->setCurrentIndex(toolType);
}

double frmSettings::toolAngle()
{
    return ui->txtToolAngle->value();
}

void frmSettings::setToolAngle(double toolAngle)
{
    ui->txtToolAngle->setValue(toolAngle);
}

int frmSettings::fps()
{
    return ui->cboFps->currentText().toInt();
}

void frmSettings::setFps(int fps)
{
    ui->cboFps->setCurrentText(QString::number(fps));
}

bool frmSettings::vsync()
{
    return ui->chkVSync->isChecked();
}

void frmSettings::setVsync(bool value)
{
    ui->chkVSync->setChecked(value);
}

bool frmSettings::msaa()
{
    return ui->radMSAA->isChecked();
}

void frmSettings::setMsaa(bool msaa)
{
    ui->radMSAA->setChecked(msaa);
}

bool frmSettings::autoCompletion()
{
    return ui->chkConsoleAutocompletion->isChecked();
}

void frmSettings::setAutoCompletion(bool autoCompletion)
{
    ui->chkConsoleAutocompletion->setChecked(autoCompletion);
}

int frmSettings::units()
{
    return m_units;
}

void frmSettings::setUnits(int units)
{
    m_units = units;
}

bool frmSettings::simplify()
{
    return ui->chkSimplify->isChecked();
}

void frmSettings::setSimplify(bool simplify)
{
    ui->chkSimplify->setChecked(simplify);
}

double frmSettings::simplifyPrecision()
{
    return ui->txtSimplifyPrecision->value();
}

void frmSettings::setSimplifyPrecision(double simplifyPrecision)
{
    ui->txtSimplifyPrecision->setValue(simplifyPrecision);
}

QList<ColorPicker *> frmSettings::colors()
{
    return ui->grpColors->findChildren<ColorPicker*>();
}

QColor frmSettings::colors(QString name)
{
    ColorPicker *pick = this->findChildren<ColorPicker*>("clp" + name).at(0);
    if (pick) return pick->color(); else return QColor();
}

int frmSettings::fontSize()
{
    return ui->cboFontSize->currentText().toInt();
}

void frmSettings::setFontSize(int fontSize)
{
    ui->cboFontSize->setCurrentText(QString::number(fontSize));
}

bool frmSettings::grayscaleSegments()
{
    return ui->chkGrayscale->isChecked();
}

void frmSettings::setGrayscaleSegments(bool value)
{
    ui->chkGrayscale->setChecked(value);
}

bool frmSettings::grayscaleSCode()
{
    return ui->radGrayscaleS->isChecked();
}

void frmSettings::setGrayscaleSCode(bool value)
{
    ui->radGrayscaleS->setChecked(value);
    ui->radGrayscaleZ->setChecked(!value);
}

bool frmSettings::drawModeVectors()
{
    return ui->radDrawModeVectors->isChecked();
}

void frmSettings::setDrawModeVectors(bool value)
{
    ui->radDrawModeVectors->setChecked(value);
    ui->radDrawModeRaster->setChecked(!value);
}

bool frmSettings::ignoreErrors()
{
    return ui->chkIgnoreErrors->isChecked();
}

void frmSettings::setIgnoreErrors(bool value)
{
    ui->chkIgnoreErrors->setChecked(value);
}

bool frmSettings::autoLine()
{
    return ui->chkAutoLine->isChecked();
}

void frmSettings::setAutoLine(bool value)
{
    ui->chkAutoLine->setChecked(value);
}

bool frmSettings::useStartCommands()
{
    return ui->chkUseStartCommands->isChecked();
}

void frmSettings::setUseStartCommands(bool value)
{
    ui->chkUseStartCommands->setChecked(value);
}

QString frmSettings::startCommands()
{
    return ui->txtStartCommands->toPlainText();
}

void frmSettings::setStartCommands(QString commands)
{
    ui->txtStartCommands->setPlainText(commands);
}

bool frmSettings::useEndCommands()
{
    return ui->chkUseEndCommands->isChecked();
}

void frmSettings::setUseEndCommands(bool value)
{
    ui->chkUseEndCommands->setChecked(value);
}

QString frmSettings::endCommands()
{
    return ui->txtEndCommands->toPlainText();
}

void frmSettings::setEndCommands(QString commands)
{
    ui->txtEndCommands->setPlainText(commands);
}

QString frmSettings::toolChangeCommands()
{
    return ui->txtToolChangeCommands->toPlainText();
}

void frmSettings::setToolChangeCommands(QString commands)
{
    ui->txtToolChangeCommands->setPlainText(commands);
}

bool frmSettings::toolChangeUseCommands()
{
    return ui->chkToolChangeUseCommands->isChecked();
}

void frmSettings::setToolChangeUseCommands(bool value)
{
    ui->chkToolChangeUseCommands->setChecked(value);
}

bool frmSettings::toolChangeUseCommandsConfirm()
{
    return ui->chkToolChangeUseCommandsConfirm->isChecked();
}

void frmSettings::setToolChangeUseCommandsConfirm(bool value)
{
    ui->chkToolChangeUseCommandsConfirm->setChecked(value);
}

bool frmSettings::toolChangePause()
{
    return ui->chkToolChangePause->isChecked();
}

void frmSettings::setToolChangePause(bool pause)
{
    ui->chkToolChangePause->setChecked(pause);
}

QString frmSettings::language()
{
    return ui->cboLanguage->currentData().toString();
}

void frmSettings::setLanguage(QString language)
{
    int i = ui->cboLanguage->findData(language);
    if (i != -1) ui->cboLanguage->setCurrentIndex(i);
}

QVector3D frmSettings::machineBounds()
{
    return m_machineBounds;
}

void frmSettings::setMachineBounds(QVector3D bounds)
{
    m_machineBounds = bounds;
}

bool frmSettings::homingEnabled()
{
    return m_homingEnabled;
}

void frmSettings::setHomingEnabled(bool homing)
{
    m_homingEnabled = homing;
}

bool frmSettings::softLimitsEnabled()
{
    return m_softLimitsEnabled;
}

void frmSettings::setSoftLimitsEnabled(bool softLimits)
{
    m_softLimitsEnabled = softLimits;
}

bool frmSettings::referenceXPlus()
{
    return ui->radReferenceXPlus->isChecked();
}

void frmSettings::setReferenceXPlus(bool value)
{
    ui->radReferenceXPlus->setChecked(value);
}

bool frmSettings::referenceYPlus()
{
    return ui->radReferenceYPlus->isChecked();
}

void frmSettings::setReferenceYPlus(bool value)
{
    ui->radReferenceYPlus->setChecked(value);
}

bool frmSettings::referenceZPlus()
{
    return ui->radReferenceZPlus->isChecked();
}

void frmSettings::setReferenceZPlus(bool value)
{
    ui->radReferenceZPlus->setChecked(value);
}

void frmSettings::setUsePauseCommands(bool value)
{
    ui->chkUsePauseCommands->setChecked(value);
}

bool frmSettings::usePauseCommands()
{
    return ui->chkUsePauseCommands->isChecked();
}

void frmSettings::setBeforePauseCommands(QString text)
{
    ui->txtBeforePauseCommands->setPlainText(text);
}

void frmSettings::setAfterPauseCommands(QString text)
{
    ui->txtAfterPauseCommands->setPlainText(text);
}

QString frmSettings::beforePauseCommands()
{
    return ui->txtBeforePauseCommands->toPlainText();
}

QString frmSettings::afterPauseCommands()
{
    return ui->txtAfterPauseCommands->toPlainText();
}

void frmSettings::showEvent(QShowEvent *se)
{
    Q_UNUSED(se)
}

void frmSettings::searchForSerialPorts()
{
    ui->cboSerialPort->clear();

    foreach (QSerialPortInfo info ,QSerialPortInfo::availablePorts()) {
        ui->cboSerialPort->insertItem(0, info.portName());
    }
}

void frmSettings::on_cmdSerialRefresh_clicked()
{
    searchForSerialPorts();
}

void frmSettings::on_cmdOK_clicked()
{
    applySettings();

    this->accept();
}

void frmSettings::on_cmdCancel_clicked()
{
    this->reject();
}

void frmSettings::on_cboToolType_currentIndexChanged(int index)
{
    ui->lblToolAngle->setEnabled(index == 1);
    ui->txtToolAngle->setEnabled(index == 1);
}

void frmSettings::resetToDefaults()
{
    m_configuration.setDefaults();
    initializeWidgets();
}

void frmSettings::on_cmdDefaults_clicked()
{
    if (QMessageBox::warning(this, qApp->applicationDisplayName(), tr("Reset settings to default values?"),
                             QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel) != QMessageBox::Yes) return;

    resetToDefaults();

    setIgnoreErrors(false);

    setRapidSpeed(2000);
    setAcceleration(100);
    setSpindleSpeedMin(0);
    setSpindleSpeedMax(10000);
    setLaserPowerMin(0);
    setLaserPowerMax(100);
    setUnits(0);

    setArcLength(0.1);
    setArcDegreeMode(true);
    setArcDegree(5.0);

    setLineWidth(1.5);
    setAntialiasing(true);
    setMsaa(true);
    setSimplify(true);
    setSimplifyPrecision(0.0);
    setFps(60);
    setZBuffer(false);
    setFov(60.0);
    setNearPlane(0.5);
    setFarPlane(10000.0);
    setGrayscaleSegments(false);
    setGrayscaleSCode(true);
    setDrawModeVectors(true);

    setToolType(1);
    setToolAngle(15.0);
    setToolDiameter(3.0);
    setToolLength(30.0);

    setShowProgramCommands(false);
    setAutoCompletion(true);

    ui->clpTool->setColor(QColor(255, 153, 0));

    ui->clpVisualizerBackground->setColor(QColor(255, 255, 255));
    ui->clpVisualizerText->setColor(QColor(0, 0, 0));

    ui->clpToolpathNormal->setColor(QColor(0, 0, 0));
    ui->clpToolpathDrawn->setColor(QColor(217, 217, 217));
    ui->clpToolpathHighlight->setColor(QColor(145, 130, 230));
    ui->clpToolpathZMovement->setColor(QColor(255, 0, 0));
    ui->clpToolpathStart->setColor(QColor(255, 0, 0));
    ui->clpToolpathEnd->setColor(QColor(0, 255, 0));

    setFontSize(9);

    // Shortcuts
    QMap<QString, QString> d;
    d["actFileNew"] = "Ctrl+N";
    d["actFileOpen"] = "Ctrl+O";
    d["actFileSave"] = "Ctrl+S";
    d["actFileSaveAs"] = "Ctrl+Shift+S";
    d["actJogXPlus"] = "Num+6";
    d["actJogXMinus"] = "Num+4";
    d["actJogYPlus"] = "Num+8";
    d["actJogYMinus"] = "Num+2";
    d["actJogZPlus"] = "Num+9";
    d["actJogZMinus"] = "Num+3";
    d["actJogStop"] = "Num+5";
    d["actJogStepNext"] = "Num+1";
    d["actJogStepPrevious"] = "Num+7";
    d["actJogFeedNext"] = "Num++";
    d["actJogFeedPrevious"] = "Num+-";
    d["actJogKeyboardControl"] = "ScrollLock";
    d["actSpindleOnOff"] = "Num+0";
    d["actSpindleSpeedPlus"] = "Num+*";
    d["actSpindleSpeedMinus"] = "Num+/";
    
    QTableWidget *table = ui->tblShortcuts;

    for (int i = 0; i < table->rowCount(); i++) {
        QString s = table->item(i, 0)->data(Qt::DisplayRole).toString();
        table->item(i, 2)->setData(Qt::DisplayRole, d.keys().contains(s) ? d[s] : "");
    }

    ui->txtStartCommands->clear();
    ui->txtEndCommands->clear();
    ui->txtToolChangeCommands->clear();
    ui->chkToolChangePause->setChecked(false);
    ui->chkToolChangeUseCommands->setChecked(false);
    ui->chkToolChangeUseCommandsConfirm->setChecked(false);
    setLanguage("en");

    emit settingsSetToDefault();
}

void frmSettings::on_cboFontSize_currentTextChanged(const QString &arg1)
{
    qApp->setStyleSheet(QString(qApp->styleSheet()).replace(QRegExp("font-size:\\s*\\d+"), "font-size: " + arg1));
}

void frmSettings::on_radDrawModeVectors_toggled(bool checked)
{
    ui->chkSimplify->setEnabled(checked);
    ui->lblSimpilyPrecision->setEnabled(checked && ui->chkSimplify->isChecked());
    ui->txtSimplifyPrecision->setEnabled(checked && ui->chkSimplify->isChecked());

    ui->radDrawModeRaster->setChecked(!checked);
}

void frmSettings::on_radDrawModeRaster_toggled(bool checked)
{
    ui->radDrawModeVectors->setChecked(!checked);
}

void frmSettings::on_radGrayscaleS_toggled(bool checked)
{
    ui->radGrayscaleZ->setChecked(!checked);
}

void frmSettings::on_radGrayscaleZ_toggled(bool checked)
{
    ui->radGrayscaleS->setChecked(!checked);
}

void frmSettings::onConnectionModeChanged(int mod)
{
    ui->frameConnectionRawSocket->hide();
    ui->frameConnectionSerial->hide();
    ui->frameConnectionSimulator->hide();
    switch (mod) {
        // serial
        case 0:
            ui->frameConnectionSerial->show();
            break;
        // tcp
        case 1:
            ui->frameConnectionRawSocket->show();
            break;
        // virtual uCNC
        case 2:
            ui->frameConnectionSimulator->show();
            break;
    }
}
