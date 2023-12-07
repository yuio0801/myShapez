#include "store.h"
#include "ui_store.h"

extern bool ifload;
Store::Store(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Store)
{
    ui->setupUi(this);
}

Store::~Store()
{
    delete ui;
}
