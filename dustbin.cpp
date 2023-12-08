#include "dustbin.h"

extern int Maxx,Maxy;
extern QString Facility_name[10];
extern const int SIZE;

extern unordered_map<Mine *, Block *> mine_all;
extern unordered_map<Harvestor *, Block *> harvestor_all;
extern unordered_map<Base *, Block *> base_all;
extern unordered_map<Conveyer *, Block *> conveyer_all;
extern unordered_map<Cutter *,Block *>cutter_all;
extern unordered_map<Dustbin *, Block *> dustbin_all;
extern map<int, Mineral *> mineral_all;
extern int mineral_cnt;

extern int money;
extern int mineral_num[5];
extern int mineral_value[5];
extern Block* block[30][20];
Dustbin::Dustbin(QObject *parent, Block *init_bl)
    :Base(parent, init_bl)
{
    qDebug()<<"create dustbin";

    icon.load(":/res/facility2_1");
    if(icon.isNull())
        qDebug()<<"open dustbin icon fail";
    type = 5;
    connect(this, &Dustbin::Mineral_trigger, this, &Dustbin::Mineral_tackle);

    //qDebug()<<this->rotatable;
}

Dustbin::~Dustbin()
{
    qDebug()<<"delete dustbin";
}
void Dustbin::settle()
{
    if(!bl)
    {
        qDebug()<<Facility_name[type] +" bl error";
        return ;
    }
    assert(settle_available());

    bl->clear();
    bl->facility = this;
    dustbin_all.insert(make_pair(this,bl));
}

void Dustbin::Mineral_in(Mineral *tmp)
{
    qDebug()<<"Mineral type" + QString::number(tmp->type) + " into the dustbin";
    emit this->Mineral_trigger(tmp);

}
bool Dustbin::Mineral_tackle(Mineral *tmp)
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
    return false;
}
