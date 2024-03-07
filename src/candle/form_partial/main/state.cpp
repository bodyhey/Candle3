#include "state.h"
#include "ui_state.h"

partMainState::partMainState(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::State)
{
    ui->setupUi(this);
}

partMainState::~partMainState()
{
    delete ui;
}
