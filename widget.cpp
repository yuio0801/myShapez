#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <iostream>
#include <iomanip>
#include <QPainter>
bool ifload = 0;

using namespace std;
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    qDebug()<<"进入主界面";
    setFixedSize(500,500);
    setWindowTitle("myShapez");

    ui->newgamebtn->setText("New Game");
    ui->loadbtn->setText("Load");
    ui->storebtn->setText("Store");
    ui->storebtn->setEnabled(false);
    ui->exitbtn->setText("Exit");
    //connect(ui->newgamebtn,&QPushButton::clicked,this,&Widget::close);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_newgamebtn_clicked()
{
    this->hide();
    scene=new GameScene();
    scene->show();
}


void Widget::on_loadbtn_clicked()
{
    //this->hide();
    do{
        filePath = QFileDialog::getOpenFileName(this,"slect file","","*.txt");
        qDebug()<<"selectd file path:"<<filePath;
        savedata.open(filePath.toStdString());
    }while(savedata.fail() || !load_data());
    ui->savedatainfo->setText(filePath);

    /*TODO*/

}

void Widget::on_storebtn_clicked()
{
    this->hide();
    mystore->show();
    mystore->update();
}


void Widget::on_exitbtn_clicked()
{
    this->close();
}

bool Widget::load_data()
{
    return 1;
}
