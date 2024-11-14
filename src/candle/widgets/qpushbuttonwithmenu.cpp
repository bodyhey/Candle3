// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "qpushbuttonwithmenu.h"
#include <QMenu>

QPushButtonWithMenu::QPushButtonWithMenu(QWidget *parent) : QPushButton(parent)
{
    setMenu(new QMenu(this));
}

bool QPushButtonWithMenu::hitButton(const QPoint &pos) const
{
    return true;
}
