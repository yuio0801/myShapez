#include "cutter.h"

extern int Maxx,Maxy;
extern QString Facility_name[10];
extern const int SIZE;

extern unordered_map<Mine *, Block *> mine_all;
extern unordered_map<Harvestor *, Block *> harvestor_all;
extern unordered_map<Base *, Block *> base_all;
extern unordered_map<Conveyer *, Block *> conveyer_all;
extern unordered_map<Cutter *,Block *>cutter_all;
extern map<int, Mineral *> mineral_all;
extern int mineral_cnt;
extern int cutter_speed;
extern int money;
extern int mineral_num[4];
extern int mineral_value[4];
extern Block* block[30][20];

Cutter::Cutter(QObject *parent, Block *init_bl, int init_part, int init_dir, Cutter *init_another)
    :Facility(parent, init_bl, 4, init_dir, true)
{
    qDebug()<<"create Cutter";
    part = init_part;
    empty = true;
    icon.load(":/res/facility4_" + QString::number(part) + "_" + QString::number(dir));
    if(icon.isNull())
        qDebug()<<"open cutter icon fail";

    cd = new QTimer();
    if(part == 0)
    {
        connect(this, &Cutter::Mineral_trigger, this, &Cutter::Mineral_tackle);
        connect(cd, &QTimer::timeout, this, [&](){
                empty = true;this->cut_mineral();
        });
        connect(cd, &QTimer::timeout, this, [this](){
            this->cd->stop();
        });
        Block* nxt = NULL;
        switch(dir)
        {
        case 0:nxt = block[bl->id_x + 1][bl->id_y];break;
        case 1:nxt = block[bl->id_x][bl->id_y + 1];break;
        case 2:nxt = block[bl->id_x - 1][bl->id_y];break;
        case 3:nxt = block[bl->id_x][bl->id_y - 1];break;
        default:assert(0);
        }
        another = new Cutter(parent, nxt, 1, dir, this);
    }
    else
    {
        another = init_another;
    }

}

Cutter::~Cutter()
{
    qDebug()<<"delete cutter";
}

void Cutter::resetdir()
{
    int nex = 0, ney = 0;
    switch(dir)
    {
    case 0:nex = bl->id_x, ney = bl->id_y + 1; break;
    case 1:nex = bl->id_x - 1, ney = bl->id_y;break;
    case 2:nex = bl->id_x, ney = bl->id_y - 1;break;
    case 3:nex = bl->id_x + 1, ney = bl->id_y;break;
    default:assert(0);
    }
    if(nex < 0 || nex >= Maxx || ney < 0 || ney >= Maxy)
    {
        return ;
    }
    another->bl->facility = NULL;
    another->bl = block[nex][ney];
    dir = (dir + 1) % 4;
    in_dir = out_dir = dir;
    another->dir = another->in_dir = another->out_dir = dir;
    icon.load(":/res/facility4_0_" + QString::number(dir));
    another->icon.load(":/res/facility4_1_" + QString::number(dir));
}

void Cutter::cut_mineral()
{
    assert(part == 0);
    if(part == 1) return ;
    empty = true;
    Block *bl1 = bl, *bl2 = NULL;
    switch(dir)
    {
    case 0:bl1 = block[bl->id_x][bl->id_y - 1];bl2 = block[bl->id_x + 1][bl->id_y - 1]; break;
    case 1:bl1 = block[bl->id_x + 1][bl->id_y];bl2 = block[bl->id_x + 1][bl->id_y + 1];break;
    case 2:bl1 = block[bl->id_x][bl->id_y + 1];bl2 = block[bl->id_x - 1][bl->id_y + 1];break;
    case 3:bl1 = block[bl->id_x - 1][bl->id_y];bl2 = block[bl->id_x - 1][bl->id_y - 1];break;
    default:assert(0);
    }

    Mineral *tmp = new Mineral(NULL, bl1, bl1->middle.pos_x, bl1->middle.pos_y, 4);
    switch(dir)
    {
    case 0:tmp->p.pos_y += SIZE / 2; break;
    case 1:tmp->p.pos_x -= SIZE / 2; break;
    case 2:tmp->p.pos_y -= SIZE / 2; break;
    case 3:tmp->p.pos_x += SIZE / 2; break;
    default:assert(0);
    }
    mineral_cnt++;
    mineral_all.insert(make_pair(mineral_cnt, tmp));
    assert(bl1->facility);
    bl1->facility->Mineral_in(tmp);

    tmp = new Mineral(NULL, bl2, bl2->middle.pos_x, bl2->middle.pos_y, 3);
    switch(dir)
    {
    case 0:tmp->p.pos_y += SIZE / 2; break;
    case 1:tmp->p.pos_x -= SIZE / 2; break;
    case 2:tmp->p.pos_y -= SIZE / 2; break;
    case 3:tmp->p.pos_x += SIZE / 2; break;
    default:assert(0);
    }
    mineral_cnt++;
    mineral_all.insert(make_pair(mineral_cnt, tmp));
    assert(bl2->facility);
    bl2->facility->Mineral_in(tmp);

}

bool Cutter::settle_available()
{
    bool flag = true;
    if(!bl)
    {
        qDebug()<<Facility_name[type] +" bl error";
        flag = false;
        return flag;
    }
    if(bl->mine)
    {
        flag = false;
    }
    if(bl->facility)
    {
        if(bl->facility->type == 1 || bl->facility->type == 2)
        {
            flag = false;
        }
        else
        {
            flag = true;
        }
    }
    if(part == 0)
    {
        if(!another->bl)
        {
            qDebug()<<Facility_name[type] +" bl error";
            flag = false;
            return flag;
        }
        if(another->bl->mine)
        {
            flag = false;
        }
        if(another->bl->facility)
        {
            if(another->bl->facility->type == 1 || another->bl->facility->type == 2)
            {
                flag = false;
            }
            else
            {
                flag = true;
            }
        }
    }
    return flag;
}

bool Cutter::Cutter_out()
{
    Mineral *tmp;
    if(part == 0)
    tmp = new Mineral(NULL, bl, bl->middle.pos_x, bl->middle.pos_y, 4);
    else
    tmp = new Mineral(NULL, bl, bl->middle.pos_x, bl->middle.pos_y, 3);
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
    if(!this->Facility::Mineral_out(tmp))
    {
        flag = false;
    }

    if(part == 0)
    {
        delete tmp;
        return flag && another->Cutter_out();
    }
    else
    {
        delete tmp;
        return flag;
    }
}

void Cutter::Cutter_settle()
{
    assert(part == 0);
    this->Facility::settle();
    another->Facility::settle();
    return ;
}

void Cutter::Mineral_in(Mineral *tmp)
{
    qDebug()<<"Mineral type" + QString::number(tmp->type) + " into the cutter" + QString::number(part);

    emit this->Mineral_trigger(tmp);
}

bool Cutter::Mineral_tackle(Mineral *tmp)
{
    int idx = 0;
    for(auto pai:mineral_all)
    {
        if(pai.second == tmp)
        {
            idx = pai.first;
            break;
        }
    }
    mineral_all.erase(idx);
    delete tmp;

    cd->start(cutter_speed);
    empty = false;
    return false;
}

