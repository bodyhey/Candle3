#include "utils.h"
#include <QScreen>
#include <QApplication>
#include <QDebug>

void Utils::positionDialog(QWidget *widget, QRect geometry) {
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
    qDebug() << "Frame size" << size;
    widget->move(geometry.x(), geometry.y());
    widget->resize(
        geometry.width() - size.width(),
        geometry.height() - size.height()
    );
}
