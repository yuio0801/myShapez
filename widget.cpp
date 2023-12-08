#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <iostream>
#include <iomanip>
#include <QPainter>
bool ifload = 0;
QString filePath;
int earned = 0;
extern bool isnew;

//当前地图最大长宽
int Maxx = 10, Maxy = 10;
int mineral_value[5]={20, 10, 50, 30, 50};

int num_mine[2]={3,3};
int num_base = 2;

using namespace std;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    qDebug()<<"进入主界面";
    setFixedSize(500,500);
    setWindowTitle("myShapez");

    mystore = new Store;

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

    ifstream savedata;
    if(filePath.isEmpty())
    {
        filePath = "C:\\Users\\ouyan\\Desktop\\myShapez\\savedata.txt";
    }
    savedata.open(filePath.toStdString());

    string strFileData = "";
    int line = 0;
    char tmpLineData[1024] = {0};
    while(savedata.getline(tmpLineData, sizeof(tmpLineData)))
    {
        if(line == 0)
        {
            int d;
            sscanf(tmpLineData, "%d",&d);
            if(d == 0)
            {
                isnew = true;
            }
            else
            {
                isnew = false;
            }
        }
        if(line == 1)
        {
            sscanf(tmpLineData, "%d %d",&Maxx,&Maxy);
        }
        if(line == 2)
        {
            sscanf(tmpLineData, "%d %d %d %d %d",&mineral_value[0],&mineral_value[1],&mineral_value[2],&mineral_value[3],&mineral_value[4]);
        }
        if(line == 3)
        {
            sscanf(tmpLineData, "%d %d",&num_mine[0],&num_mine[1]);
        }
        if(line == 4)
        {
            sscanf(tmpLineData, "%d",&num_base);
        }
        if(line == 5)
        {
            sscanf(tmpLineData, "%d",&earned);
        }
        line++;
        if(line == 6) break;
    }
}

void Widget::on_storebtn_clicked()
{
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
