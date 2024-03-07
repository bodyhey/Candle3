#ifndef OVERRIDE_H
#define OVERRIDE_H

#include <QWidget>

namespace Ui {
class partMainOverride;
}

class partMainOverride : public QWidget
{
    Q_OBJECT

public:
    explicit partMainOverride(QWidget *parent = nullptr);
    ~partMainOverride();

private:
    Ui::partMainOverride *ui;
};

#endif // OVERRIDE_H
