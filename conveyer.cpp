#include "conveyer.h"

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

extern int money;
extern int mineral_num[4];
extern int mineral_value[4];
extern Block* block[30][20];

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
        out_dir = 1;
        break;
    case 2:
        in_dir = 2;
        out_dir = 2;
        break;
    case 3:
        in_dir = 3;
        out_dir = 3;
        break;
    case 4:
        in_dir = 0;
        out_dir = 3;
        break;
    case 5:
        in_dir = 0;
        out_dir = 1;
        break;
    case 6:
        in_dir = 1;
        out_dir = 0;
        break;
    case 7:
        in_dir = 1;
        out_dir = 2;
        break;
    case 8:
        in_dir = 2;
        out_dir = 3;
        break;
    case 9:
        in_dir = 2;
        out_dir = 1;
        break;
    case 10:
        in_dir = 3;
        out_dir = 0;
        break;
    case 11:
        in_dir = 3;
        out_dir = 2;
        break;
    default:
        break;
    }
    connect(this, &Facility::Mineral_trigger, this, &Conveyer::Mineral_tackle);
}

Conveyer::~Conveyer()
{
    qDebug()<<"delete conveyer";
}


bool Conveyer::Mineral_tackle(Mineral *tmp)
{
    assert(tmp != NULL);
    mineral_inque = NULL;
    mineral_outque = tmp;
    return false;
}

void Conveyer::Mineral_move()
{
    if(!mineral_inque && !mineral_outque) return ;
    Mineral *tmp = NULL;
    if(mineral_outque)
    {
        dir = out_dir;
        tmp = mineral_outque;
        if(!tmp->moveable(tmp->p.pos_x, tmp->p.pos_y, dir))
        {
            qDebug()<<"Mineral blocked at" + QString::number(tmp->p.pos_x) + " " + QString::number(tmp->p.pos_y);
            return ;
        }

        if((dir == 0 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y - SIZE / 2) ||
            (dir == 1 && tmp->p.pos_x == bl->middle.pos_x + SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y) ||
            (dir == 2 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y + SIZE / 2) ||
            (dir == 3 && tmp->p.pos_x == bl->middle.pos_x - SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y))
        {
            if(Mineral_out(tmp))
            {
                Block* nxt = NULL; Facility *nfac = NULL;
                switch(dir)
                {
                case 0:nxt = block[bl->id_x][bl->id_y - 1];break;
                case 1:nxt = block[bl->id_x + 1][bl->id_y];break;
                case 2:nxt = block[bl->id_x][bl->id_y + 1];break;
                case 3:nxt = block[bl->id_x - 1][bl->id_y];break;
                default:assert(0);
                }
                nfac = nxt->facility;
                assert(nfac != NULL);
                nfac->Mineral_in(tmp);
                mineral_outque = NULL;
                return ;
            }
            else
            {
                qDebug()<<"Mineral blocked at the end of this conveyer";
                return ;
            }
        }
    }
    else if(mineral_inque)
    {
        dir = in_dir;
        tmp = mineral_inque;
        if(tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y)
        {
            emit Mineral_trigger(tmp);
            dir = out_dir;
        }
    }
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};
    tmp->p.pos_x += dx[dir];
    tmp->p.pos_y += dy[dir];
    return ;
}

void Conveyer::Mineral_in(Mineral *tmp)
{
    //assert(mineral_outque == NULL);
    mineral_inque = tmp;
    return ;
}

void Conveyer::resetdir()
{
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
        out_dir = 1;
        break;
    case 2:
        in_dir = 2;
        out_dir = 2;
        break;
    case 3:
        in_dir = 3;
        out_dir = 3;
        break;
    case 4:
        in_dir = 0;
        out_dir = 3;
        break;
    case 5:
        in_dir = 0;
        out_dir = 1;
        break;
    case 6:
        in_dir = 1;
        out_dir = 0;
        break;
    case 7:
        in_dir = 1;
        out_dir = 2;
        break;
    case 8:
        in_dir = 2;
        out_dir = 3;
        break;
    case 9:
        in_dir = 2;
        out_dir = 1;
        break;
    case 10:
        in_dir = 3;
        out_dir = 0;
        break;
    case 11:
        in_dir = 3;
        out_dir = 2;
        break;
    default:
        break;
    }
}
