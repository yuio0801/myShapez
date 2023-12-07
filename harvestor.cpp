#include "harvestor.h"

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
extern Block* block[30][20];

Harvestor::Harvestor(QObject *parent, Block *init_bl, int init_dir)
    :Facility(parent, init_bl, 1, init_dir, true)
{
    qDebug()<<"create harvestor";

    dir = init_dir;
    in_dir = -2;
    out_dir = dir;

    icon.load(":/res/facility1_" + QString::number(dir));
    if(icon.isNull())
        qDebug()<<"open harvestor icon fail";

}

Harvestor::~Harvestor()
{
        bl->facility = NULL;
    qDebug()<<"delete harvestor";
}

bool Harvestor::settle_available()
{
    if(!bl)
    {
        qDebug()<<Facility_name[type] +" bl error";
        return false;
    }
    if(bl->mine) return true;
    else
    {
        qDebug()<<"Can't put harvestor here";
        return false;
    }
}

void Harvestor::settle()
{
    if(!bl)
    {
        qDebug()<<Facility_name[type] +" bl error";
        return ;
    }
    assert(settle_available());

    bl->clear();
    bl->facility = this;
    harvestor_all.insert(make_pair(this,bl));
}


void Harvestor::Mineral_in(Mineral *tmp)
{
    assert(0);

}
bool Harvestor::Mineral_tackle(Mineral *tmp)
{
    assert(0);
    return false;
}
void Harvestor::resetdir()
{
    icon.load(":/res/facility1_" + QString::number(dir));
    if(icon.isNull())
        qDebug()<<"open harvestor icon fail";
    switch(dir)
    {
    case 0:
        out_dir = 0;
        break;
    case 1:
        out_dir = 1;
        break;
    case 2:
        out_dir = 2;
        break;
    case 3:
        out_dir = 3;
        break;
    default:
        break;
    }
}
bool Harvestor::Mineral_out(Mineral *tmp)
{
    int dir = out_dir;
    if(tmp->p.pos_x == 0 || tmp->p.pos_x == Maxx * SIZE || tmp->p.pos_y == 0 || tmp->p.pos_y == Maxy * SIZE)
    {
        return 0;
    }
    Block* nxt;Facility *nfac; bool flag = false;
    if(dir == 0 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y - SIZE / 2)
    {
        nxt = block[bl->id_x][bl->id_y - 1];
        if(!nxt->facility || nxt->facility->type != 3)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->type != 3)
            {
                flag = false;
            }
            else if(nfac->in_dir == -1 || nfac->in_dir == 0)
            {
                flag = true;
            }

        }
        //if(/*TODO:CUTTER*/)
        }
    else if(dir == 1 && tmp->p.pos_x == bl->middle.pos_x + SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y)
    {
        nxt = block[bl->id_x + 1][bl->id_y];
        if(!nxt->facility || nxt->facility->type != 3)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->type != 3)
            {
                flag = false;
            }
            else
            if(nfac->in_dir == -1 || nfac->in_dir == 1)
            {
                //assert(nfac->mineral_inque == NULL);
                flag = true;
            }
        }
    }
    else if(dir == 2 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y + SIZE / 2)
    {
        nxt = block[bl->id_x][bl->id_y + 1];
        if(!nxt->facility || nxt->facility->type != 3)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->type != 3)
            {
                flag = false;
            }
            else
            if(nfac->in_dir == -1 || nfac->in_dir == 2)
            {
                flag = true;
            }
        }
    }
    else if(dir == 3 && tmp->p.pos_x == bl->middle.pos_x - SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y)
    {
        nxt = block[bl->id_x - 1][bl->id_y];
        if(!nxt->facility || nxt->facility->type != 3)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->type != 3)
            {
                flag = false;
            }
            else
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
