// This file is a part of "Candle" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich

#include "frmsettings.h"
#include "ui_frmsettings.h"
#include "utils.h"
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

    connect(ui->cmdOK, &QAbstractButton::clicked, this, &frmSettings::onCmdOKClicked);
    connect(ui->cmdCancel, &QAbstractButton::clicked, this, &frmSettings::onCmdCancelClicked);
    connect(ui->cmdDefaults, &QAbstractButton::clicked, this, &frmSettings::onCmdDefaultsClicked);
    connect(ui->cmdSerialPortsRefresh, &QAbstractButton::clicked, this, &frmSettings::onCmdSerialPortsRefreshClicked);
    connect(ui->radDrawModeVectors, &QRadioButton::toggled, this, &frmSettings::onDrawModeVectorsToggled);
    connect(ui->radArcDegreeMode, &QRadioButton::toggled, this, &frmSettings::onArcApproximationModeChanged);
    connect(ui->radArcLengthMode, &QRadioButton::toggled, this, &frmSettings::onArcApproximationModeChanged);
    connect(ui->chkOverrideMaxTravel, &QAbstractButton::toggled, [=](bool checked) {
        ui->txtMaxTravelX->setEnabled(checked);
        ui->txtMaxTravelY->setEnabled(checked);
        ui->txtMaxTravelZ->setEnabled(checked);
    });

    //

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

    static QRegularExpression fx("_([^\\.]+)");
    foreach (const QString &t, tl) {
        QRegularExpressionMatch match = fx.match(t);
        if (match.hasMatch()) {
            QLocale l(match.captured(1));
            ui->cboLanguage->addItem(l.nativeLanguageName(), l.name().left(2));
        }
    }

    // Connection mode
    ui->frameConnectionRawSocket->hide();
    ui->frameConnectionSimulator->hide();
    connect(ui->cboConnectionMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onConnectionModeChanged(int)));

    m_animatingScrollBox = false;
    m_scrollingManuallyScrollBox = false;

    //Validators
    this->setLocale(QLocale::C);
    m_intValidator.setBottom(1);
    m_intValidator.setTop(999);

    ui->cboFpsLock->setValidator(&m_intValidator);
    ui->cboFontSize->setValidator(&m_intValidator);

    connect(ui->txtJoggingStepChoices, &QLineEdit::editingFinished, this, [=]() {
        QLineEdit *sender = dynamic_cast<QLineEdit*>(QObject::sender());
        QString text = sender->text();
        int pos = 0;
        auto state = m_commaSeparatedDoubleValidator.validate(text, pos);
        QPalette pal = sender->palette();
        if (state == QValidator::Invalid) {
            pal.setColor(QPalette::Text, Qt::red);
        } else {
            pal.setColor(QPalette::Text, nullptr);
        }
        this->widgetValidity("JoggingStepChoices", state != QValidator::Invalid);
        sender->setPalette(pal);
    });
    connect(ui->txtJoggingFeedChoices, &QLineEdit::editingFinished, this, [=]() {
        QLineEdit *sender = dynamic_cast<QLineEdit*>(QObject::sender());
        QString text = sender->text();
        int pos = 0;
        auto state = m_commaSeparatedIntValidator.validate(text, pos);
        QPalette pal = sender->palette();
        if (state == QValidator::Invalid) {
            pal.setColor(QPalette::Text, Qt::red);
        } else {
            pal.setColor(QPalette::Text, nullptr);
        }
        this->widgetValidity("JoggingFeedChoices", state != QValidator::Invalid);
        sender->setPalette(pal);
    });
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
    ui->chkConsoleShowSystemCommands->setChecked(console.showSystemCommands());

    const ConfigurationConnection &connection = m_configuration.connectionModule();
    ui->cboConnectionMode->setCurrentIndex(connection.connectionMode());
    ui->txtQueryStateInterval->setValue(connection.queryStateInterval());
    ui->cboSerialPort->setCurrentText(connection.serialPort());
    ui->cboSerialBaud->setCurrentText(QString::number(connection.serialBaud()));
    ui->txtRawTcpHost->setText(connection.rawTcpHost());
    ui->txtRawTcpPort->setText(QString::number(connection.rawTcpPort()));

    const ConfigurationVisualizer &visualizer = m_configuration.visualizerModule();
    ui->chkAntialiasing->setChecked(visualizer.antialiasing());
    ui->chkZBuffer->setChecked(visualizer.zBuffer());
    ui->txtFieldOfView->setValue(visualizer.fieldOfView());
    ui->txtNearPlane->setValue(visualizer.nearPlane());
    ui->txtFarPlane->setValue(visualizer.farPlane());
    ui->txtLineWidth->setValue(visualizer.lineWidth());
    ui->cboFpsLock->setCurrentText(QString::number(visualizer.fpsLock()));
    ui->chkVSync->setChecked(visualizer.vsync());
    ui->chkMSAA->setChecked(visualizer.msaa());
    ui->chkSimplifyGeometry->setChecked(visualizer.simplifyGeometry());
    ui->txtSimplifyPrecision->setValue(visualizer.simplifyGeometryPrecision());
    ui->chkGrayscaleSegments->setChecked(visualizer.grayscaleSegments());
    ui->radGrayscaleSegmentsByS->setChecked(visualizer.grayscaleSegmentsBySCode());
    ui->radGrayscaleSegmentsByZ->setChecked(visualizer.grayscaleSegmentsByZCode());
    ui->radDrawModeVectors->setChecked(visualizer.programDrawMode() == ConfigurationVisualizer::ProgramDrawMode::Vectors);
    ui->radDrawModeRaster->setChecked(visualizer.programDrawMode() == ConfigurationVisualizer::ProgramDrawMode::Raster);
    ui->cboToolType->setCurrentIndex(visualizer.toolType());
    ui->txtToolAngle->setValue(visualizer.toolAngle());
    ui->txtToolDiameter->setValue(visualizer.toolDiameter());
    ui->txtToolLength->setValue(visualizer.toolLength());
    ui->clpToolColor->setColor(visualizer.toolColor());
    ui->clpVisualizerBackgroundColor->setColor(visualizer.backgroundColor());
    ui->clpVisualizerTextColor->setColor(visualizer.textColor());
    ui->clpToolpathNormalColor->setColor(visualizer.normalToolpathColor());
    ui->clpToolpathDrawnColor->setColor(visualizer.drawnToolpathColor());
    ui->clpToolpathHighlightColor->setColor(visualizer.hightlightToolpathColor());
    ui->clpToolpathZMovementColor->setColor(visualizer.zMovementColor());
    ui->clpToolpathStartColor->setColor(visualizer.startPointColor());
    ui->clpToolpathEndColor->setColor(visualizer.endPointColor());
    ui->clpVisualizerTableGridColor->setColor(visualizer.tableSurfaceGridColor());
    ui->chkShow3dCursor->setChecked(visualizer.show3dCursor());

    const ConfigurationSender &sender = m_configuration.senderModule();
    ui->chkUseStartCommands->setChecked(sender.useProgramStartCommands());
    ui->txtStartCommands->setPlainText(sender.programStartCommands());
    ui->chkUseEndCommands->setChecked(sender.useProgramEndCommands());
    ui->txtEndCommands->setPlainText(sender.programEndCommands());
    ui->txtToolChangeCommands->setPlainText(sender.toolChangeCommands());
    ui->chkUseToolChangeCommands->setChecked(sender.useToolChangeCommands());
    ui->chkConfirmToolChangeCommandsExecution->setChecked(sender.confirmToolChangeCommandsExecution());
    ui->chkPauseOnToolChange->setChecked(sender.pauseSenderOnToolChange());
    ui->chkUsePauseCommands->setChecked(sender.usePauseCommands());
    ui->txtBeforePauseCommands->setPlainText(sender.beforePauseCommands());
    ui->txtAfterPauseCommands->setPlainText(sender.afterPauseCommands());
    ui->chkSetParseStateBeforeSendFromLine->setChecked(sender.setParserStateBeforeSendingFromSelectedLine());
    ui->chkIgnoreResponseErrors->setChecked(sender.ignoreErrorResponses());

    const ConfigurationParser &parser = m_configuration.parserModule();
    ui->radArcDegreeMode->setChecked(parser.arcApproximationMode() == ConfigurationParser::ByAngle);
    ui->txtArcLength->setValue(parser.arcApproximationLength());
    ui->txtArcDegree->setValue(parser.arcApproximationAngle());

    const ConfigurationMachine &machine = m_configuration.machineModule();
    ui->txtSpindleSpeedMin->setValue(machine.spindleSpeedRange().min);
    ui->txtSpindleSpeedMax->setValue(machine.spindleSpeedRange().max);
    ui->txtLaserPowerMin->setValue(machine.laserPowerRange().min);
    ui->txtLaserPowerMax->setValue(machine.laserPowerRange().max);
    ui->radReferenceXMinus->setChecked(machine.referencePositionDirX() == ConfigurationMachine::Negative);
    ui->radReferenceXPlus->setChecked(machine.referencePositionDirX() == ConfigurationMachine::Positive);
    ui->radReferenceYMinus->setChecked(machine.referencePositionDirY() == ConfigurationMachine::Negative);
    ui->radReferenceYPlus->setChecked(machine.referencePositionDirY() == ConfigurationMachine::Positive);
    ui->radReferenceZMinus->setChecked(machine.referencePositionDirZ() == ConfigurationMachine::Negative);
    ui->radReferenceZPlus->setChecked(machine.referencePositionDirZ() == ConfigurationMachine::Positive);
    ui->chkOverrideMaxTravel->setChecked(machine.overrideMaxTravel());
    ui->txtMaxTravelX->setValue(machine.maxTravel().x());
    ui->txtMaxTravelY->setValue(machine.maxTravel().y());
    ui->txtMaxTravelZ->setValue(machine.maxTravel().z());

    const ConfigurationUI &ui_ = m_configuration.uiModule();
    ui->cboFontSize->setCurrentText(QString::number(ui_.fontSize()));
    ui->cboLanguage->setCurrentIndex(ui->cboLanguage->findData(ui_.language()));

    const ConfigurationJogging &jogging = m_configuration.joggingModule();
    ui->txtJoggingStepChoices->setText(jogging.stepChoices().join(", "));
    ui->txtJoggingFeedChoices->setText(jogging.feedChoices().join(", "));
}

void frmSettings::applySettings()
{
    ConfigurationConsole &console = m_configuration.consoleModule();
    console.m_commandAutoCompletion = ui->chkConsoleAutocompletion->isChecked();
    console.m_darkBackgroundMode = ui->chkConsoleDarkMode->isChecked();
    console.m_showProgramCommands = ui->chkConsoleShowProgramCommands->isChecked();
    console.m_showUiCommands = ui->chkConsoleShowUICommands->isChecked();
    console.m_showSystemCommands = ui->chkConsoleShowSystemCommands->isChecked();

    ConfigurationConnection &connection = m_configuration.connectionModule();
    connection.m_connectionMode = static_cast<ConnectionMode>(ui->cboConnectionMode->currentIndex());
    connection.m_queryStateInterval = ui->txtQueryStateInterval->value();
    connection.m_serialPort = ui->cboSerialPort->currentText();
    connection.m_serialBaud = ui->cboSerialBaud->currentText().toInt();
    connection.m_rawTcpHost = ui->txtRawTcpHost->text();
    connection.m_rawTcpPort = ui->txtRawTcpPort->text().toInt();

    ConfigurationVisualizer &visualizer = m_configuration.visualizerModule();
    visualizer.m_antialiasing = ui->chkAntialiasing->isChecked();
    visualizer.m_zBuffer = ui->chkZBuffer->isChecked();
    visualizer.m_fieldOfView = ui->txtFieldOfView->value();
    visualizer.m_nearPlane = ui->txtNearPlane->value();
    visualizer.m_farPlane = ui->txtFarPlane->value();
    visualizer.m_lineWidth = ui->txtLineWidth->value();
    visualizer.m_fpsLock = ui->cboFpsLock->currentText().toInt();
    visualizer.m_vsync = ui->chkVSync->isChecked();
    visualizer.m_msaa = ui->chkMSAA->isChecked();
    visualizer.m_simplifyGeometry = ui->chkSimplifyGeometry->isChecked();
    visualizer.m_simplifyGeometryPrecision = ui->txtSimplifyPrecision->value();
    visualizer.m_grayscaleSegments = ui->chkGrayscaleSegments->isChecked();
    visualizer.m_grayscaleSegmentsBySCode = ui->radGrayscaleSegmentsByS->isChecked();
    visualizer.m_grayscaleSegmentsByZCode = ui->radGrayscaleSegmentsByZ->isChecked();
    visualizer.m_programDrawMode = ui->radDrawModeVectors->isChecked() ? ConfigurationVisualizer::ProgramDrawMode::Vectors : ConfigurationVisualizer::ProgramDrawMode::Raster;
    visualizer.m_toolType = (ConfigurationVisualizer::ToolType) ui->cboToolType->currentIndex();
    visualizer.m_toolAngle = ui->txtToolAngle->value();
    visualizer.m_toolDiameter = ui->txtToolDiameter->value();
    visualizer.m_toolLength = ui->txtToolLength->value();
    visualizer.m_toolColor = ui->clpToolColor->color();
    visualizer.m_backgroundColor = ui->clpVisualizerBackgroundColor->color();
    visualizer.m_textColor = ui->clpVisualizerTextColor->color();
    visualizer.m_normalToolpathColor = ui->clpToolpathNormalColor->color();
    visualizer.m_drawnToolpathColor = ui->clpToolpathDrawnColor->color();
    visualizer.m_hightlightToolpathColor = ui->clpToolpathHighlightColor->color();
    visualizer.m_zMovementColor = ui->clpToolpathZMovementColor->color();
    visualizer.m_startPointColor = ui->clpToolpathStartColor->color();
    visualizer.m_endPointColor = ui->clpToolpathEndColor->color();
    visualizer.m_tableSurfaceGridColor = ui->clpVisualizerTableGridColor->color();
    visualizer.m_show3dCursor = ui->chkShow3dCursor->isChecked();

    ConfigurationSender &sender = m_configuration.senderModule();
    sender.m_useProgramStartCommands = ui->chkUseStartCommands->isChecked();
    sender.m_programStartCommands = ui->txtStartCommands->toPlainText();
    sender.m_useProgramEndommands = ui->chkUseEndCommands->isChecked();
    sender.m_programEndCommands = ui->txtEndCommands->toPlainText();
    sender.m_usePauseCommands = ui->chkUsePauseCommands->isChecked();
    sender.m_beforePauseCommands = ui->txtBeforePauseCommands->toPlainText();
    sender.m_afterPauseCommands = ui->txtAfterPauseCommands->toPlainText();
    sender.m_useToolChangeCommands = ui->chkUseToolChangeCommands->isChecked();
    sender.m_toolChangeCommands = ui->txtToolChangeCommands->toPlainText();
    sender.m_confirmToolChangeCommandsExecution = ui->chkConfirmToolChangeCommandsExecution->isChecked();
    sender.m_toolChangePause = ui->chkPauseOnToolChange->isChecked();
    sender.m_ignoreErrorResponses = ui->chkIgnoreResponseErrors->isChecked();
    sender.m_setParserStateBeforeSendingFromSelectedLine = ui->chkSetParseStateBeforeSendFromLine->isChecked();

    ConfigurationParser &parser = m_configuration.parserModule();
    parser.m_arcApproximationMode = ui->radArcDegreeMode->isChecked() ? ConfigurationParser::ByAngle : ConfigurationParser::ByLength;
    parser.m_arcApproximationLength = ui->txtArcLength->value();
    parser.m_arcApproximationAngle = ui->txtArcDegree->value();
    onArcApproximationModeChanged(false);

    ConfigurationMachine &machine = m_configuration.machineModule();
    machine.m_spindleSpeedRange = {ui->txtSpindleSpeedMin->value(), ui->txtSpindleSpeedMax->value()};
    machine.m_laserPowerRange = {ui->txtLaserPowerMin->value(), ui->txtLaserPowerMax->value()};
    machine.m_referencePositionDirX = ui->radReferenceXMinus->isChecked() ? ConfigurationMachine::Negative : ConfigurationMachine::Positive;
    machine.m_referencePositionDirY = ui->radReferenceYMinus->isChecked() ? ConfigurationMachine::Negative : ConfigurationMachine::Positive;
    machine.m_referencePositionDirZ = ui->radReferenceZMinus->isChecked() ? ConfigurationMachine::Negative : ConfigurationMachine::Positive;
    machine.m_overrideMaxTravel = ui->chkOverrideMaxTravel->isChecked();
    machine.m_maxTravel = QVector3D(ui->txtMaxTravelX->value(), ui->txtMaxTravelY->value(), ui->txtMaxTravelZ->value());

    ConfigurationUI &ui_ = m_configuration.uiModule();
    ui_.m_fontSize = ui->cboFontSize->currentText().toInt();
    ui_.m_language = ui->cboLanguage->currentData().toString();

    ConfigurationJogging &jogging = m_configuration.joggingModule();
    QRegularExpression nonDigits("[^\\d^.]");
    jogging.m_stepChoices = ui->txtJoggingStepChoices->text().split(",").replaceInStrings(nonDigits, "");
    jogging.m_feedChoices = ui->txtJoggingFeedChoices->text().split(",").replaceInStrings(nonDigits, "");
}

void frmSettings::widgetValidity(QString widgetName, bool valid)
{
    if (valid) {
        invalidWidgets.removeOne(widgetName);
    } else if (invalidWidgets.indexOf(widgetName) == -1) {
        invalidWidgets.append(widgetName);
    }
    qDebug() << "Invalid widgets: " << invalidWidgets;
    ui->cmdOK->setEnabled(invalidWidgets.empty());
}

int frmSettings::exec()
{
    Utils::positionDialog(this, m_configuration.uiModule().settingsFormGeometry());

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

    invalidWidgets.clear();
    initializeWidgets();

    int result = QDialog::exec();

    m_configuration.uiModule().setSettingsFormGeometry(this);
    m_configuration.uiModule().setSettingsFormSlicerSizes(ui->splitMain->sizes());

    return result;
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

void frmSettings::searchForSerialPorts()
{
    ui->cboSerialPort->clear();

    foreach (QSerialPortInfo info ,QSerialPortInfo::availablePorts()) {
        ui->cboSerialPort->insertItem(0, info.portName());
    }
}

void frmSettings::onCmdSerialPortsRefreshClicked()
{
    searchForSerialPorts();
}

void frmSettings::onCmdOKClicked()
{
    applySettings();

    this->accept();
}

void frmSettings::onCmdCancelClicked()
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
    invalidWidgets.clear();
    initializeWidgets();
}

void frmSettings::onCmdDefaultsClicked()
{
    if (QMessageBox::warning(this, qApp->applicationDisplayName(), tr("Reset settings to default values?"),
                             QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel) != QMessageBox::Yes) return;

    resetToDefaults();

    // setFontSize(9);

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
    ui->chkPauseOnToolChange->setChecked(false);
    ui->chkUseToolChangeCommands->setChecked(false);
    ui->chkConfirmToolChangeCommandsExecution->setChecked(false);

    emit settingsSetToDefault();
}

void frmSettings::onDrawModeVectorsToggled(bool checked)
{
    ui->chkSimplifyGeometry->setEnabled(checked);
    ui->lblSimpilyPrecision->setEnabled(checked && ui->chkSimplifyGeometry->isChecked());
    ui->txtSimplifyPrecision->setEnabled(checked && ui->chkSimplifyGeometry->isChecked());
}

void frmSettings::onArcApproximationModeChanged(bool checked)
{
    Q_UNUSED(checked);

    ui->txtArcLength->setEnabled(ui->radArcLengthMode->isChecked());
    ui->txtArcDegree->setEnabled(ui->radArcDegreeMode->isChecked());
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
