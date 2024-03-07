#include "jog.h"
#include "ui_jog.h"

partMainJog::partMainJog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::widgetJog)
{
    ui->setupUi(this);
}

partMainJog::~partMainJog()
{
    delete ui;
}
