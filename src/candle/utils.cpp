#include "utils.h"
#include <QScreen>
#include <QApplication>
#include <QDebug>

void Utils::positionDialog(QWidget *widget, QRect geometry, bool maximized)
{
    QSize screenSize = QGuiApplication::primaryScreen()->availableSize();
    QSize size = geometry.size();
    if (geometry.x() == -1) {
        geometry.setLeft(screenSize.width() / 2 - geometry.width() / 2);
        geometry.setWidth(size.width());
    }
    if (geometry.y() == -1) {
        geometry.setTop(screenSize.height() / 2 - geometry.height() / 2);
        geometry.setHeight(size.height());
    }
    if (geometry.width() == -1 || geometry.x() + geometry.width() > screenSize.width()) {
        geometry.setWidth(screenSize.width() - geometry.x());
    }
    if (geometry.height() == -1 || geometry.y() + geometry.height() > screenSize.height()) {
        geometry.setHeight(screenSize.height() - geometry.y());
    }
    QSize frameSize = widget->frameSize();
    size = frameSize - widget->size();
    widget->move(geometry.x(), geometry.y());
    widget->resize(
        geometry.width() - size.width(),
        geometry.height() - size.height()
    );
    if (maximized) {
        widget->showMaximized();
    }
}

bool Utils::pointInTriangle(QPoint p, QPoint p0, QPoint p1, QPoint p2)
{
    int dX = p.x()-p2.x();
    int dY = p.y()-p2.y();
    int dX21 = p2.x()-p1.x();
    int dY12 = p1.y()-p2.y();
    int d = dY12*(p0.x()-p2.x()) + dX21*(p0.y()-p2.y());
    int s = dY12*dX + dX21*dY;
    int t = (p2.y()-p0.y())*dX + (p0.x()-p2.x())*dY;

    if (d<0) {
        return s<=0 && t<=0 && s+t>=d;
    }

    return s>=0 && t>=0 && s+t<=d;
}

bool Utils::triangleDir(QPoint p0, QPoint p1, QPoint p2)
{
    return (p0.x() - p2.x()) * (p1.y() - p2.y()) - (p1.x() - p2.x()) * (p0.y() - p2.y()) < 0;
}

