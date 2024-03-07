#ifndef JOG_H
#define JOG_H

#include <QWidget>

namespace Ui {
class widgetJog;
}

class partMainJog : public QWidget
{
    Q_OBJECT

public:
    explicit partMainJog(QWidget *parent = nullptr);
    ~partMainJog();

private:
    Ui::widgetJog *ui;
};

#endif // JOG_H
