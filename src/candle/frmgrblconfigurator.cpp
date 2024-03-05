#include "frmgrblconfigurator.h"
#include "ui_frmgrblconfigurator.h"

frmGrblConfigurator::frmGrblConfigurator(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::frmGrblConfigurator)
{
    ui->setupUi(this);
}

frmGrblConfigurator::~frmGrblConfigurator()
{
    delete ui;
}
