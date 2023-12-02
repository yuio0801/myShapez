#include "block.h"
#include <QDebug>

#define SIZE 30

extern int Maxx,Maxy;
extern QString Facility_name[10];

Block::Block(QObject *parent, int id_x, int id_y)
    : QObject{parent}
{
    icon.load(":/res/block.png");
    if(icon.isNull())
        qDebug()<<"open block icon"<<"[" + QString::number(id_x) + "]" + "[" + QString::number(id_y) + "]" + "fails";
    x = id_x, y = id_y;
    size = SIZE;
    pos_x = x * size;
    pos_y = y * size;
    facility = NULL;
    qDebug()<<"create block:"<<x<<" "<<y;

}

Block::~Block()
{
    qDebug()<<"delete block:"<<x<<" "<<y;
    if(facility)
    {
        delete facility;
    }

}

Facility::Facility(QObject *parent, Block *init_bl, int init_type, bool init_rotatable)
    : QObject{parent}
{
    size = SIZE;
    bl = init_bl;
    if(!bl)
    {
        qDebug()<<"bl error in creat facility";
        return ;
    }
    middle_x = bl->pos_x + size / 2;
    middle_y = bl->pos_y + size / 2;
    rotatable = init_rotatable;
    type = init_type;
}

Facility::~Facility()
{
    qDebug()<<"delete "+ Facility_name[type];
}

void Facility::settle()
{
    if(!bl)
    {
        qDebug()<<Facility_name[type] +" bl error";
        return ;
    }
    //if(settle_available())
    bl->facility = this;

}

Mine::Mine(QObject *parent, Block *init_bl, int init_id_mineral)
    :Facility(parent, init_bl, 0, false)
{
    qDebug()<<"create mine";

    id_mineral = init_id_mineral;
    harvestor = NULL;

    icon.load(":/res/facility0_" + QString::number(id_mineral));
    if(icon.isNull())
        qDebug()<<"open mine icon fail";

}

Mine::~Mine()
{
    /*TODO*/
    ;
}

Harvestor::Harvestor(QObject *parent, Block *init_bl, int init_dir)
    :Facility(parent, init_bl, 1, true)
{
    qDebug()<<"create harvestor";

    dir = init_dir;

    icon.load(":/res/facility1_" + QString::number(dir));
    if(icon.isNull())
        qDebug()<<"open harvestor icon fail";


    //qDebug()<<this->rotatable;
}


Mineral::Mineral(QObject *parent, int init_pos_x, int init_pos_y)
    : QObject{parent}
{
    pos_x = init_pos_x, pos_y = init_pos_y;
    size = SIZE;
    middle_x = pos_x + size / 2;
    middle_y = pos_y + size / 2;

}
