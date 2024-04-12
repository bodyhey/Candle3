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

public:
    explicit frmSettings(QWidget *parent, Configuration &configuration);
    ~frmSettings();

    Ui::frmSettings *ui;

    int exec();
    void undo();

    void addCustomSettings(QGroupBox *box);

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

    void initializeWidgets();
    void resetToDefaults();
    void applySettings();
};

#endif // FRMSETTINGS_H
