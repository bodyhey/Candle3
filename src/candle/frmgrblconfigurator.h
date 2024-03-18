#ifndef FRMGRBLCONFIGURATOR_H
#define FRMGRBLCONFIGURATOR_H

#include <QDialog>
#include "communicator.h"

namespace Ui {
class frmGrblConfigurator;
}

class frmGrblConfigurator : public QDialog
{
    Q_OBJECT

    public:
        explicit frmGrblConfigurator(QWidget *parent, Communicator *communicator);
        ~frmGrblConfigurator();

    private:
        Ui::frmGrblConfigurator *ui;
        Communicator *m_communicator;

    private slots:
        void setSettings(QMap<int, double>);
};

#endif // FRMGRBLCONFIGURATOR_H
