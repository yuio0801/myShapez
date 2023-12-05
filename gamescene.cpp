#include "gamescene.h"
#include "ui_gamescene.h"
#include <QString>
#include <QPainter>
#include <QDebug>
#include <QCloseEvent>
#include <QTimer>

const int SIZE = 50;

int Task = 1;
Block *block[30][20];
//当前地图最大长宽
int Maxx = 10, Maxy = 10;
QString Facility_name[10]={"error", "harvestor", "base", "conveyer",};
extern QString filePath;
int mineral_value[4]={20, 10, };
//此时Pos中储存id_x, id_y
unordered_map<Mine *, Block *> mine_all;
unordered_map<Harvestor *, Block *> harvestor_all;
unordered_map<Base *, Block *> base_all;
unordered_map<Conveyer *, Block *> conveyer_all;

map<int, Mineral *> mineral_all;
int mineral_cnt = 0;

int money;
int mineral_num[4];

int harvestor_speed = 15;
int base_speed = 10;
int conveyer_speed = 20;

//int Harvestor::speed = harvestor_speed;
//int Base::speed = 0xffff;
//int Conveyer::speed = 20;

int settletype = -1;
Conveyer *Contobesettle = NULL;
bool settleable = false;
int curdir = 0;

GameScene::GameScene(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameScene)
{
    ui->setupUi(this);

    //初始化
    setFixedSize(1700,1000);
    setWindowTitle("Task "+QString::number(Task));
    if(!filePath.isNull())
    {
        Load();
    }
    GameTime = 0;
    ui->TimeLabel->setText("GameTime:0s");
    timer0 = new QTimer;
    timer1 = new QTimer;
    timer2 = new QTimer;
    timer3 = new QTimer;
    timer4 = new QTimer;
    connect(timer0, &QTimer::timeout, this, [=](){
            ++GameTime;
            ui->TimeLabel->setText("GameTime:" + QString::number(GameTime) + "s");
    });
    connect(timer0, &QTimer::timeout, this, [=](){
        ui->MoneyLabel->setText("Money:" + QString::number(money));
    });
    timer0->start(1000);

    //Block *test = new Block();
    for(int i = 0; i < Maxy; ++i)
    {
        for(int j = 0; j < Maxx; ++j)
        {
            block[i][j] = new Block(ui->WorkPlace, i, j);
        }
    }
    connect(timer1, &QTimer::timeout, this, [this]{this->create_mineral();});//Mine
    timer1->start(harvestor_speed);

    connect(timer2, &QTimer::timeout, this, [this]{this->move_mineral();});//Conveyer
    timer2->start(conveyer_speed);

//    connect(timer3, &QTimer::timeout, this, [this]{this->remove_mineral;});//Base
//    timer3->start(base_speed);

    connect(timer4, &QTimer::timeout, this, [this]{this->update();});//刷新屏幕
    timer4->start(100);

    //放置
    connect(ui->Consettletn, &QPushButton::clicked, this, [&, this]{settletype = 3; this->settle_mode();});


    Mine *tmp = new Mine(parent, block[0][0], 0);
    tmp->settle();
    Harvestor *hav = new Harvestor(parent, block[0][0], 1);
    if(hav->settle_available())
    {
        hav->settle();
    }
    Base *base = new Base(parent, block[2][5]);
    if(base->settle_available())
    {
        base->settle();
    }
    Conveyer *conv = new Conveyer(parent, block[1][0], 1);
    if(conv->settle_available())
    {
        conv->settle();
    }
    conv = new Conveyer(parent, block[2][0], 6);
    if(conv->settle_available())
    {
        conv->settle();
    }
    conv = new Conveyer(parent, block[2][1], 2);
    if(conv->settle_available())
    {
        conv->settle();
    }
    conv = new Conveyer(parent, block[2][2], 2);
    if(conv->settle_available())
    {
        conv->settle();
    }
    conv = new Conveyer(parent, block[2][3], 2);
    if(conv->settle_available())
    {
        conv->settle();
    }
    else
    {
        delete conv;
    }
    conv = new Conveyer(parent, block[2][4], 3);
    if(conv->settle_available())
    {
        conv->settle();
    }
    //delete block[0][0];
    //block[0][0]=NULL;
    //qDebug()<<"delete asda";
}

GameScene::~GameScene()
{
    delete ui;
}
void GameScene::Load()
{
    return ;
}
void GameScene::create_mineral()
{
    for(auto pai :harvestor_all)
    {
        Harvestor *hav = pai.first;
        Block *bl = pai.second;
        assert(bl->mine);
        int dir = hav->out_dir;
        Mineral *tmp = new Mineral(NULL, bl, bl->middle.pos_x, bl->middle.pos_y, bl->mine->id_mineral);
        switch(dir)
        {
        case 0:tmp->p.pos_y -= SIZE / 2; break;
        case 1:tmp->p.pos_x += SIZE / 2; break;
        case 2:tmp->p.pos_y += SIZE / 2; break;
        case 3:tmp->p.pos_x -= SIZE / 2; break;
        default:assert(0);
        }
        bool flag = true;
        if(!tmp->moveable(tmp->p.pos_x, tmp->p.pos_y, dir))
        {
            flag = false;
        }
        if(!hav->Mineral_out(tmp))
        {
            flag = false;
        }
        if(!flag)
        {
            delete tmp;
        }
        else
        {
            Block *nxt = NULL;
            switch(dir)
            {
            case 0:nxt = block[bl->id_x][bl->id_y - 1];break;
            case 1:nxt = block[bl->id_x + 1][bl->id_y];break;
            case 2:nxt = block[bl->id_x][bl->id_y + 1];break;
            case 3:nxt = block[bl->id_x - 1][bl->id_y];break;
            default:assert(0);
            }
            Facility *nfac = nxt->facility;
            assert(nfac->type == 3);
            mineral_cnt++;
            mineral_all.insert(make_pair(mineral_cnt, tmp));
            qDebug()<<QString::number(mineral_cnt);
            assert(nfac->mineral_inque == NULL);
            nfac->Mineral_in(tmp);
        }
    }
}

void GameScene::move_mineral()
{
    for(auto pai :conveyer_all)
    {
        Conveyer *con = pai.first;
        Block *bl = pai.second;
        assert(bl);
        con->Mineral_move();
    }
    /*
    for(auto pai:mineral_all)
    {
        int idx = pai.first;
        Mineral* tmp = pai.second;
        Block* bl = tmp->bl;
        Facility* fac = tmp->bl->facility;
        if(fac->type)
        {

        }
    }*/
}
void GameScene::settle_mode()
{
    if(settletype == 3)
    {
        Contobesettle = new Conveyer(NULL, block[0][0], 0);

    }
}
void GameScene::paintEvent(QPaintEvent* event)
{

    QPainter painter(this);
    QPixmap pix;
    QFont font;
    for(int i = 0; i < Maxy; ++i)
    {
        for(int j = 0; j < Maxx; ++j)
        {
            //if(i==0&&j==0) continue;
            //qDebug()<<i<<" "<<j<<" "<<block[i][j];
            if(block[i][j])
            draw_block(painter, block[i][j]);
            //qDebug()<<"asd";
        }
    }
    for(auto pai:mineral_all)
    {
        Mineral *tmp = pai.second;
        painter.drawPixmap(tmp->p.pos_x - SIZE / 2, tmp->p.pos_y - SIZE / 2, tmp->icon);
    }
    if(settletype != -1)
    {
        Block *pos;QPixmap icon;
        icon.load(":/res/arrow" + QString::number(settleable));
        if(icon.isNull())
            qDebug()<<"open arrow icon fail";
        if(settletype == 3)
        {
            pos = Contobesettle->bl;
            painter.drawPixmap(pos->p.pos_x, pos->p.pos_y, icon);
            if(Contobesettle)
            painter.drawPixmap(1650, 950, Contobesettle->icon);
        }
    }

}

void GameScene::closeEvent(QCloseEvent *event)
{

}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape)
    {
        //show_exitmenu();
    }
    else
    {
        switch(event->key())
        {
            case Qt::Key_3:
                settletype = 3;
                this->settle_mode();
                break;
            case Qt::Key_W:
                if(settletype == 3)
                {
                    if(Contobesettle->bl->id_y != 0)
                    {
                        Contobesettle->bl = block[Contobesettle->bl->id_x][Contobesettle->bl->id_y - 1];
                        settleable = Contobesettle->settle_available();
                    }
                }
                break;
            case Qt::Key_S:
                if(settletype == 3)
                {
                    if(Contobesettle->bl->id_y != Maxy - 1)
                    {
                        Contobesettle->bl = block[Contobesettle->bl->id_x][Contobesettle->bl->id_y + 1];
                        settleable = Contobesettle->settle_available();
                    }
                }
                break;
            case Qt::Key_A:
                if(settletype == 3)
                {
                    if(Contobesettle->bl->id_x != 0)
                    {
                        Contobesettle->bl = block[Contobesettle->bl->id_x - 1][Contobesettle->bl->id_y];
                        settleable = Contobesettle->settle_available();
                    }
                }
                break;
            case Qt::Key_D:
                if(settletype == 3)
                {
                    if(Contobesettle->bl->id_x != Maxx - 1)
                    {
                        Contobesettle->bl = block[Contobesettle->bl->id_x + 1][Contobesettle->bl->id_y];
                        settleable = Contobesettle->settle_available();
                    }
                }
                break;
            case Qt::Key_Z:
                if(settletype == 3)
                {
                    if(Contobesettle->settle_available())
                    {
                        Contobesettle->settle();
                        settletype = -1;
                        curdir = 0;
                    }

                }
                break;
            case Qt::Key_R:
                if(settletype == 3)
                {
                    Contobesettle->dir = (Contobesettle->dir + 1) %12;
                    curdir = Contobesettle->dir;
                    Contobesettle->resetdir();
                }
                break;
            case Qt::Key_X:
                    if(settletype == 3)
                    {
                        delete Contobesettle;
                        settletype = -1;
                    }
                break;
        }
    }
    this->update();
    return ;
}


void GameScene::draw_block(QPainter &painter, Block *bl)
{
    painter.drawPixmap(bl->p.pos_x, bl->p.pos_y, bl->icon);
    if(bl->mine)
    {
        painter.drawPixmap(bl->p.pos_x, bl->p.pos_y, bl->mine->icon);
    }
    if(bl->facility)
    {
        painter.drawPixmap(bl->p.pos_x, bl->p.pos_y, bl->facility->icon);
        if(bl->facility->type == 0)
        {
            if(((Mine* )(bl->facility))->harvestor)
            {
                painter.drawPixmap(bl->p.pos_x, bl->p.pos_y, ((Mine* )(bl->facility))->harvestor->icon);
            }
        }
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
