// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#include "qpushbuttonwithmenu.h"
#include <QMenu>
#include <QDebug>
#include <QAbstractButton>

QPushButtonWithMenu::QPushButtonWithMenu(QWidget *parent) : QPushButton(parent)
{
    m_menu = new QMenu(this);
    setMenu(m_menu);
}

QMenu *QPushButtonWithMenu::menu() const
{
    return m_menu;
}

void QPushButtonWithMenu::mousePressEvent(QMouseEvent *e)
{
    if (!isOnArrow(e->pos())) {
        QPushButton::setMenu(nullptr);
    }

    QPushButton::mousePressEvent(e);
    QPushButton::setMenu(m_menu);
}

void QPushButtonWithMenu::mouseReleaseEvent(QMouseEvent *e)
{
    if (!isOnArrow(e->pos())) {
        QPushButton::setMenu(nullptr);
    }

    QPushButton::mouseReleaseEvent(e);
    QPushButton::setMenu(m_menu);
}

bool QPushButtonWithMenu::isOnArrow(const QPoint &pos) const
{
    return pos.x() > width() - 20;
}
