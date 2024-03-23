// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "console.h"
#include "ui_console.h"

partMainConsole::partMainConsole(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::partMainConsole)
{
    ui->setupUi(this);
}

partMainConsole::~partMainConsole()
{
    delete ui;
}

void partMainConsole::append(QString text)
{
    ui->txtConsole->appendPlainText(text);
}

void partMainConsole::clear()
{
    ui->txtConsole->clear();
}

void partMainConsole::send()
{
    QString command = ui->cboCommand->currentText().trimmed();
    ui->cboCommand->clearEditText();

    if (command.isEmpty()) return;

    emit newCommand(command);
}

void partMainConsole::onClearClicked()
{
    clear();
    emit consoleCleared();
}

void partMainConsole::onSendClicked()
{
    send();
}
