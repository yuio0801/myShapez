#include "mine.h"

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

Mine::Mine(QObject *parent, Block *init_bl, int init_id_mineral)
{
    bl = init_bl;
    id_mineral = init_id_mineral;
    harvestor = NULL;
    icon.load(":/res/facility0_" + QString::number(id_mineral));
    if(icon.isNull())
        qDebug()<<"open mine icon fail";

    qDebug()<<"create mine";
}

Mine::~Mine()
{
    /*TODO*/
    qDebug()<<"delete miner";
    ;
}

bool Mine::settle_available()
{
    if(!bl)
    {
        qDebug()<<"Mine bl error";
        return false;
    }
    if(bl->facility == NULL) return true; // not base
    else return false;
}

void Mine::settle()
{
    if(!bl)
    {
        qDebug()<<"Mine bl error";
        return ;
    }
    //if(settle_available())
    //bl->clear_facility();
    bl->mine = this;
    mine_all.insert(make_pair((Mine*)bl->facility, bl));

}
