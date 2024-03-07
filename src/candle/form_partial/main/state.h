#ifndef STATE_H
#define STATE_H

#include <QWidget>

namespace Ui {
class State;
}

class partMainState : public QWidget
{
    Q_OBJECT

public:
    explicit partMainState(QWidget *parent = nullptr);
    ~partMainState();

private:
    Ui::State *ui;
};

#endif // STATE_H
