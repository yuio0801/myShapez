#include "gamescene.h"
#include "ui_gamescene.h"
#include <QString>
#include <QPainter>
#include <QDebug>
#include <QCloseEvent>
#include <QTimer>

const int SIZE = 50;

int init_harvestor_speed = 15;
int init_base_speed = 10;
int init_conveyer_speed = 20;
int init_cutter_speed = 1000;
int init_Task = 1;
int money = 0;
Block *block[30][20];

//当前地图最大长宽
int Maxx = 10, Maxy = 10;
QString Facility_name[10]={"error", "harvestor", "base", "conveyer","cutter","dustbin",};
extern QString filePath;
int mineral_value[4]={20, 10, 50, 30};
//此时Pos中储存id_x, id_y
unordered_map<Mine *, Block *> mine_all;
unordered_map<Harvestor *, Block *> harvestor_all;
unordered_map<Base *, Block *> base_all;
unordered_map<Conveyer *, Block *> conveyer_all;
unordered_map<Cutter *,Block *>cutter_all;
unordered_map<Dustbin *, Block *> dustbin_all;

map<int, Mineral *> mineral_all;
int mineral_cnt = 0;


int mineral_num[4];

int harvestor_speed = 15;
int base_speed = 10;
int conveyer_speed = 20;
int cutter_speed = 5000;

//int Harvestor::speed = harvestor_speed;
//int Base::speed = 0xffff;
//int Conveyer::speed = 20;

int settletype = -1;
Conveyer *Contobesettle = NULL;
Harvestor *Hartobesettle = NULL;
Dustbin *Dustobesettle = NULL;
Cutter *Cuttobesettle = NULL;

bool settleable = false;
int curdir = 0;

GameScene::GameScene(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameScene)
{
    ui->setupUi(this);

    //初始化
    setFixedSize(1700,1000);
    setWindowTitle("myShapez");

    if(!filePath.isNull())
    {
        if(!Load())
        {
            Init();
        }
    }
    else
    {
        Init();
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
    connect(ui->Havsettletn, &QPushButton::clicked, this, [&, this]{settletype = 1; this->settle_mode();});
    connect(ui->Dustsettletn, &QPushButton::clicked, this, [&, this]{settletype = 5; this->settle_mode();});
    connect(ui->Cutsettletn, &QPushButton::clicked, this, [&, this]{settletype = 4; this->settle_mode();});

    Mine *tmp = new Mine(parent, block[0][0], 0);
    tmp->settle();
    tmp = new Mine(parent, block[0][1], 0);
    tmp->settle();

    Harvestor *hav = new Harvestor(parent, block[0][0], 1);
    if(hav->settle_available())
    {
        hav->settle();
    }
    hav = new Harvestor(parent, block[0][1], 1);
    if(hav->settle_available())
    {
        hav->settle();
    }
    Base *base = new Dustbin(parent, block[2][0]);
    if(base->settle_available())
    {
        base->settle();
    }
    base = new Base(parent, block[2][2]);
    if(base->settle_available())
    {
        base->settle();
    }
    Cutter *cut = new Cutter(parent, block[2][1], 0, 2);
    if(cut->settle_available())
    {
        cut->Cutter_settle();
    }



    Conveyer *conv;
    conv = new Conveyer(parent, block[1][0], 1);
    if(conv->settle_available())
    {
        conv->settle();
    }
    conv = new Conveyer(parent, block[2][0], 7);
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
    conv = new Conveyer(parent, block[1][2], 2);
    if(conv->settle_available())
    {
        conv->settle();
    }
    base = new Base(parent, block[1][5]);
    if(base->settle_available())
    {
        base->settle();
    }

    //delete block[0][0];
    //block[0][0]=NULL;
    //qDebug()<<"delete asda";
}

GameScene::~GameScene()
{
    delete ui;
}
bool GameScene::Load()
{
    return 0;
}
void GameScene::Init()
{
    mine_all.clear();
    harvestor_all.clear();
    base_all.clear();
    conveyer_all.clear();

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
        settleable = Contobesettle->settle_available();
    }
    if(settletype == 1)
    {
        Hartobesettle = new Harvestor(NULL, block[0][0], 0);
        settleable = Hartobesettle->settle_available();
    }
    if(settletype == 5)
    {
        Dustobesettle = new Dustbin(NULL, block[0][0]);
        settleable = Dustobesettle->settle_available();
    }
    if(settletype == 4)
    {
        Cuttobesettle = new Cutter(NULL, block[0][0], 0, 0, NULL);
        settleable = Cuttobesettle->settle_available();
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
            painter.drawPixmap(1650, 950, Contobesettle->icon);
        }
        if(settletype == 1)
        {
            pos = Hartobesettle->bl;
            painter.drawPixmap(pos->p.pos_x, pos->p.pos_y, icon);
            painter.drawPixmap(1650, 950, Hartobesettle->icon);
        }
        if(settletype == 5)
        {
            pos = Dustobesettle->bl;
            painter.drawPixmap(pos->p.pos_x, pos->p.pos_y, icon);
            painter.drawPixmap(1650, 950, Dustobesettle->icon);
        }
        if(settletype == 4)
        {
            pos = Cuttobesettle->bl;
            painter.drawPixmap(pos->p.pos_x, pos->p.pos_y, icon);
            painter.drawPixmap(1650, 950, Cuttobesettle->icon);
            pos = Cuttobesettle->another->bl;
            painter.drawPixmap(pos->p.pos_x, pos->p.pos_y, icon);
            painter.drawPixmap(1650, 950, Cuttobesettle->another->icon);
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
}
