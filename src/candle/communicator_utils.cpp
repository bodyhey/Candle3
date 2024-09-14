// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "communicator.h"

bool Communicator::dataIsFloating(QString data)
{
    QStringList ends;

    ends << "Reset to continue";
    ends << "'$H'|'$X' to unlock";
    ends << "ALARM: Soft limit";
    ends << "ALARM: Hard limit";
    ends << "Check Door";

    foreach (QString str, ends) {
        if (data.contains(str)) return true;
    }

    return false;
}

bool Communicator::dataIsEnd(QString data)
{
    QStringList ends;

    ends << "ok";
    ends << "error";

    foreach (QString str, ends) {
        if (data.contains(str)) return true;
    }

    return false;
}
