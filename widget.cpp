#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <iostream>
#include <iomanip>
#include <QPainter>
bool ifload = 0;
QString filePath;
int earned = 0;


//当前地图最大长宽
int Maxx = 10, Maxy = 10;
int mineral_value[4]={20, 10, 50, 30};

int num_mine[2]={2,2};
int num_base = 1;

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

}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_newgamebtn_clicked()
{
    this->hide();
    scene=new GameScene();
    connect(scene,&GameScene::close,this,&Widget::show);
    scene->show();
}


void Widget::on_loadbtn_clicked()
{
    //this->hide();
    filePath = QFileDialog::getOpenFileName(this,"slect file","","savedata.txt");
    qDebug()<<"selectd file path:"<<filePath;
    savedata.open(filePath.toStdString());

    ui->savedatainfo->setText(filePath);
    ui->newgamebtn->setText("Continue");
    ui->storebtn->setEnabled(true);
    ifload = 1;
    mystore->update();
}

void Widget::on_storebtn_clicked()
{
    this->hide();
    mystore->update();
    mystore->show();

}


void Widget::on_exitbtn_clicked()
{
    this->close();
}

bool Widget::load_data()
{
    return 1;
}
