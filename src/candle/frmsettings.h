// This file is a part of "Candle" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich

#ifndef FRMSETTINGS_H
#define FRMSETTINGS_H

#include <QDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QSettings>
#include <QGroupBox>
#include <QVector3D>
#include "globals.h"
#include "colorpicker.h"
#include "config/configuration.h"

namespace Ui {
class frmSettings;
}

// TODO: Add overriding bounds settings
class frmSettings : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int spindleSpeedMin READ spindleSpeedMin WRITE setSpindleSpeedMin)
    Q_PROPERTY(int spindleSpeedMax READ spindleSpeedMax WRITE setSpindleSpeedMax)
    Q_PROPERTY(int laserPowerMin READ laserPowerMin WRITE setLaserPowerMin)
    Q_PROPERTY(int laserPowerMax READ laserPowerMax WRITE setLaserPowerMax)
    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize)
    Q_PROPERTY(bool ignoreErrors READ ignoreErrors WRITE setIgnoreErrors)
    Q_PROPERTY(bool autoLine READ autoLine WRITE setAutoLine)
    Q_PROPERTY(QString language READ language WRITE setLanguage)
    Q_PROPERTY(int rapidSpeed READ rapidSpeed WRITE setRapidSpeed)
    Q_PROPERTY(int acceleration READ acceleration WRITE setAcceleration)
    Q_PROPERTY(QVector3D machineBounds READ machineBounds WRITE setMachineBounds)
    Q_PROPERTY(bool homingEnabled READ homingEnabled WRITE setHomingEnabled)
    Q_PROPERTY(bool softLimitsEnabled READ softLimitsEnabled WRITE setSoftLimitsEnabled)

public:
    explicit frmSettings(QWidget *parent, Configuration &configuration);
    ~frmSettings();

    Ui::frmSettings *ui;

    int exec();
    void undo();

    void addCustomSettings(QGroupBox *box);

    int spindleSpeedMin();
    void setSpindleSpeedMin(int speed);
    int spindleSpeedMax();
    void setSpindleSpeedMax(int speed);
    int laserPowerMin();
    void setLaserPowerMin(int value);
    int laserPowerMax();
    void setLaserPowerMax(int value);
    int rapidSpeed();
    void setRapidSpeed(int rapidSpeed);
    int acceleration();
    void setAcceleration(int acceleration);
    int queryStateTime();
    void setQueryStateTime(int queryStateTime);
//    QList<ColorPicker*> colors();
//    QColor colors(QString name);
    int fontSize();
    void setFontSize(int fontSize);
    bool ignoreErrors();
    void setIgnoreErrors(bool value);
    bool autoLine();
    void setAutoLine(bool value);
    QString language();
    void setLanguage(QString language);
    QVector3D machineBounds();
    void setMachineBounds(QVector3D bounds);
    bool homingEnabled();
    void setHomingEnabled(bool homing);
    bool softLimitsEnabled();
    void setSoftLimitsEnabled(bool softLimits);    
    bool referenceXPlus();
    void setReferenceXPlus(bool value);
    bool referenceYPlus();
    void setReferenceYPlus(bool value);
    bool referenceZPlus();
    void setReferenceZPlus(bool value);

signals:
    void settingsSetToDefault();

protected:
    void showEvent(QShowEvent *se);

private slots:
    void onScrollBarValueChanged(int value);
    void onCmdSerialPortsRefreshClicked();
    void onCmdOKClicked();
    void onCmdCancelClicked();
    void on_cboToolType_currentIndexChanged(int index);
    void on_listCategories_currentRowChanged(int currentRow);
    void onCmdDefaultsClicked();
    void on_cboFontSize_currentTextChanged(const QString &arg1);
    void onDrawModeVectorsToggled(bool checked);
    void onConnectionModeChanged(int);

private:
    Configuration &m_configuration;

    void searchForSerialPorts();

    QList<double> m_storedValues;
    QList<bool> m_storedChecks;
    QList<QString> m_storedCombos;
    QList<QColor> m_storedColors;
    QList<QString> m_storedTextBoxes;
    QList<QString> m_storedPlainTexts;
    bool m_animatingScrollBox;
    bool m_scrollingManuallyScrollBox;

    QIntValidator m_intValidator;

    QList<QWidget*> m_customSettings;

    int m_acceleration;
    int m_rapidSpeed;
    QVector3D m_machineBounds;
    bool m_homingEnabled;
    bool m_softLimitsEnabled;
    void initializeWidgets();
    void resetToDefaults();
    void applySettings();
};

#endif // FRMSETTINGS_H
