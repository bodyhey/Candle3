#include "utils.h"
#include <QScreen>
#include <QApplication>
#include <QDebug>

void Utils::positionDialog(QWidget *widget, QRect geometry) {
    QSize screenSize = QGuiApplication::primaryScreen()->size();
    QSize size = geometry.size();
    if (geometry.x() == -1) {
        geometry.setLeft(screenSize.width() / 2 - geometry.width() / 2);
        geometry.setWidth(size.width());
    }
    if (geometry.y() == -1) {
        geometry.setTop(screenSize.height() / 2 - geometry.height() / 2);
        geometry.setHeight(size.height());
    }
    if (geometry.width() == -1) {
        geometry.setWidth(screenSize.width() - geometry.x());
    }
    if (geometry.height() == -1) {
        geometry.setHeight(screenSize.height() - geometry.y());
    }
    widget->setGeometry(geometry);
}
