#ifndef JOG_H
#define JOG_H

#include "globals.h"
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

private slots:
    void onCmdYPlusPressed();
    void onCmdYPlusReleased();
    void onCmdYMinusPressed();
    void onCmdYMinusReleased();
    void onCmdXPlusPressed();
    void onCmdXPlusReleased();
    void onCmdXMinusPressed();
    void onCmdXMinusReleased();
    void onCmdZPlusPressed();
    void onCmdZPlusReleased();
    void onCmdZMinusPressed();
    void onCmdZMinusReleased();
    void onCmdStopClicked();
    void onChkKeyboardControlToggled(bool checked);

signals:
    void jog(JoggindDir dir);
    void command(GRBLCommand command);
    void stop();

};

#endif // JOG_H
