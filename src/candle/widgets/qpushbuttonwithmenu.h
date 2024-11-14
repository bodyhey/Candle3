// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef QPUSHBUTTONWITHMENU_H
#define QPUSHBUTTONWITHMENU_H

#include <QPushButton>

class QPushButtonWithMenu : public QPushButton
{
    public:
        QPushButtonWithMenu(QWidget *parent = nullptr);

    protected:
        // bool event(QEvent *e) override;
        // void paintEvent(QPaintEvent *) override;
        // void keyPressEvent(QKeyEvent *) override;
        // void focusInEvent(QFocusEvent *) override;
        // void focusOutEvent(QFocusEvent *) override;
        // void mouseMoveEvent(QMouseEvent *) override;
        bool hitButton(const QPoint &pos) const override;
};

#endif // QPUSHBUTTONWITHMENU_H
