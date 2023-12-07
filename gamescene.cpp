#include "gamescene.h"
#include "ui_gamescene.h"
#include <QString>
#include <QPainter>
#include <QDebug>
#include <QCloseEvent>
#include <QTimer>
#include <iostream>
#include <fstream>
using namespace  std;
const int SIZE = 50;

extern bool ifload;
//当前地图最大长宽
extern int Maxx, Maxy;
extern int num_mine[2];

extern QString filePath;
extern int mineral_value[4];
extern int earned;
extern int num_base;

int init_harvestor_speed = 15;
int init_base_speed = 10;
int init_conveyer_speed = 20;
int init_cutter_speed = 2000;

int money = 0;
Block *block[30][20];

QString Facility_name[10]={"error", "harvestor", "base", "conveyer","cutter","dustbin",};

//此时Pos中储存id_x, id_y
unordered_map<Mine *, Block *> mine_all;
unordered_map<Harvestor *, Block *> harvestor_all;
unordered_map<Base *, Block *> base_all;
unordered_map<Conveyer *, Block *> conveyer_all;
unordered_map<Cutter *,Block *>cutter_all;
unordered_map<Dustbin *, Block *> dustbin_all;

map<int, Mineral *> mineral_all;
int mineral_cnt = 0;

int mineral_num[4]={0,0,0,0};
bool bufflag[4]={0,0,0,0};
int harvestor_speed = 30;
int base_speed = 10;
int conveyer_speed = 40;
int cutter_speed = 5000;
int settletype = -1;
Conveyer *Contobesettle = NULL;
Harvestor *Hartobesettle = NULL;
Dustbin *Dustobesettle = NULL;
Cutter *Cuttobesettle = NULL;

bool settleable = false;
int curdir = 0;
int isnew = 0;
Block *shovelblock;
GameScene::GameScene(QWidget *parent, bool isload) :
    QWidget(parent),
    ui(new Ui::GameScene)
{
    ui->setupUi(this);

    //初始化
    setFixedSize(1700,1000);
    setWindowTitle("myShapez");

    if(isload)
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
    timer0->start(1000);

    //Block *test = new Block();
    for(int i = 0; i < Maxy; ++i)
    {
        for(int j = 0; j < Maxx; ++j)
        {
            block[i][j] = new Block(NULL, i, j);
        }
    }

    connect(timer1, &QTimer::timeout, this, [this]{this->create_mineral();});//Harvestor
    timer1->start(harvestor_speed);

    connect(timer2, &QTimer::timeout, this, [this]{this->move_mineral();});//Conveyer
    timer2->start(conveyer_speed);


    connect(timer4, &QTimer::timeout, this, [this]{this->update();});//刷新屏幕

    connect(timer4, &QTimer::timeout, this, [=](){
        ui->MoneyLabel->setText("Money:" + QString::number(money));
    });
    connect(timer4, &QTimer::timeout, this, [this]{this->buff_check();});

    connect(ui->buff0, &QPushButton::clicked, this, [&]{conveyer_speed /= 2;});
    connect(ui->buff0, &QPushButton::clicked, ui->buff0, [this]{ui->buff0->hide(); ui->buff0text->hide();timer2->setInterval(conveyer_speed);});

    connect(ui->buff1, &QPushButton::clicked, this, [&]{harvestor_speed /= 2;});
    connect(ui->buff1, &QPushButton::clicked, ui->buff1, [this]{ui->buff1->hide(); ui->buff1text->hide();timer1->setInterval(harvestor_speed);});

    connect(ui->buff2, &QPushButton::clicked, this, [&]{cutter_speed /= 2;});
    connect(ui->buff2, &QPushButton::clicked, ui->buff2, [this]{ui->buff2->hide(); ui->buff2text->hide();});

    ui->buff0->hide();ui->buff1->hide();ui->buff2->hide();
    ui->buff0text->hide();ui->buff1text->hide();ui->buff2text->hide();

    timer4->start(20);

    ui->exittext->setText("Save&Exit");
    connect(ui->exit, &QPushButton::clicked, this, [this]{this->Exit(isnew);});

    //放置
    connect(ui->Consettletn, &QPushButton::clicked, this, [&, this]{
        if(settletype == -1)
            {
            settletype = 3; this->settle_mode();}
            }
        );
    connect(ui->Havsettletn, &QPushButton::clicked, this, [&, this]{
            if(settletype == -1)
            {
            settletype = 1; this->settle_mode();}
            });
    connect(ui->Dustsettletn, &QPushButton::clicked, this, [&, this]{
        if(settletype == -1)
        {
            settletype = 5; this->settle_mode();}
        });
    connect(ui->Cutsettletn, &QPushButton::clicked, this, [&, this]{
        if(settletype == -1)
        {
            settletype = 4; this->settle_mode();
        }
    });
    connect(ui->shovel, &QPushButton::clicked, this, [&, this]{
        if(settletype == -1)
        {
            settletype = 6; this->settle_mode();
        }
    });
    Mine *tmp = new Mine(parent, block[0][0], 0);
    tmp->settle();
    tmp = new Mine(parent, block[0][1], 0);
    tmp->settle();

    Harvestor *hav = new Harvestor(parent, block[0][0], 1);
    if(hav->settle_available())
    {
        hav->settle();
    }
    qDebug()<<conveyer_all.size();
    hav = new Harvestor(parent, block[0][1], 1);
    if(hav->settle_available())
    {
        hav->settle();
    }
    qDebug()<<conveyer_all.size();
    Base *base = new Dustbin(parent, block[2][0]);
    if(base->settle_available())
    {
        base->settle();
    }
    qDebug()<<conveyer_all.size();
    base = new Base(parent, block[2][2]);
    if(base->settle_available())
    {
        base->settle();
    }
    qDebug()<<conveyer_all.size();
    Cutter *cut = new Cutter(parent, block[2][1], 0, 2);
    if(cut->settle_available())
    {
        cut->Cutter_settle();
    }
qDebug()<<conveyer_all.size();


    Conveyer *conv;
    conv = new Conveyer(parent, block[1][0], 1);
    if(conv->settle_available())
    {
        conv->settle();
    }
    qDebug()<<conveyer_all.size();
    conv = new Conveyer(parent, block[2][0], 7);
    if(conv->settle_available())
    {
        conv->settle();
    }
    else
    {
        delete conv;
    }
    qDebug()<<conveyer_all.size();
    conv = new Conveyer(parent, block[2][4], 3);
    if(conv->settle_available())
    {
        conv->settle();
    }
    qDebug()<<conveyer_all.size();
    conv = new Conveyer(parent, block[1][2], 2);
    if(conv->settle_available())
    {
        conv->settle();
    }
    qDebug()<<conveyer_all.size();
    base = new Base(parent, block[1][5]);
    if(base->settle_available())
    {
        base->settle();
    }
qDebug()<<conveyer_all.size();
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
void GameScene::Save(int flag)
{
    std::ofstream savedata;
    if(filePath.isEmpty())
    {
        filePath = "C:\\Users\\ouyan\\Desktop\\myShapez\\savedata.txt";
    }
    savedata.open(filePath.toStdString());


    //game_info
    savedata<<flag<<endl;
    if(!flag)
    {
    savedata<<Maxx<<" "<<Maxy<<endl;
    for(int i=0;i<4;++i)
    {
        savedata<<mineral_num[i]<<" ";
    }
    savedata<<endl;
    savedata<<num_mine[0]<<" "<<num_mine[1]<<endl;
    savedata<<num_base<<endl;
    savedata<<base_all.size()<<endl;
    for(auto pai:base_all)
    {
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<endl;
    }
    savedata<<mine_all.size()<<endl;
    for(auto pai:mine_all)
    {
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<endl;
    }
    savedata<<harvestor_all.size()<<endl;
    for(auto pai:harvestor_all)
    {
        Harvestor *hav = pai.first;
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<" "<<hav->dir<<endl;
    }
    savedata<<conveyer_all.size()<<endl;
    for(auto pai:conveyer_all)
    {
        Conveyer *con = pai.first;
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<" "<<con->dir<<endl;
    }
    for(auto pai:cutter_all)
    {
        Cutter *cut = pai.first;
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<" "<<cut->dir<<endl;
    }
    for(auto pai:dustbin_all)
    {
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<endl;
    }
    for(int i=0;i<4;++i)
    {
        savedata<<mineral_num[i]<<" ";
    }
    savedata<<endl;
    for(int i=0;i<4;++i)
    {
        if(bufflag[i])
        savedata<<"1 ";
        else savedata<<"2 ";
    }
    savedata<<endl;
    savedata<<harvestor_speed<<endl;
    savedata<<conveyer_speed<<endl;
    savedata<<cutter_speed<<endl;
    savedata<<money<<endl;
    }
    else
    {
    savedata<<Maxx<<" "<<Maxy<<endl;
    for(int i=0;i<4;++i)
    {
        savedata<<mineral_num[i]<<" ";
    }
    savedata<<endl;
    savedata<<num_mine[0]<<" "<<num_mine[1]<<endl;
    savedata<<num_base<<endl;
    savedata<<earned + money<<endl;
    }

    savedata.close();
}
void GameScene::Exit(int flag)
{
    Save(flag);
    emit close();
    this->show();
    delete this;

}
void GameScene::buff_check()
{
    ui->labmineral0->setText(QString::number(mineral_num[0]));
    ui->labmineral1->setText(QString::number(mineral_num[1]));
    ui->labmineral2->setText(QString::number(mineral_num[2]));
    ui->labmineral3->setText(QString::number(mineral_num[3]));
    if(mineral_num[0] >= 20 && !bufflag[0])
    {
        ui->buff0->show();
        ui->buff0text->show();
        ui->buff0text->setText("传送带加速");
        bufflag[0] = 1;
    }
    if(mineral_num[1] >= 30 && !bufflag[1])
    {
        ui->buff1->show();
        ui->buff1text->show();
        ui->buff1text->setText("采矿器加速");
        bufflag[1] = 1;
    }
    if(mineral_num[2] >= 50 && !bufflag[2])
    {
        ui->buff2->show();
        ui->buff2text->show();
        ui->buff2text->setText("切割器加速");
        bufflag[2] = 1;
    }
    if(mineral_num[0] >= 50 && mineral_num[1] >= 50 && mineral_num[2] >= 50)
    {
        timer0->stop();
        timer1->stop();
        timer2->stop();
        timer3->stop();
        timer4->stop();
        ui->exittext->setText("Congratulations!");
    }
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
    if(settletype == 6)
    {
        shovelblock = block[0][0];
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
            painter.drawPixmap(1600, 900, Contobesettle->icon);
        }
        if(settletype == 1)
        {
            pos = Hartobesettle->bl;
            painter.drawPixmap(pos->p.pos_x, pos->p.pos_y, icon);
            painter.drawPixmap(1600, 900, Hartobesettle->icon);
        }
        if(settletype == 5)
        {
            pos = Dustobesettle->bl;
            painter.drawPixmap(pos->p.pos_x, pos->p.pos_y, icon);
            painter.drawPixmap(1600, 900, Dustobesettle->icon);
        }
        if(settletype == 4)
        {
            pos = Cuttobesettle->bl;
            painter.drawPixmap(pos->p.pos_x, pos->p.pos_y, icon);
            pos = Cuttobesettle->another->bl;
            painter.drawPixmap(pos->p.pos_x, pos->p.pos_y, icon);
            painter.drawPixmap(1600, 900, Cuttobesettle->icon);
        }
        if(settletype == 6)
        {
            icon.load(":/res/hoe");
            painter.drawPixmap(shovelblock->p.pos_x, shovelblock->p.pos_y, icon);
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
                if(settletype == 1)
                {
                    if(Hartobesettle->bl->id_y != 0)
                    {
                        Hartobesettle->bl = block[Hartobesettle->bl->id_x][Hartobesettle->bl->id_y - 1];
                        settleable = Hartobesettle->settle_available();
                    }
                }
                if(settletype == 5)
                {
                    if(Dustobesettle->bl->id_y != 0)
                    {
                        Dustobesettle->bl = block[Dustobesettle->bl->id_x][Dustobesettle->bl->id_y - 1];
                        settleable = Dustobesettle->settle_available();
                    }
                }
                if(settletype == 4)
                {
                    if(Cuttobesettle->bl->id_y != 0 && Cuttobesettle->another->bl->id_y != 0)
                    {

                        Cuttobesettle->bl = block[Cuttobesettle->bl->id_x][Cuttobesettle->bl->id_y - 1];
                        Cuttobesettle->another->bl = block[Cuttobesettle->another->bl->id_x][Cuttobesettle->another->bl->id_y - 1];
                        settleable = Cuttobesettle->settle_available();
                    }
                }
                if(settletype == 6)
                {
                    if(shovelblock->id_y != 0)
                    {
                        shovelblock = block[shovelblock->id_x][shovelblock->id_y - 1];
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
                if(settletype == 1)
                {
                    if(Hartobesettle->bl->id_y != Maxy - 1)
                    {
                        Hartobesettle->bl = block[Hartobesettle->bl->id_x][Hartobesettle->bl->id_y + 1];
                        settleable = Hartobesettle->settle_available();
                    }
                }
                if(settletype == 5)
                {
                    if(Dustobesettle->bl->id_y != Maxy - 1)
                    {
                        Dustobesettle->bl = block[Dustobesettle->bl->id_x][Dustobesettle->bl->id_y + 1];
                        settleable = Dustobesettle->settle_available();
                    }
                }
                if(settletype == 4)
                {
                    if(Cuttobesettle->bl->id_y != Maxy - 1 && Cuttobesettle->another->bl->id_y != Maxy - 1)
                    {

                        Cuttobesettle->bl = block[Cuttobesettle->bl->id_x][Cuttobesettle->bl->id_y + 1];
                        Cuttobesettle->another->bl = block[Cuttobesettle->another->bl->id_x][Cuttobesettle->another->bl->id_y + 1];
                        settleable = Cuttobesettle->settle_available();
                    }
                }
                if(settletype == 6)
                {
                    if(shovelblock->id_y != Maxy - 1)
                    {
                        shovelblock = block[shovelblock->id_x][shovelblock->id_y + 1];
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
                if(settletype == 1)
                {
                    if(Hartobesettle->bl->id_x != 0)
                    {
                        Hartobesettle->bl = block[Hartobesettle->bl->id_x - 1][Hartobesettle->bl->id_y];
                        settleable = Hartobesettle->settle_available();
                    }
                }
                if(settletype == 5)
                {
                    if(Dustobesettle->bl->id_x != 0)
                    {
                        Dustobesettle->bl = block[Dustobesettle->bl->id_x - 1][Dustobesettle->bl->id_y];
                        settleable = Dustobesettle->settle_available();
                    }
                }
                if(settletype == 4)
                {
                    if(Cuttobesettle->bl->id_x != 0 && Cuttobesettle->another->bl->id_x != 0)
                    {

                        Cuttobesettle->bl = block[Cuttobesettle->bl->id_x - 1][Cuttobesettle->bl->id_y];
                        Cuttobesettle->another->bl = block[Cuttobesettle->another->bl->id_x - 1][Cuttobesettle->another->bl->id_y];
                        settleable = Cuttobesettle->settle_available();
                    }
                }
                if(settletype == 6)
                {
                    if(shovelblock->id_x != 0)
                    {
                        shovelblock = block[shovelblock->id_x - 1][shovelblock->id_y];
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
                if(settletype == 1)
                {
                    if(Hartobesettle->bl->id_x != Maxx - 1)
                    {
                        Hartobesettle->bl = block[Hartobesettle->bl->id_x + 1][Hartobesettle->bl->id_y];
                        settleable = Hartobesettle->settle_available();
                    }
                }
                if(settletype == 5)
                {
                    if(Dustobesettle->bl->id_x != Maxx - 1)
                    {
                        Dustobesettle->bl = block[Dustobesettle->bl->id_x + 1][Dustobesettle->bl->id_y];
                        settleable = Dustobesettle->settle_available();
                    }
                }
                if(settletype == 4)
                {
                    if(Cuttobesettle->bl->id_x != Maxx - 1 && Cuttobesettle->another->bl->id_x != Maxx - 1)
                    {

                        Cuttobesettle->bl = block[Cuttobesettle->bl->id_x + 1][Cuttobesettle->bl->id_y];
                        Cuttobesettle->another->bl = block[Cuttobesettle->another->bl->id_x + 1][Cuttobesettle->another->bl->id_y];
                        settleable = Cuttobesettle->settle_available();
                    }
                }
                if(settletype == 6)
                {
                    if(shovelblock->id_x != Maxx - 1)
                    {
                        shovelblock = block[shovelblock->id_x + 1][shovelblock->id_y];
                    }
                }
                break;
            case Qt::Key_Z:
                //if(!settleable) return;
                if(settletype == 3)
                {
                    if(Contobesettle->settle_available())
                    {
                        Contobesettle->settle();
                        settletype = -1;
                        curdir = 0;
                    }

                }
                if(settletype == 1)
                {
                    if(Hartobesettle->settle_available())
                    {
                        Hartobesettle->settle();
                        settletype = -1;
                        curdir = 0;
                    }

                }
                if(settletype == 5)
                {
                    if(Dustobesettle->settle_available())
                    {
                        Dustobesettle->settle();
                        settletype = -1;
                        curdir = 0;
                    }

                }
                if(settletype == 4)
                {
                    if(Cuttobesettle->settle_available())
                    {
                        Cuttobesettle->Cutter_settle();
                        settletype = -1;
                        curdir = 0;
                    }

                }
                if(settletype == 6)
                {
                    shovelblock->clear();
                    shovelblock = NULL;
                    settletype = -1;
                    curdir = 0;
                }
                break;
            case Qt::Key_R:
                if(settletype == 3)
                {
                    Contobesettle->dir = (Contobesettle->dir + 1) % 12;
                    curdir = Contobesettle->dir;
                    Contobesettle->resetdir();
                }
                if(settletype == 1)
                {
                    Hartobesettle->dir = (Hartobesettle->dir + 1) % 4;
                    curdir = Hartobesettle->dir;
                    Hartobesettle->resetdir();
                }
                if(settletype == 4)
                {
                    Cuttobesettle->resetdir();
                }
                break;
            case Qt::Key_X:
                    if(settletype == 3)
                    {
                        delete Contobesettle;
                        settletype = -1;
                    }
                    if(settletype == 1)
                    {
                        delete Hartobesettle;
                        settletype = -1;
                    }
                    if(settletype == 5)
                    {
                        delete Dustobesettle;
                        settletype = -1;
                    }
                    if(settletype == 4)
                    {
                        delete Cuttobesettle->another;
                        delete Cuttobesettle;
                        settletype = -1;

                    }
                    if(settletype == 6)
                    {
                        shovelblock = NULL;
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
