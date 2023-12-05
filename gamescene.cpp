#include "gamescene.h"
#include "ui_gamescene.h"
#include <QString>
#include <QPainter>
#include <QDebug>
#include <QCloseEvent>
#include <QTimer>

const int SIZE = 50;

int Task = 1;
//当前地图最大长宽
int Maxx = 10, Maxy = 10;
QString Facility_name[10]={"error", "harvestor", "base", "conveyer",};
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

int harvestor_speed = 2;
int base_speed = 0xffff;
int conveyer_speed = 4;

int Harvestor::speed = harvestor_speed;
int Base::speed = 0xffff;
int Conveyer::speed = 20;

GameScene::GameScene(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameScene)
{
    ui->setupUi(this);

    //初始化
    setFixedSize(1700,1000);
    setWindowTitle("Task "+QString::number(Task));
    GameTime = 0;
    ui->TimeLabel->setText("GameTime:0s");
    timer0 = new QTimer;
    timer1 = new QTimer;
    timer2 = new QTimer;
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

    connect(timer1, &QTimer::timeout, this, [this]{this->move_all_mineral();});//移动所有矿石
    connect(timer1, &QTimer::timeout, this, [this]{this->update();});//刷新屏幕
    timer1->start(100);



    Mine *tmp = new Mine(parent, block[0][0], 0);
    tmp->settle();
    Harvestor *hav = new Harvestor(parent, block[0][0], 1);
    if(hav->settle_available())
    {
        hav->settle();
    }
    Base *base = new Base(parent, block[2][4]);
    if(base->settle_available())
    {
        base->settle();
    }
    Conveyer *conv = new Conveyer(parent, block[1][0], 2);
    if(conv->settle_available())
    {
        conv->settle();
    }
    conv = new Conveyer(parent, block[2][0], 3);
    if(conv->settle_available())
    {
        conv->settle();
    }
    conv = new Conveyer(parent, block[2][1], 4);
    if(conv->settle_available())
    {
        conv->settle();
    }
    conv = new Conveyer(parent, block[2][2], 4);
    if(conv->settle_available())
    {
        conv->settle();
    }
    conv = new Conveyer(parent, block[2][3], 4);
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


bool GameScene::Mineral_out(Block *bl, Facility *fac, Mineral *tmp)
{
    int dir = fac->out_dir;
    if(tmp->p.pos_x == 0 || tmp->p.pos_x == Maxx * SIZE || tmp->p.pos_y == 0 || tmp->p.pos_y == Maxy * SIZE)
    {
        delete tmp;
        fac->mineral_outque = NULL;
        for(auto pai:mineral_all)
        {
            if(pai.second == tmp)
            {
                mineral_all.erase(pai.first);
                break;
            }
        }
        return 0;
    }
    Block* nxt;Facility *nfac; bool flag = false;
    if(dir == 0 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y - SIZE / 2)
    {
        nxt = block[bl->id_x][bl->id_y - 1];
        if(!nxt->facility)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->in_dir == -1 || nfac->in_dir == 0)
            {
                flag = true;
            }
        }
        //if(/*TODO:CUTTER*/)
        }
    else if(dir == 1 && tmp->p.pos_x == bl->middle.pos_x + SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y)
    {
        nxt = block[bl->id_x + 1][bl->id_y];
        if(!nxt->facility)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->in_dir == -1 || nfac->in_dir == 1)
            {
                flag = true;
            }
        }
    }
    else if(dir == 2 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y + SIZE / 2)
    {
        nxt = block[bl->id_x][bl->id_y + 1];
        if(!nxt->facility)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->in_dir == -1 || nfac->in_dir == 2)
            {
                flag = true;
            }
        }
    }
    else if(dir == 3 && tmp->p.pos_x == bl->middle.pos_x - SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y)
    {
        nxt = block[bl->id_x - 1][bl->id_y];
        if(!nxt->facility)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->in_dir == -1 || nfac->in_dir == 3)
            {
                flag = true;
            }
        }
    }
    assert(tmp->p.pos_x % 25 == 0 && tmp->p.pos_y % 25 == 0);
    //qDebug()<<"out of situation error";
    return flag;
}

void GameScene::move_all_mineral()
{
    vector<Mineral* > v;
    for(auto pai:mineral_all)
    {
        int idx = pai.first;
        Mineral* tmp = pai.second;
        Block* bl = tmp->bl;
        Facility* fac = tmp->bl->facility;
        if(!fac)
        {
            qDebug()<<"locating mineral error1";
            return ;
        }
        int distance;
        switch(fac->type)
        {
        case 1: distance = harvestor_speed; break;
        case 2: distance = base_speed; break;
        case 3: distance = conveyer_speed; break;
        default: distance = 0xffff; break;
        }
        int dx[4] = {0, 1, 0, -1};
        int dy[4] = {-1, 0, 1, 0};
        int dir;

        int last = 0, lastspeed = distance;

        if(fac->mineral_outque == tmp)
        {
            dir = fac->out_dir;

            if((dir == 0 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y - SIZE / 2) ||
                (dir == 1 && tmp->p.pos_x == bl->middle.pos_x + SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y) ||
                (dir == 2 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y + SIZE / 2) ||
                (dir == 3 && tmp->p.pos_x == bl->middle.pos_x - SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y))
            {
                if(Mineral_out(bl, fac, tmp))
                {
                    if(!tmp->bl){
                        qDebug()<<"mineral_out error3";
                    }

                    Block* nxt; Facility *nfac;
                    switch(dir)
                    {
                    case 0:nxt = block[bl->id_x][bl->id_y - 1];break;
                    case 1:nxt = block[bl->id_x + 1][bl->id_y];break;
                    case 2:nxt = block[bl->id_x][bl->id_y + 1];break;
                    case 3:nxt = block[bl->id_x - 1][bl->id_y];break;

                    }
                    //qDebug()<<"out of situation error";
                    nfac = nxt->facility;
                    assert(nfac != NULL);
                    if(nfac->type ==2)
                    {
                        money += mineral_value[tmp->type];
                        v.push_back(tmp);
                        break;
                    }
                    tmp->bl =nxt;
                    fac->mineral_outque = NULL;
                    nfac->mineral_inque = tmp;

                    bl = tmp->bl;
                    fac = tmp->bl->facility;
                    assert(fac != NULL);
                    assert(fac->type != 0);

                    int nxtspeed;
                    switch(fac->type)
                    {
                    case 1: nxtspeed = harvestor_speed; break;
                    case 2: nxtspeed = base_speed; break;
                    case 3: nxtspeed = conveyer_speed; break;
                    default: nxtspeed = 0xffff; break;
                    }
                    distance = nxtspeed;
                    lastspeed = nxtspeed;
                    last = 0;
                }
                else
                {
                    break;
                }
            }
        }



        while(distance>=0)
        {

            if(fac->mineral_inque == tmp)
            {
                if(tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y)
                {
                    if(fac->Mineral_tackle())
                    {
                        v.push_back(tmp);
                        break;
                    }
                    if(tmp->bl == NULL)
                    {
                        break;
                    }
                }

                dir = fac->in_dir;
                tmp->p.pos_x += dx[dir];
                tmp->p.pos_y += dy[dir];

                if(tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y)
                {
                    fac->Mineral_tackle();
                    if(tmp->bl == NULL)
                    {
                        break;
                    }
                }
            }
            else if(fac->mineral_outque == tmp)
            {
                dir = fac->out_dir;
                if(!tmp->moveable(tmp->p.pos_x, tmp->p.pos_y, dir))
                {
                    break;
                }
                tmp->p.pos_x += dx[dir];
                tmp->p.pos_y += dy[dir];

                if((dir == 0 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y - SIZE / 2) ||
                    (dir == 1 && tmp->p.pos_x == bl->middle.pos_x + SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y) ||
                    (dir == 2 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y + SIZE / 2) ||
                    (dir == 3 && tmp->p.pos_x == bl->middle.pos_x - SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y))
                {
                    if(Mineral_out(bl, fac, tmp))
                    {
                        if(!tmp->bl){
                            qDebug()<<"mineral_out error3";
                        }

                        Block* nxt; Facility *nfac;
                        switch(dir)
                        {
                        case 0:nxt = block[bl->id_x][bl->id_y - 1];break;
                        case 1:nxt = block[bl->id_x + 1][bl->id_y];break;
                        case 2:nxt = block[bl->id_x][bl->id_y + 1];break;
                        case 3:nxt = block[bl->id_x - 1][bl->id_y];break;

                        }
                        //qDebug()<<"out of situation error";
                        nfac = nxt->facility;
                        assert(nfac != NULL);
                        if(nfac->type == 2)
                        {
                            money += mineral_value[tmp->type];
                            v.push_back(tmp);
                            break;
                        }
                        tmp->bl =nxt;
                        fac->mineral_outque = NULL;
                        nfac->mineral_inque = tmp;

                        bl = tmp->bl;
                        fac = tmp->bl->facility;
                        assert(fac != NULL);
                        assert(fac->type != 0);

                        int nxtspeed;
                        switch(fac->type)
                        {
                        case 0: assert(0);break;
                        case 1: nxtspeed = harvestor_speed; break;
                        case 2: nxtspeed = base_speed; break;
                        case 3: nxtspeed = conveyer_speed; break;
                        default: nxtspeed = 0xffff; break;
                        }
                        distance = nxtspeed - nxtspeed * last / lastspeed;
                        lastspeed = nxtspeed;
                        last = 0;
                    }
                    else
                        break;
                }
            }
            else
            {
                qDebug()<<"locating mineral error2";
            }
            last++;
            distance--;
        }

        qDebug()<<"move mineral" + QString::number(idx);
    }
    for(auto tmp:v)
    {
        Block *bl = tmp->bl;
        assert(bl->facility && bl->facility->type != 0);

        mineral_num[tmp->type]++;
        money += mineral_value[tmp->type];
        delete tmp;
        bl->facility->mineral_inque = NULL;
        for(auto pai:mineral_all)
        {
            if(pai.second == tmp)
            {
                mineral_all.erase(pai.first);
                break;
            }
        }
    }
    for(auto pai:harvestor_all)
    {
        Harvestor *hav = pai.first;
        Block *bl = pai.second;
        assert(bl->mine);
        int dir = hav->out_dir;
        Mineral *tmp = new Mineral(NULL, bl, bl->middle.pos_x, bl->middle.pos_y, bl->mine->id_mineral);
        if(tmp->moveable(tmp->p.pos_x, tmp->p.pos_y, dir))
        {
            mineral_cnt++;
            mineral_all.insert(make_pair(mineral_cnt, tmp));
            hav->mineral_outque = tmp;

        }
        else
        {
            delete tmp;
        }
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

}

void GameScene::closeEvent(QCloseEvent *event)
{

}
void GameScene::keyPressEvent(QKeyEvent *event)
{

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
