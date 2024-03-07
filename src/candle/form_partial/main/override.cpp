#include "override.h"
#include "ui_override.h"

partMainOverride::partMainOverride(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::partMainOverride)
{
    ui->setupUi(this);
}

partMainOverride::~partMainOverride()
{
    delete ui;
}
