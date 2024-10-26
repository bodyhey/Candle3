// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef UTILS_H
#define UTILS_H

#include <QDesktopWidget>
#include <QApplication>

class Utils {
    public:
        static void positionDialog(QWidget *widget, QRect geometry) {
            if (geometry.x() == -1 && geometry.y() == -1) {
                QRect screenGeometry = QApplication::desktop()->screenGeometry();
                widget->setGeometry(QRect(
                    screenGeometry.width() / 2 - geometry.width() / 2,
                    screenGeometry.height() / 2 - geometry.height() / 2,
                    geometry.width(),
                    geometry.height()
                    ));
            } else {
                widget->setGeometry(geometry);
            }
        }
};

#endif // UTILS_H
