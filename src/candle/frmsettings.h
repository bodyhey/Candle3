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
#include "config/configuration.h"
#include "utils/validators.h"

namespace Ui {
class frmSettings;
}

// TODO: Add overriding bounds settings
class frmSettings : public QDialog
{
    Q_OBJECT

public:
    explicit frmSettings(QWidget *parent, Configuration &configuration);
    ~frmSettings();

    Ui::frmSettings *ui;

    int exec();
    void addCustomSettings(QGroupBox *box);

signals:
    void settingsSetToDefault();

private slots:
    void onScrollBarValueChanged(int value);
    void onCmdSerialPortsRefreshClicked();
    void onCmdOKClicked();
    void onCmdCancelClicked();
    void on_cboToolType_currentIndexChanged(int index);
    void on_listCategories_currentRowChanged(int currentRow);
    void onCmdDefaultsClicked();
    void onDrawModeVectorsToggled(bool checked);
    void onArcApproximationModeChanged(bool checked);
    void onConnectionModeChanged(int);

private:
    Configuration &m_configuration;

    QList<double> m_storedValues;
    QList<bool> m_storedChecks;
    QList<QString> m_storedCombos;
    QList<QColor> m_storedColors;
    QList<QString> m_storedTextBoxes;
    QList<QString> m_storedPlainTexts;
    bool m_animatingScrollBox;
    bool m_scrollingManuallyScrollBox;

    QIntValidator m_intValidator;
    QCommaSeparatedIntValidator m_commaSeparatedIntValidator;
    QCommaSeparatedDoubleValidator m_commaSeparatedDoubleValidator;

    QList<QWidget*> m_customSettings;

    void searchForSerialPorts();

    void undo();
    void initializeWidgets();
    void resetToDefaults();
    void applySettings();

    int invalidWidgets;
    void widgetValidity(bool valid);
};

#endif // FRMSETTINGS_H
