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


    //qDebug()<<this->rotatable;
}

Harvestor::~Harvestor()
{
    qDebug()<<"delete harvestor";
}

bool Harvestor::Mineral_tackle()
{
    return false;
}

bool Harvestor::settle_available()
{
    if(!bl)
    {
        qDebug()<<Facility_name[type] +" bl error";
        return false;
    }
    if(bl->facility == NULL && bl->mine) return true;
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
    //if(settle_available())
    assert(bl->mine != NULL);
    bl->facility = this;
    harvestor_all.insert(make_pair(this, bl));
}
