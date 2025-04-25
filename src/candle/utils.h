// This file is a part of "G-Pilot (formerly Candle)" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich
// Copyright 2024 BTS

#ifndef UTILS_H
#define UTILS_H

#include <QWidget>

class Utils {
    public:
        static void positionDialog(QWidget *widget, QRect geometry, bool maximized);
        static QRect getDialogGeometry(QWidget *widget);
        static bool pointInTriangle(QPoint p, QPoint p0, QPoint p1, QPoint p2);
        static bool triangleDir(QPoint p0, QPoint p1, QPoint p2);
};

#endif // UTILS_H
