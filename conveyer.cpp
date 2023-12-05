#include "conveyer.h"

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

Conveyer::Conveyer(QObject *parent, Block *init_bl, int init_dir)
    :Facility(parent, init_bl, 3, init_dir, true)
{
    qDebug()<<"create conveyer";

    icon.load(":/res/facility3_" + QString::number(dir));
    if(icon.isNull())
        qDebug()<<"open conveyer icon fail";
    switch(dir)
    {
    case 0:
        in_dir = 0;
        out_dir = 0;
        break;
    case 1:
        in_dir = 1;
        out_dir = 0;
        break;
    case 2:
        in_dir = 1;
        out_dir = 1;
        break;
    case 3:
        in_dir = 1;
        out_dir = 2;
        break;
    case 4:
        in_dir = 2;
        out_dir = 2;
        break;
    case 5:
        in_dir = 2;
        out_dir = 3;
        break;
    case 6:
        in_dir = 3;
        out_dir = 3;
        break;
    case 7:
        in_dir = 3;
        out_dir = 0;
        break;
    default:
        break;
    }

}

Conveyer::~Conveyer()
{
    qDebug()<<"delete conveyer";
}


bool Conveyer::Mineral_tackle()
{
    Mineral *tmp = mineral_inque;
    mineral_inque = NULL;
    mineral_outque = tmp;
    return false;
}
