// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "partmainconsole.h"
#include "ui_partmainconsole.h"
#include <QScrollBar>
#include <QCompleter>

partMainConsole::partMainConsole(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::partMainConsole)
{
    ui->setupUi(this);

    #ifndef UNIX
        ui->cboCommand->setStyleSheet("QComboBox {padding: 2;} QComboBox::drop-down {width: 0; border-style: none;} QComboBox::down-arrow {image: url(noimg);	border-width: 0;}");
    #endif

    ui->cboCommand->setMinimumHeight(ui->cboCommand->height());
    ui->cmdClearConsole->setFixedHeight(ui->cboCommand->height());
    ui->cmdCommandSend->setFixedHeight(ui->cboCommand->height());
}

void partMainConsole::initialize(ConfigurationConsole &configurationConsole)
{
    m_configurationConsole = &configurationConsole;

    if (m_configurationConsole->commandAutoCompletion()) {
        QCompleter completer = ui->cboCommand->completer();
        completer.setCompletionMode(QCompleter::InlineCompletion);
    } else {
        ui->cboCommand->setCompleter(nullptr);
    }

    //m_completerModel.setCommands(m_configurationConsole->commandHistory());
    ui->cboCommand->addItems(m_configurationConsole->commandHistory());
    ui->cboCommand->setCurrentIndex(-1);

    if (m_configurationConsole->darkBackgroundMode()) {
        applyDarkBackgroundMode();
    }
}

partMainConsole::~partMainConsole()
{
    delete ui;
}

void partMainConsole::applyDarkBackgroundMode()
{
    ui->txtConsole->setStyleSheet("QPlainTextEdit { background-color: #000000; color: #FFFFFF; }");
}

void partMainConsole::append(QString text)
{
    ui->txtConsole->appendPlainText(text);
}

void partMainConsole::append(CommandAttributes commandAttributes)
{
    append(">> " + commandAttributes.commandLine);

    QTextBlock block = lastBlock();
    BlockData *blockData = new BlockData(block.blockNumber(), commandAttributes.commandIndex);
    block.setUserData(blockData);
}

void partMainConsole::appendFiltered(CommandAttributes commandAttributes)
{
    switch (commandAttributes.source) {
        case CommandSource::System:
            if (!m_configurationConsole->showSystemCommands()) return;
            break;
        case CommandSource::Program:
        case CommandSource::ProgramAdditionalCommands:
            if (!m_configurationConsole->showProgramCommands()) return;
            break;
        case CommandSource::Console:
        case CommandSource::GeneralUI:
            if (!m_configurationConsole->showUiCommands()) return;
            break;
    }

    append(commandAttributes);
}

void partMainConsole::appendResponse(int consoleIndex, QString command, QString response)
{
    bool scrolledDown = this->isScrolledToEnd();

    QTextBlock block = ui->txtConsole->document()->findBlockByNumber(consoleIndex);
    if (!block.isValid() || block.text() != command) return;

    QTextCursor cursor(block);

    // Update text block numbers
    // @TODO response is added as multiple lines, do we have to do this?
    // int blocksAdded = response.count("; ");

    // if (blocksAdded > 0) for (int i = 0; i < m_communicator->m_commands.count(); i++) {
    //         if (m_communicator->m_commands[i].consoleIndex != -1) m_communicator->m_commands[i].consoleIndex += blocksAdded;
    //     }

    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::EndOfBlock);
    // @TODO response was added as multiple lines, do we have to do this?
    // cursor.insertText(" < " + QString(response).replace("; ", "\r\n"));
    cursor.insertText(" < " + QString(response));
    cursor.endEditBlock();

    if (scrolledDown) this->scrollToEnd();
}

void partMainConsole::appendResponse(CommandAttributes commandAttributes)
{
    QTextDocument *document = ui->txtConsole->document();
    QTextBlock block = document->lastBlock();
    int blocksCounter = 0;
    do {
        BlockData *blockData = static_cast<BlockData *>(block.userData());
        if (blockData && blockData->commandIndex() == commandAttributes.commandIndex) {
            QTextCursor cursor(block);

            cursor.beginEditBlock();
            cursor.movePosition(QTextCursor::EndOfBlock);
            // @TODO response was added as multiple lines, do we have to do this?
            // cursor.insertText(" < " + QString(response).replace("; ", "\r\n"));
            cursor.insertHtml(QString("&nbsp;<span style='color: gray; font-size: 90%'>(%1)</span> ").arg(commandAttributes.response));
            cursor.endEditBlock();

            // do we need it anymore?
            block.setUserData(nullptr);

            return;
        }
        // let's check max 50 blocks from the end
        if (blocksCounter++ > 50) {
            return;
        }

        block = block.previous();
    } while (block.isValid());
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

    m_configurationConsole->setCommandHistory(ui->cboCommand->items());

    emit newCommand(command);
}

bool partMainConsole::isScrolledToEnd()
{
    return ui->txtConsole->verticalScrollBar()->value()
                        == ui->txtConsole->verticalScrollBar()->maximum();
}

void partMainConsole::scrollToEnd()
{
    ui->txtConsole->verticalScrollBar()->setValue(
        ui->txtConsole->verticalScrollBar()->maximum());
}

QTextBlock partMainConsole::lastBlock()
{
    return ui->txtConsole->document()->lastBlock();
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
