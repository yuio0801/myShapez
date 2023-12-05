#include "block.h"

using namespace std;

extern int Maxx,Maxy;
extern QString Facility_name[10];
extern const int SIZE;

extern unordered_map<Mine *, Block *> mine_all;
extern unordered_map<Harvestor *, Block *> harvestor_all;
extern unordered_map<Base *, Block *> base_all;
extern unordered_map<Conveyer *, Block *> conveyer_all;

extern map<int, Mineral *> mineral_all;
extern int mineral_cnt;

extern int money;
extern int mineral_num[4];
extern int mineral_value[4];

Block::Block(QObject *parent, int init_id_x, int init_id_y)
    : QObject{parent}
{
    icon.load(":/res/block.png");
    if(icon.isNull())
        qDebug()<<"open block icon"<<"[" + QString::number(id_x) + "]" + "[" + QString::number(id_y) + "]" + "fails";
    id_x = init_id_x, id_y = init_id_y;
    size = SIZE;
    p.pos_x = id_x * size;
    p.pos_y = id_y * size;
    middle.pos_x = p.pos_x + SIZE / 2;
    middle.pos_y = p.pos_y + SIZE / 2;
    facility = NULL;
    mine  = NULL;


    qDebug()<<"create block:"<<id_x<<" "<<id_y;

}

Block::~Block()
{
    qDebug()<<"delete block:"<<id_x<<" "<<id_y;
    if(facility)
    {
        delete facility;
    }
    if(mine)
    {
        delete mine;
    }
}

Facility::Facility(QObject *parent, Block *init_bl, int init_type, int init_dir, bool init_rotatable)
    : QObject{parent}
{
    size = SIZE;
    bl = init_bl;
    if(!bl)
    {
        qDebug()<<"bl error in creat facility";
        return ;
    }
    middle.pos_x = bl->middle.pos_x;
    middle.pos_y = bl->middle.pos_y;
    dir = init_dir;
    if(1/*TODO*/)
    {
        in_dir = dir;
        out_dir = dir;
    }

    rotatable = init_rotatable;
    type = init_type;
}

Facility::~Facility()
{
    //qDebug()<<"delete "+ Facility_name[type];

}

bool Facility::settle_available()
{
    if(!bl)
    {
        qDebug()<<Facility_name[type] +" bl error";
        return false;
    }
    if(bl->facility == NULL && bl->mine == NULL) return true; // not base
    else return false;
}

void Facility::settle()
{
    if(!bl)
    {
        qDebug()<<Facility_name[type] +" bl error";
        return ;
    }
    //if(settle_available())

    if(bl->facility)
    {
        switch(bl->facility->type)
        {
        case 2:base_all.erase((Base*)bl->facility);
        case 3:conveyer_all.erase((Conveyer*)bl->facility);
        }

        delete bl->facility;
    }
    bl->facility = this;
    switch(bl->facility->type)
    {
    case 1:harvestor_all.insert(make_pair((Harvestor*)bl->facility, bl));
    case 2:base_all.insert(make_pair((Base*)bl->facility, bl));
    case 3:conveyer_all.insert(make_pair((Conveyer*)bl->facility,bl));
    }
}


Mineral::Mineral(QObject *parent, Block *init_bl, int init_pos_x , int init_pos_y, int init_type)
    : QObject{parent}
{
    size = SIZE;
    bl = init_bl;
    p.pos_x = init_pos_x;
    p.pos_y = init_pos_y;
    type = init_type;
    icon.load(":/res/mineral" + QString::number(type));
    if(icon.isNull())
        qDebug()<<"open mineral icon fail";
}

Mineral::~Mineral()
{
    //qDebug()<<"delete Mineral";
}


bool Mineral::moveable(int x, int y, int dir)
{
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};
    int nxt_x, nxt_y;
    switch(dir)
    {
    case 0:nxt_x = x; nxt_y = y - SIZE / 2 + dy[dir];break;
    case 1:nxt_x = x + SIZE / 2 + dx[dir]; nxt_y = y;break;
    case 2:nxt_x = x; nxt_y = y + SIZE / 2 + dy[dir];break;
    case 3:nxt_x = x - SIZE / 2 + dx[dir]; nxt_y = y;break;
    }
    for(auto pia: mineral_all)
    {
        Mineral *tmp = pia.second;
        if(tmp == this) continue;
        if(tmp->p.pos_x - SIZE / 2 <= nxt_x && nxt_x < tmp->p.pos_x + SIZE / 2 &&
            tmp->p.pos_y - SIZE / 2 <= nxt_y && nxt_y < tmp->p.pos_y + SIZE / 2)
        {
            return false;
        }
    }
    return true;
}
