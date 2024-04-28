#include "partmainoverride.h"
#include "ui_partmainoverride.h"

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
