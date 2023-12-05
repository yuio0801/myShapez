#include "base.h"

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

Base::Base(QObject *parent, Block *init_bl)
    :Facility(parent, init_bl, 2, 0, false)
{
    qDebug()<<"create base";

    in_dir = -1;
    icon.load(":/res/facility2");
    if(icon.isNull())
        qDebug()<<"open harvestor icon fail";


    //qDebug()<<this->rotatable;
}

Base::~Base()
{
    qDebug()<<"delete base";
}

bool Base::Mineral_tackle()
{
    qDebug()<<"base take mineral";
    return true;
}
