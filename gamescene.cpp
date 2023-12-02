#include "gamescene.h"
#include "ui_gamescene.h"
#include <QString>
#include <QPainter>
#include <QDebug>
#include <QCloseEvent>
#include <QTimer>

//当前地图最大长宽
int Maxx = 300, Maxy = 300;
int Task = 1;
int Mapsize = 10;
QString Facility_name[10]={"mine", "harvestor", };

GameScene::GameScene(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameScene)
{
    ui->setupUi(this);

    //初始化
    setFixedSize(1000,600);
    setWindowTitle("Task "+QString::number(Task));
    GameTime = 0;
    ui->TimeLabel->setText("GameTime:0s");
    timer0 = new QTimer;
    timer1 = new QTimer;

    connect(timer0, &QTimer::timeout, this, [=](){
            ++GameTime;
            ui->TimeLabel->setText("GameTime:" + QString::number(GameTime) + "s");
    });
    timer0->start(1000);

    //Block *test = new Block();
    for(int i = 0; i < Mapsize; ++i)
    {
        for(int j = 0; j < Mapsize; ++j)
        {
            block[i][j] = new Block(ui->WorkPlace, i, j);
        }
    }

    connect(timer1,&QTimer::timeout,this, [this]{this->update();});//刷新屏幕
    timer1->start(100);

    Mine *tmp = new Mine(parent, block[0][0], 0);
    tmp->settle();
    //delete block[0][0];
    //block[0][0]=NULL;
    //qDebug()<<"delete asda";
}

GameScene::~GameScene()
{
    delete ui;
}


void GameScene::paintEvent(QPaintEvent* event)
{

    QPainter painter(this);
    QPixmap pix;
    QFont font;
    for(int i = 0; i < Mapsize; ++i)
    {
        for(int j = 0; j < Mapsize; ++j)
        {
            //if(i==0&&j==0) continue;
            //qDebug()<<i<<" "<<j<<" "<<block[i][j];
            if(block[i][j])
            draw_block(painter, block[i][j]);
            //qDebug()<<"asd";
        }
    }


}

void GameScene::closeEvent(QCloseEvent *event)
{

}
void GameScene::keyPressEvent(QKeyEvent *event)
{

}

void GameScene::draw_block(QPainter &painter, Block *bl)
{
    painter.drawPixmap(bl->pos_x, bl->pos_y, bl->icon);
    if(bl->facility)
    {
        painter.drawPixmap(bl->pos_x, bl->pos_y, bl->facility->icon);
    }
//    for(int i=0;i<Monster_Num;i++)
//    {

//        if(pmonster[i].cur_HP>0)
//        {

//            painter.drawPixmap(pmonster[i].pos_x,pmonster[i].pos_y,*(pmonster[i].picon));
//            //血条
//            QBrush trans_brush( QColor(0,0,0,20) );//把刷子设置为透明
//            painter.setBrush(trans_brush);//应用刷子


//            painter.drawRect(pmonster[i].pos_x,pmonster[i].pos_y,pmonster[i].icon2.width(),pmonster[i].icon2.height()*0.12);//绘制矩形
//            float rate=1.0*pmonster[i].cur_HP/pmonster[i].HP;//计算比例
//            QBrush red_brush( QColor("#F20900") );//把刷子设置为红色
//            painter.setBrush(red_brush);//应用刷子
//            painter.drawRect(pmonster[i].pos_x,pmonster[i].pos_y,rate*pmonster[i].icon2.width(),pmonster[i].icon2.height()*0.12);//绘制矩形
//        }
//        else if(pmonster[i].ExpFlag)
//        {
//            painter.drawPixmap(pmonster[i].pos_x,pmonster[i].pos_y,pmonster[i].exp_icon);
//        }
//    }
}
