#include "control.h"
#include "ui_control.h"

partMainControl::partMainControl(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::partMainControl)
{
    ui->setupUi(this);
}

partMainControl::~partMainControl()
{
    delete ui;
}
