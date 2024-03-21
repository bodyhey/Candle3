#include "frmgrblconfigurator.h"
#include "ui_frmgrblconfigurator.h"
#include <CPropertyEditor.h>
#include <CPropertyHeader.h>
#include <CStringProperty.h>
#include <CBoolProperty.h>
#include <CIntegerProperty.h>
#include <CColorProperty.h>
#include <CPropertyHeader.h>
#include <CBoolProperty.h>
#include <CDoubleProperty.h>
#include <CListProperty.h>
#include <CFontProperty.h>

enum Group {
    General,
    StepInvert,
    DirInvert,
    Limits,
    Homing,
    HomingInvert,
    Spindle,
    Steps,
    MaxRate,
    Acceleration,
    Reporting,
    MaxTravel,
    Quality,
};

struct ConfigGroup {
    Group group;
    QByteArray name;
    std::vector<ConfigEntry> entries;
    CPropertyHeader* header = nullptr;
};

#define REPORT_POS_TYPE_SETTING_ID 150
#define REPORT_POS_TYPE_SETTING_BIT 1
#define REPORT_BUFFER_SETTING_ID 151
#define REPORT_BUFFER_SETTING_BIT 2

std::vector<ConfigGroup> ConfigMap = {
    {
        General,
        "General settings",
        {
            {0, "Step pulse, microseconds", "Shortest pulses for stepper drivers to recognize.", Integer},
            {1, "Step idle delay, milliseconds", "Delay before disabling steppers after motion.", Integer},
            {32, "Laser mode", "Enables continuous motion for laser cutting.", Boolean},
            //uCNC uses mask??
            {4, "Step enable invert", "Inverts the stepper enable pin.", Boolean},
            //uCNC uses mask??
            {5, "Limit pins invert", "Inverts the limit pins.", Boolean},
            {6, "Probe pin invert", "Inverts the probe pin.", Boolean},
        }
    },
    {
        Reporting,
        "Reporting machine status",
        {
             {REPORT_POS_TYPE_SETTING_ID , "Report position type", "Enabled `MPos`, Disabled `WPos`", Boolean},
             {REPORT_BUFFER_SETTING_ID, "Report buffer data", "Enabled `Buf` field appears with planner and serial RX available buffer.", Boolean},
             {13, "Report inches", "Toggles between mm and inches for reporting.", Boolean},
        }
    },
    {
        Quality,
        "Quality",
        {
            {11, "Junction deviation, mm", "Determines cornering speed.", Double},
            {12, "Arc tolerance, mm", "Accuracy of arc tracing.", Double},
        }
    },
    {
        Limits,
        "Limits",
        {
             {20, "Soft limits", "Prevents machine from traveling beyond limits.", Boolean},
             {21, "Hard limits", "Uses physical switches to prevent over-travel.", Boolean},
        },
    },
    {
        StepInvert,
        "Step signal polarity",
        {
            {2, "%1 step signal invert", "Inverts the step pulse signal.", Axes},
        }
    },
    {
        DirInvert,
        "Dir signal polarity",
        {
            {3, "%1 direction signal invert", "Inverts the direction signal for each axis.", Axes},
        }
    },
    {
        Homing,
        "Homing settings",
        {
            {22, "Homing cycle", "Locates a consistent position on startup.", Boolean},
            {24, "Homing feed, mm/min", "Feed rate for precise homing location.", Integer},
            {25, "Homing seek, mm/min", "Search rate for homing cycle.", Integer},
            {26, "Homing debounce, milliseconds", "Debounce delay for homing switches.", Integer},
            {27, "Homing pull-off, mm", "Distance to pull off from limit switches after homing.", Integer},
        }
    },
    {
        HomingInvert,
        "Homing signal polarity",
        {
            {23, "%1 homing dir invert", "Inverts homing direction for certain axes.", Axes},
        }
    },
    {
        Spindle,
        "Spindle",
        {
            {30, "Max spindle speed, RPM", "Sets max spindle speed for 5V PWM output.", Integer},
            {31, "Min spindle speed, RPM", "Sets min spindle speed for 0.02V PWM output.", Integer},
        }
    },
    {
        Steps,
        "Steps per mm",
        {
            {100, "X steps/mm", "Steps per mm for X axis.", Integer},
            {101, "Y steps/mm", "Steps per mm for Y axis.", Integer},
            {102, "Z steps/mm", "Steps per mm for Z axis.", Integer},
        }
    },
    {
        MaxRate,
        "Max axis rate",
        {
            {110, "X max rate, mm/min", "Max rate X axis can move.", Integer},
            {111, "Y max rate, mm/min", "Max rate Y axis can move.", Integer},
            {112, "Z max rate, mm/min", "Max rate Z axis can move.", Integer},
        }
    },
    {
        Acceleration,
        "Max axis acceleration",
        {
            {120, "X acceleration, mm/sec^2", "Acceleration parameters for X axis.", Integer},
            {121, "Y acceleration, mm/sec^2", "Acceleration parameters for Y axis.", Integer},
            {122, "Z acceleration, mm/sec^2", "Acceleration parameters for Z axis.", Integer},
        }
    },
    {
        MaxTravel,
        "Max axis travel",
        {
            {130, "X max travel, mm", "Max travel for X axis.", Integer},
            {131, "Y max travel, mm", "Max travel for Y axis.", Integer},
            {132, "Z max travel, mm", "Max travel for Z axis.", Integer},
        }
    }
};

frmGrblConfigurator::frmGrblConfigurator(QWidget *parent, Communicator *communicator)
    : QDialog(parent)
    , ui(new Ui::frmGrblConfigurator)
    , m_communicator(communicator)
{
    ui->setupUi(this);
    ui->editor->init();

    for (std::vector<ConfigGroup>::iterator it = ConfigMap.begin(); it != ConfigMap.end(); it++) {
        ConfigGroup &group = *it;
        CPropertyHeader* header = new CPropertyHeader(group.name, group.name);

        group.header = header;
        ui->editor->add(header);

        for (std::vector<ConfigEntry>::iterator it2 = group.entries.begin(); it2 != group.entries.end(); it2++) {
            ConfigEntry &entry = *it2;
            switch (entry.type) {
                case Axes:
                    entry.properties = addAxesProperty(header, entry);
                    break;
                case Boolean:
                    entry.property = addBooleanProperty(header, entry);
                    break;
                case Integer:
                    entry.property = addIntegerProperty(header, entry);
                    break;
                case Double:
                    entry.property = addDoubleProperty(header, entry);
                    break;
            }
        }
    }

    ui->editor->adjustToContents();

    connect(ui->editor, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(itemChanged(QTreeWidgetItem*,int)));

    update();
}

frmGrblConfigurator::~frmGrblConfigurator()
{
    delete ui;
}

void frmGrblConfigurator::setSettings(QMap<int, double> settings)
{
    disconnect(m_communicator, SIGNAL(settingsReceived(QMap<int,double>)), this, SLOT(setSettings(QMap<int,double>)));

    if (m_isSaving) {
        m_isSaving = false;
        findParametersToBeSaved(settings);

        return;
    }


    m_currentSettings = settings;

    setInfo("Updated", Qt::black);
    qDebug() << "Settings received" << settings;

    // ugly hack! split status report setting into two separate settings
    int statusReportSetting = settings[10];
    settings[REPORT_BUFFER_SETTING_ID] = statusReportSetting & REPORT_BUFFER_SETTING_BIT;
    settings[REPORT_POS_TYPE_SETTING_ID] = statusReportSetting & REPORT_POS_TYPE_SETTING_BIT;
    //

    for (std::vector<ConfigGroup>::iterator it = ConfigMap.begin(); it != ConfigMap.end(); it++) {
        ConfigGroup &group = *it;
        for (std::vector<ConfigEntry>::iterator it2 = group.entries.begin(); it2 != group.entries.end(); it2++) {
            ConfigEntry entry = *it2;

            double setting = settings[entry.index];

            QMap<Axis, CBaseProperty*> properties;
            switch (entry.type) {
                case Axes:
                    properties = entry.properties;
                    (dynamic_cast<CBoolProperty*>(properties[X]))->setValue(((int) setting) & 1);
                    (dynamic_cast<CBoolProperty*>(properties[Y]))->setValue(((int) setting) & 2);
                    (dynamic_cast<CBoolProperty*>(properties[Z]))->setValue(((int) setting) & 4);
                    break;
                case Boolean:
                    CBoolProperty *boolProperty;
                    boolProperty = dynamic_cast<CBoolProperty*>(entry.property);
                    boolProperty->setValue((bool) setting);
                    break;
                case Integer:
                    CIntegerProperty *intProperty;
                    intProperty = dynamic_cast<CIntegerProperty*>(entry.property);
                    intProperty->setValue((int) setting);
                    break;
                case Double:
                    CDoubleProperty *doubleProperty;
                    doubleProperty = dynamic_cast<CDoubleProperty*>(entry.property);
                    doubleProperty->setValue(setting);
                    break;
            }
        }
    }
}

void frmGrblConfigurator::onUpdateClicked()
{
    update();
}

void frmGrblConfigurator::itemChanged(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    CBaseProperty *property = dynamic_cast<CBaseProperty*>(item);

    int entryIndex = property->data(0, Qt::UserRole).toInt();
    Axis axis = Axis::None;
    if (entryIndex > 1000) {
        axis = Axis(entryIndex % 10000);
        entryIndex = entryIndex / 10000;
    }

    if (entryIndex == REPORT_POS_TYPE_SETTING_ID) {
        setSettingsBit(10, REPORT_POS_TYPE_SETTING_BIT, property->getVariantValue().toBool());
    } else
    if (entryIndex == REPORT_BUFFER_SETTING_ID) {
        setSettingsBit(10, REPORT_BUFFER_SETTING_BIT, property->getVariantValue().toBool());
    } else
    if (axis != Axis::None) {
        setSettingsBit(entryIndex, 1 << axis, property->getVariantValue().toBool());
    } else {
        m_currentSettings[entryIndex] = property->getVariantValue().toDouble();
    }
}

void frmGrblConfigurator::update()
{
    setInfo("Updating...", Qt::red);

    connect(m_communicator, SIGNAL(settingsReceived(QMap<int,double>)), this, SLOT(setSettings(QMap<int,double>)));
    QTimer::singleShot(200, this, [this]() {
        m_communicator->sendCommand("$$");
    });
}

void frmGrblConfigurator::setInfo(QString text, QColor color)
{
    ui->lblInfo->setText(text);
    ui->lblInfo->setStyleSheet(QString("color: %1").arg(color.name()));
}

QMap<Axis, CBaseProperty*> frmGrblConfigurator::addAxesProperty(CPropertyHeader *header, ConfigEntry entry)
{
    return {
        {X, addAxisProperty(header, entry, Axis::X)},
        {Y, addAxisProperty(header, entry, Axis::Y)},
        {Z, addAxisProperty(header, entry, Axis::Z)},
    };
}

CBaseProperty* frmGrblConfigurator::addAxisProperty(CPropertyHeader *header, ConfigEntry entry, Axis axis)
{
    // @TODO ...
    QMap<Axis, QString> axisNames = {
        {X, "X"},
        {Y, "Y"},
        {Z, "Z"},
    };

    CBoolProperty* property = new CBoolProperty(
        header,
        QByteArray::fromStdString(QString("#%1_%2").arg(entry.index).arg(axis).toStdString()),
        entry.description.arg(axisNames[axis]),
        0,
        0
    );
    property->setData(0, Qt::UserRole, entry.index * 10000 + axis);

    ui->editor->add(property);

    return property;
}

void frmGrblConfigurator::setSettingsBit(int index, int bit, bool bitValue)
{
    int value = m_currentSettings[index];
    if (bitValue) {
        value |= bit;
    } else {
        value &= ~bit;
    }
    m_currentSettings[index] = value;
}

void frmGrblConfigurator::accept()
{
    if (QMessageBox::question(this, "Saving settings", "Are you sure you want to update machine settings?") == QMessageBox::No) {
        return;
    }

    m_isSaving = true;
    update();
}

void frmGrblConfigurator::findParametersToBeSaved(QMap<int, double> settings)
{
    QMap<int, double> toBeSaved = {};
    for (QMap<int, double>::iterator it = m_currentSettings.begin(); it != m_currentSettings.end(); it++) {
        int index = it.key();
        double value = it.value();

        if (settings[index] != value) {
            toBeSaved[index] = value;
        }
    }

    if (toBeSaved.size() == 0) {
        QDialog::accept();

        return;
    }

    for (QMap<int, double>::iterator it = toBeSaved.begin(); it != toBeSaved.end(); it++) {
        QString command = QString("$%1=%2").arg(it.key()).arg(it.value());
        qDebug() << m_communicator->sendCommand(command);
        qDebug() << "Sending command" << command;
    }

    QDialog::accept();
}

CBaseProperty* frmGrblConfigurator::addBooleanProperty(CPropertyHeader *header, ConfigEntry entry)
{
    CBoolProperty* property = new CBoolProperty(
        header,
        QByteArray::fromStdString(QString("#%1").arg(entry.index).toStdString()),
        entry.description,
        0,
        0
    );
    property->setData(0, Qt::UserRole, entry.index);

    ui->editor->add(property);

    return property;
}

CBaseProperty* frmGrblConfigurator::addIntegerProperty(CPropertyHeader *header, ConfigEntry entry)
{
    CIntegerProperty* property = new CIntegerProperty(
        header,
        QByteArray::fromStdString(QString("#%1").arg(entry.index).toStdString()),
        entry.description,
        0,
        0
    );
    property->setData(0, Qt::UserRole, entry.index);

    ui->editor->add(property);

    return property;
}

CBaseProperty *frmGrblConfigurator::addDoubleProperty(CPropertyHeader *header, ConfigEntry entry)
{
    CDoubleProperty* property = new CDoubleProperty(
        header,
        QByteArray::fromStdString(QString("#%1").arg(entry.index).toStdString()),
        entry.description,
        0,
        0
    );

    property->setData(0, Qt::UserRole, entry.index);

    ui->editor->add(property);

    return property;
}
