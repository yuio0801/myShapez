#include "gamescene.h"
#include "ui_gamescene.h"
#include <QString>
#include <QPainter>
#include <QDebug>
#include <QCloseEvent>
#include <QTimer>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace  std;
const int SIZE = 50;

extern bool ifload;
//当前地图最大长宽
extern int Maxx, Maxy;
extern int num_mine[2];

extern QString filePath;
extern int mineral_value[5];
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
unordered_map<Rotater *, Block *> rotater_all;
map<int, Mineral *> mineral_all;
int mineral_cnt = 0;

int mineral_num[5]={0,0,0,0,0};
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
Rotater *Rottobesettle = NULL;

bool settleable = false;
int curdir = 0;
int isnew = 1;
Block *shovelblock;
GameScene::GameScene(QWidget *parent, bool isload) :
    QWidget(parent),
    ui(new Ui::GameScene)
{
    ui->setupUi(this);

    //初始化
    setFixedSize(1700,1000);
    setWindowTitle("myShapez");

    if(!isnew)
    {
        Load();
        Init();
    }
    else
    {
        Init();
    }
    isnew=0;
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
    connect(ui->Rotsettletn, &QPushButton::clicked, this, [&, this]{
        if(settletype == -1)
        {
            settletype = 7; this->settle_mode();}
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
}

GameScene::~GameScene()
{
    delete ui;
}
bool GameScene::Load()
{
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

void GameScene::Init()
{
    mine_all.clear();
    harvestor_all.clear();
    base_all.clear();
    conveyer_all.clear();
    cutter_all.clear();
    dustbin_all.clear();
    rotater_all.clear();
    mineral_all.clear();
    //Block *test = new Block();
    for(int i = 0; i < Maxy; ++i)
    {
        for(int j = 0; j < Maxx; ++j)
        {
            block[i][j] = new Block(NULL, i, j);
        }
    }
    harvestor_speed = init_harvestor_speed;
    base_speed = init_base_speed;
    conveyer_speed = init_conveyer_speed;
    cutter_speed = init_cutter_speed;
    settletype = -1;
    if(!ifload || isnew)
    {
        money = 0;
        mineral_cnt = 0;
        for(int i=0;i<5;++i)
        {
            mineral_num[i]=0;
        }
        for(int i=0;i<4;++i)
        {
            bufflag[i]=0;
        }
        Contobesettle = NULL;
        Hartobesettle = NULL;
        Dustobesettle = NULL;
        Cuttobesettle = NULL;
        Rottobesettle = NULL;
        settleable = false;
        curdir = 0;
        shovelblock=NULL;
        /*TODO:BASE MINE*/
        for(int i=Maxx/2-2;i<Maxx/2+num_base - 2;++i)
        {
            for(int j=Maxy/2-2;j<Maxy/2+num_base - 2;++j)
            {
                Base *base = new Base(NULL, block[i][j]);
                base->settle();
            }
        }
        srand(time(NULL));
        int dx[4]={1,-1,0,0};
        int dy[4]={0,0,1,-1};
        int c=0;
        while(c<=num_mine[0])
        {
            int idx=rand()%Maxx;
            int idy=rand()%Maxy;
            bool flag=true;
            for(int i=0;i<4;++i)
            {
                int xx=idx+dx[i];
                int yy=idy+dy[i];
                if(xx<0||xx>=Maxx||yy<0||yy>=Maxy)
                {
                    continue;
                }
                if(block[xx][yy]->mine)
                {
                    flag=false;
                    break;
                }
                if(block[xx][yy]->facility&&block[xx][yy]->facility->type == 2)
                {
                    flag=false;
                    break;
                }
            }
            if(!flag ) continue;
            Mine *mine = new Mine(NULL,block[idx][idy],0);
            if(mine->settle_available())
            {
                mine->settle();
                c++;
            }
            else
            {
                delete mine;
            }
        }
        c=0;
        while(c<=num_mine[1])
        {
            int idx=rand()%Maxx;
            int idy=rand()%Maxy;
            bool flag=true;
            for(int i=0;i<4;++i)
            {
                int xx=idx+dx[i];
                int yy=idy+dy[i];
                if(xx<0||xx>=Maxx||yy<0||yy>=Maxy)
                {
                        continue;
                }
                if(block[xx][yy]->mine)
                {
                    flag=false;
                    break;
                }
                if(block[xx][yy]->facility&&block[xx][yy]->facility->type == 2)
                {
                    flag=false;
                    break;
                }
            }
            if(!flag ) continue;
            Mine *mine = new Mine(NULL,block[idx][idy],1);
            if(mine->settle_available())
            {
                mine->settle();
                c++;
            }
            else
            {
                delete mine;
            }
        }
    }
    else
    {
        ifstream savedata;
        string strFileData = "";
        int line = 1;int type=0;
        char tmpLineData[1024] = {0};
        if(filePath.isEmpty())
        {
            filePath = "C:\\Users\\ouyan\\Desktop\\myShapez\\savedata.txt";
        }
        savedata.open(filePath.toStdString());
        savedata.getline(tmpLineData, sizeof(tmpLineData));
        sscanf(tmpLineData, "%d", &isnew);
        while(savedata.getline(tmpLineData, sizeof(tmpLineData)))
        {
            if(line<6)
            {
                line++;
                continue;
            }
            if(type==0)
            {
                int basenum;
                sscanf(tmpLineData, "%d", &basenum);
                for(int i=0;i<basenum;++i)
                {
                    savedata.getline(tmpLineData, sizeof(tmpLineData));
                    int idx,idy;
                    sscanf(tmpLineData,"%d %d",&idx,&idy);
                    Base *base = new Base(NULL, block[idx][idy]);
                    base->settle();
                }
                type++;
                continue;
            }
            else if(type == 1)
            {
                int minenum;
                sscanf(tmpLineData, "%d", &minenum);
                for(int i=0;i<minenum;++i)
                {
                    savedata.getline(tmpLineData, sizeof(tmpLineData));
                    int idx,idy,t;
                    sscanf(tmpLineData,"%d %d %d",&idx,&idy,&t);
                    Mine *mine = new Mine(NULL, block[idx][idy],t);
                    mine->settle();
                }
                type++;
                continue;
            }
            else if(type == 2)
            {
                int havnum;
                sscanf(tmpLineData, "%d", &havnum);
                for(int i=0;i<havnum;++i)
                {
                    savedata.getline(tmpLineData, sizeof(tmpLineData));
                    int idx,idy,t;
                    sscanf(tmpLineData,"%d %d %d",&idx,&idy,&t);
                    Harvestor *hav = new Harvestor(NULL, block[idx][idy],t);
                    hav->settle();
                }
                type++;
                continue;
            }
            else if(type == 3)
            {
                int connum;
                sscanf(tmpLineData, "%d", &connum);
                for(int i=0;i<connum;++i)
                {
                    savedata.getline(tmpLineData, sizeof(tmpLineData));
                    int idx,idy,t;
                    sscanf(tmpLineData,"%d %d %d",&idx,&idy,&t);
                    Conveyer *con = new Conveyer(NULL, block[idx][idy],t);
                    con->settle();
                }
                type++;
                continue;
            }
            else if(type == 4)
            {
                int cutnum;
                sscanf(tmpLineData, "%d", &cutnum);
                for(int i=0;i<cutnum;++i)
                {
                    savedata.getline(tmpLineData, sizeof(tmpLineData));
                    int idx,idy,t;
                    sscanf(tmpLineData,"%d %d %d",&idx,&idy,&t);
                    Cutter *cut = new Cutter(NULL, block[idx][idy],0,t,NULL);
                    cut->Cutter_settle();
                }
                type++;
                continue;
            }
            else if(type == 5)
            {
                int dustnum;
                sscanf(tmpLineData, "%d", &dustnum);
                for(int i=0;i<dustnum;++i)
                {
                    savedata.getline(tmpLineData, sizeof(tmpLineData));
                    int idx,idy;
                    sscanf(tmpLineData,"%d %d",&idx,&idy);
                    Dustbin *dust = new Dustbin(NULL, block[idx][idy]);
                    dust->settle();
                }
                type++;
                continue;
            }
            else if(type == 6)
            {
                int rotnum;
                sscanf(tmpLineData, "%d", &rotnum);
                for(int i=0;i<rotnum;++i)
                {
                    savedata.getline(tmpLineData, sizeof(tmpLineData));
                    int idx,idy,t;
                    sscanf(tmpLineData,"%d %d %d",&idx,&idy,&t);
                    Rotater *rot = new Rotater(NULL, block[idx][idy],t);
                    rot->settle();
                }
                type++;
                continue;
            }
            else if(type == 7)
            {
                sscanf(tmpLineData,"%d %d %d %d %d",&mineral_num[0],&mineral_num[1],&mineral_num[2],&mineral_num[3],&mineral_num[4]);
                type++;
                continue;
            }
            else if(type == 8)
            {
                int d[4];
                sscanf(tmpLineData,"%d %d %d %d",&d[0],&d[1],&d[2],&d[3]);
                for(int i=0;i<4;++i)
                {
                    if(d[i]) bufflag[i]=1;
                    else bufflag[i]=0;
                }
                type++;
                continue;
            }
            else if(type == 9)
            {
                sscanf(tmpLineData,"%d",&harvestor_speed);
                type++;
                continue;
            }
            else if(type == 10)
            {
                sscanf(tmpLineData,"%d",&conveyer_speed);
                type++;
                continue;
            }
            else if(type == 11)
            {
                sscanf(tmpLineData,"%d",&cutter_speed);
                type++;
                continue;
            }
            else if(type == 12)
            {
                sscanf(tmpLineData,"%d",&money);
                type++;
                continue;
            }
            line++;
        }
    }
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
    if(!flag)
    {
        savedata<<"0"<<endl;
    savedata<<Maxx<<" "<<Maxy<<endl;
    for(int i=0;i<5;++i)
    {
        savedata<<mineral_value[i]<<" ";
    }
    savedata<<endl;
    savedata<<num_mine[0]<<" "<<num_mine[1]<<endl;
    savedata<<num_base<<endl;
    savedata<<earned<<endl;

    savedata<<base_all.size()<<endl;
    for(auto pai:base_all)
    {
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<endl;
    }
    savedata<<mine_all.size()<<endl;
    for(auto pai:mine_all)
    {
        Mine *mine=pai.first;
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<" "<<mine->id_mineral<<endl;
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
    savedata<<cutter_all.size()<<endl;
    for(auto pai:cutter_all)
    {
        Cutter *cut = pai.first;
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<" "<<cut->dir<<endl;
    }
    savedata<<dustbin_all.size()<<endl;
    for(auto pai:dustbin_all)
    {
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<endl;
    }
    savedata<<rotater_all.size()<<endl;
    for(auto pai:rotater_all)
    {
        Rotater *rot = pai.first;
        Block *bl = pai.second;
        savedata<<bl->id_x<<" "<<bl->id_y<<" "<<rot->dir<<endl;
    }
    for(int i=0;i<5;++i)
    {
        savedata<<mineral_num[i]<<" ";
    }
    savedata<<endl;
    for(int i=0;i<4;++i)
    {
        if(bufflag[i])
        savedata<<"1 ";
        else savedata<<"0 ";
    }
    savedata<<endl;
    savedata<<harvestor_speed<<endl;
    savedata<<conveyer_speed<<endl;
    savedata<<cutter_speed<<endl;
    savedata<<money<<endl;
    }
    else
    {
    savedata<<"1"<<endl;
    savedata<<Maxx<<" "<<Maxy<<endl;
    for(int i=0;i<5;++i)
    {
        savedata<<mineral_value[i]<<" ";
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
    Save(isnew&&flag);
    emit close();
    this->show();
    delete this;

}
void GameScene::buff_check()
{
    ui->labmineral0->setText(QString::number(mineral_num[0]) + "/20");
    ui->labmineral1->setText(QString::number(mineral_num[1]) + "/30");
    ui->labmineral2->setText(QString::number(mineral_num[2]) + "/50");
    ui->labmineral3->setText(QString::number(mineral_num[3]) + "/50");
    ui->labmineral4->setText(QString::number(mineral_num[4]) + "/30");
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
    if(mineral_num[0] >= 20 && mineral_num[1] >= 30 && mineral_num[2] >= 50 && mineral_num[3] >= 50 && mineral_num[4] >= 50)
    {
        timer0->stop();
        timer1->stop();
        timer2->stop();
        timer3->stop();
        timer4->stop();
        ui->exittext->setText("Congratulations!");
        isnew = 1;
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
    for(auto pai:rotater_all)
    {
        Rotater *rot = pai.first;
        Block *bl = pai.second;
        assert(bl);
        rot->Mineral_move();
    }
    for(auto pai:cutter_all)
    {
        Cutter *cut = pai.first;
        Block *bl = pai.second;
        assert(bl);
        cut->ifout();
    }
}
void GameScene::settle_mode()
{
    if(settletype == 3)
    {
        Contobesettle = new Conveyer(NULL, block[0][0], 0);
        settleable = Contobesettle->settle_available();
    }
    if(settletype == 7)
    {
        Rottobesettle = new Rotater(NULL, block[0][0], 0);
        settleable = Rottobesettle->settle_available();
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
        if(settletype == 7)
        {
            pos = Rottobesettle->bl;
            painter.drawPixmap(pos->p.pos_x, pos->p.pos_y, icon);
            painter.drawPixmap(1600, 900, Rottobesettle->icon);
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
                if(settletype == 7)
                {
                    if(Rottobesettle->bl->id_y != 0)
                    {
                        Rottobesettle->bl = block[Rottobesettle->bl->id_x][Rottobesettle->bl->id_y - 1];
                        settleable = Rottobesettle->settle_available();
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
                if(settletype == 7)
                {
                    if(Rottobesettle->bl->id_y != Maxy - 1)
                    {
                        Rottobesettle->bl = block[Rottobesettle->bl->id_x][Rottobesettle->bl->id_y + 1];
                        settleable = Rottobesettle->settle_available();
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
                if(settletype == 7)
                {
                    if(Rottobesettle->bl->id_x != 0)
                    {
                        Rottobesettle->bl = block[Rottobesettle->bl->id_x - 1][Rottobesettle->bl->id_y];
                        settleable = Rottobesettle->settle_available();
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
                if(settletype == 7)
                {
                    if(Rottobesettle->bl->id_x != Maxx - 1)
                    {
                        Rottobesettle->bl = block[Rottobesettle->bl->id_x + 1][Rottobesettle->bl->id_y];
                        settleable = Rottobesettle->settle_available();
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
                if(settletype == 7)
                {
                    if(Rottobesettle->settle_available())
                    {
                        Rottobesettle->settle();
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
                if(settletype == 7)
                {
                    Rottobesettle->dir = (Rottobesettle->dir + 1) % 12;
                    curdir = Rottobesettle->dir;
                    Rottobesettle->resetdir();
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
                if(settletype == -1)
                {
                    break;
                }
                    if(settletype == 3)
                    {
                        delete Contobesettle;
                        settletype = -1;
                    }
                    if(settletype == 7)
                    {
                        delete Rottobesettle;
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
