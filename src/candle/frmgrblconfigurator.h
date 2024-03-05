#ifndef FRMGRBLCONFIGURATOR_H
#define FRMGRBLCONFIGURATOR_H

#include <QDialog>

namespace Ui {
class frmGrblConfigurator;
}

class frmGrblConfigurator : public QDialog
{
    Q_OBJECT

public:
    explicit frmGrblConfigurator(QWidget *parent = nullptr);
    ~frmGrblConfigurator();

private:
    Ui::frmGrblConfigurator *ui;
};

#endif // FRMGRBLCONFIGURATOR_H
