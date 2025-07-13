#include "example.h"
#include "ui_example.h"

Example::Example(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Example)
{
    ui->setupUi(this);
}

Example::~Example()
{
    delete ui;
}
