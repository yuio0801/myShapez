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
extern Block* block[30][20];
Base::Base(QObject *parent, Block *init_bl)
    :Facility(parent, init_bl, 2, 0, false)
{
    qDebug()<<"create base";

    in_dir = -1;
    icon.load(":/res/facility2");
    if(icon.isNull())
        qDebug()<<"open harvestor icon fail";
    connect(this, &Facility::Mineral_trigger, this, &Base::Mineral_tackle);

    //qDebug()<<this->rotatable;
}

Base::~Base()
{
    qDebug()<<"delete base";
}

void Base::Mineral_in(Mineral *tmp)
{
    qDebug()<<"Mineral type" + QString::number(tmp->type) + "into the base, earn " + QString::number(mineral_value[tmp->type]);
    emit Mineral_trigger(tmp);

}
bool Base::Mineral_tackle(Mineral *tmp)
{
    money += mineral_value[tmp->type];
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
}
bool Base::Mineral_out(Mineral *tmp)
{
    assert(0);
    return false;
}
