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

void Streamer::advanceCommandIndex()
{
    m_commandIndex++;
}
