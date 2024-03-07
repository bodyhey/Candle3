#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>

namespace Ui {
class partMainControl;
}

class partMainControl : public QWidget
{
    Q_OBJECT

public:
    explicit partMainControl(QWidget *parent = nullptr);
    ~partMainControl();

private:
    Ui::partMainControl *ui;
};

#endif // CONTROL_H
