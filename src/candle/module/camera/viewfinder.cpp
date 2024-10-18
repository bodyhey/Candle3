#include "viewfinder.h"
#include <QPainter>
#include <QPaintEvent>
#include <QImage>

ViewFinder::ViewFinder(QWidget* parent)
    :QLabel(parent)
{

}

ViewFinder::~ViewFinder()
{

}

void ViewFinder::paintEvent(QPaintEvent* event)
{
   QLabel::paintEvent(event);

   QString rect_path = ":/images/face_detect_bg.png";
   QImage img(rect_path);



   QPainter painter(this);
   QPen pen = painter.pen();
   pen.setColor(Qt::red);
   pen.setWidth(5);
   painter.setPen(pen);
   painter.drawLine(this->width() / 2, 0, this->width() / 2, this->height());
   painter.drawLine(0, this->height() / 2, this->width(), this->height() / 2);
   // painter.drawImage(
   //     (this->width() - img.width()) / 2,
   //     (this->height() - img.height()) / 2,
   //     img
   //     );
}
