// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "streamer.h"

Streamer::Streamer(QObject *parent) : QObject(parent) {
    reset();
}

void Streamer::reset(int commandIndex)
{
    m_commandIndex = commandIndex;
    m_processedCommandIndex = commandIndex;
}

void Streamer::resetProcessed(int commandIndex)
{
    m_processedCommandIndex = commandIndex;
}

QString Streamer::command()
{
    return m_currentModel->data(m_currentModel->index(m_commandIndex, 1)).toString();
    //m_form->currentModel().data(m_form->currentModel().index(m_streamer->commandIndex(), 1)).toString();
}

void Streamer::advanceCommandIndex()
{
    m_commandIndex++;
}

bool Streamer::isLastCommand()
{
    return m_commandIndex == m_commandsCount - 1;
}

bool Streamer::noMoreCommands()
{
    return m_commandIndex > m_commandsCount - 1;
}

bool Streamer::isLastCommandProcessed()
{
    return m_processedCommandIndex == m_commandsCount - 1;
}

void Streamer::commandSent()
{
    m_currentModel->setData(m_currentModel->index(m_commandIndex, 2), GCodeItem::Sent);
    //        m_form->currentModel().setData(m_form->currentModel().index(m_streamer->commandIndex(), 2), GCodeItem::Sent);
}
