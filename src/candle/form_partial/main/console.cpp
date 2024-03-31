// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "console.h"
#include "ui_console.h"
#include <QScrollBar>

partMainConsole::partMainConsole(QWidget *parent, const ConfigurationConsole &configurationConsole)
    : QWidget(parent)
    , ui(new Ui::partMainConsole)
    , m_configurationConsole(configurationConsole)
{
    ui->setupUi(this);

    if (m_configurationConsole.darkBackgroundMode()) {
        applyDarkBackgroundMode();
    }

    #ifndef UNIX
        ui->cboCommand->setStyleSheet("QComboBox {padding: 2;} QComboBox::drop-down {width: 0; border-style: none;} QComboBox::down-arrow {image: url(noimg);	border-width: 0;}");
    #endif

    ui->cboCommand->setMinimumHeight(ui->cboCommand->height());
    ui->cmdClearConsole->setFixedHeight(ui->cboCommand->height());
    ui->cmdCommandSend->setFixedHeight(ui->cboCommand->height());

    ui->cboCommand->setAutoCompletion(m_configurationConsole.commandAutoCompletion());

    // restore command history
    ui->cboCommand->addItems(m_configurationConsole.commandHistory());
    ui->cboCommand->setCurrentIndex(-1);
}

partMainConsole::~partMainConsole()
{
    delete ui;
}

void partMainConsole::applyDarkBackgroundMode()
{
    ui->txtConsole->setStyleSheet("QPlainTextEdit { background-color: #000000; color: #FFFFFF; }");
}

int partMainConsole::append(QString text)
{
    ui->txtConsole->appendPlainText(text);

    return ui->txtConsole->document()->blockCount() - 1;
}

int partMainConsole::append(CommandAttributes commandAttributes)
{
    int blockNumber = append(">> " + commandAttributes.command);

    QTextBlock block = lastBlock();
    BlockData *blockData = new BlockData(block.blockNumber(), commandAttributes.commandIndex);
    block.setUserData(blockData);

    return blockNumber;
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
