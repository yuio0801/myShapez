#include "block.h"
#include "cutter.h"
using namespace std;

extern int Maxx,Maxy;
extern QString Facility_name[10];
extern const int SIZE;

extern unordered_map<Mine *, Block *> mine_all;
extern unordered_map<Harvestor *, Block *> harvestor_all;
extern unordered_map<Base *, Block *> base_all;
extern unordered_map<Conveyer *, Block *> conveyer_all;
extern unordered_map<Cutter *,Block *>cutter_all;
extern unordered_map<Dustbin *, Block *> dustbin_all;
extern unordered_map<Rotater *, Block *> rotater_all;
extern map<int, Mineral *> mineral_all;
extern int mineral_cnt;

extern int money;
extern int mineral_num[5];
extern int mineral_value[5];
extern Block* block[30][20];
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
        facility = NULL;
    }
    if(mine)
    {
        delete mine;
        mine = NULL;
    }

}
void Block::clear()
{
    if(facility)
    {
        if(facility->mineral_outque)
        {
                Mineral *tmp = facility->mineral_outque;
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
            if(facility->mineral_inque)
            {
                Mineral *tmp = facility->mineral_inque;
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
            if(facility->type == 4)
            {
                Cutter *cutter = (Cutter*)facility;
                if(cutter->part == 1)
                {
                    cutter = cutter->another;
                }
                cutter_all.erase(cutter);
                cutter->another->bl->facility = NULL;
                delete cutter->another;
                delete cutter;
                cutter->bl->facility = NULL;
            }
            else
            {
                switch(facility->type)
                {
                case 1:harvestor_all.erase(((Harvestor*)facility));break;
                case 2:base_all.erase((Base*)facility);break;
                case 3:conveyer_all.erase((Conveyer*)facility);break;
                case 5:dustbin_all.erase((Dustbin*)facility);break;
                case 7:rotater_all.erase((Rotater*)facility);break;
                }

                delete facility;
                facility = NULL;
            }

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
    mineral_inque = mineral_outque = NULL;
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
    if(bl->mine) return false;
    if(bl->facility)
    {
        if(bl->facility->type == 2) return false;
        else return true;
    }
    return true;
}

/*void Facility::settle()
{
    if(!bl)
    {
        qDebug()<<Facility_name[type] +" bl error";
        return ;
    }
    assert(settle_available());

    if(bl->facility)
    {
        if(bl->facility->mineral_outque)
        {
            Mineral *tmp = bl->facility->mineral_outque;
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
        if(bl->facility->mineral_inque)
        {
            Mineral *tmp = bl->facility->mineral_inque;
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
        if(bl->facility->type == 4)
        {
            Cutter *cutter = (Cutter*)bl->facility;
            if(cutter->part == 1)
            {
                cutter = cutter->another;
            }
            cutter_all.erase(cutter);
            delete cutter->another;
            delete cutter;
        }
        else
        {
            switch(bl->facility->type)
            {
            case 1:harvestor_all.erase(((Harvestor*)bl->facility));break;
            case 2:base_all.erase((Base*)bl->facility);break;
            case 3:conveyer_all.erase((Conveyer*)bl->facility);break;
            case 5:dustbin_all.erase((Dustbin*)bl->facility);break;
            case 7:rotater_all.erase((Rotater*)bl->facility);break;
            }

            delete bl->facility;
        }
    }
    bl->facility = this;
    if(bl->facility->type == 4)
    {
        Cutter *cutter = (Cutter*)bl->facility;
        if(cutter->part == 0)
        cutter_all.insert(make_pair((Cutter*)bl->facility, bl));
    }
    else
    switch(bl->facility->type)
    {
    case 1:harvestor_all.insert(make_pair((Harvestor*)bl->facility, bl));break;
    case 2:base_all.insert(make_pair((Base*)bl->facility, bl));break;
    case 3:conveyer_all.insert(make_pair((Conveyer*)bl->facility,bl));break;
    case 5:dustbin_all.insert(make_pair((Dustbin*)bl->facility, bl));break;
    case 7:rotater_all.insert(make_pair((Rotater*)bl->facility, bl));break;
    }
}*/
void Facility::resetdir()
{
    return ;
}
bool Facility::Mineral_out(Mineral *tmp)
{
    assert(tmp!=NULL);
    int dir = out_dir;
    if(tmp->p.pos_x == 0 || tmp->p.pos_x == Maxx * SIZE || tmp->p.pos_y == 0 || tmp->p.pos_y == Maxy * SIZE)
    {
        return 0;
    }
    Block* nxt;Facility *nfac; bool flag = false;
    if(dir == 0 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y - SIZE / 2)
    {
        nxt = block[bl->id_x][bl->id_y - 1];
        if(!nxt->facility)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->type == 4)
            {
                Cutter * cutter = (Cutter*)nfac;
                if(tmp->type != 0 || cutter->part != 0 || cutter->in_dir != 0 || !cutter->empty || !cutter->Cutter_out() || cutter->mineral_outque || cutter->another->mineral_outque)
                {
                    flag = false;
                }
                else
                {
                    flag = true;
                }
            }
            else if(nfac->type == 7)
            {
                if(tmp->type != 2 || nfac->in_dir != 0)
                {
                    flag = false;
                }
                else
                {
                    flag = true;
                }
            }
            else if(nfac->in_dir == -1 || nfac->in_dir == 0)
            {
                flag = true;
            }
        }

    }
    else if(dir == 1 && tmp->p.pos_x == bl->middle.pos_x + SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y)
    {
        nxt = block[bl->id_x + 1][bl->id_y];
        if(!nxt->facility)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->type == 4)
            {
                Cutter * cutter = (Cutter*)nfac;
                if(tmp->type != 0 || cutter->part != 0 || cutter->in_dir != 1 || !cutter->empty || !cutter->Cutter_out() || cutter->mineral_outque || cutter->another->mineral_outque)
                {
                    flag = false;
                }
                else
                {
                    flag = true;
                }
            }
            else if(nfac->type == 7)
            {
                if(tmp->type != 2 || nfac->in_dir != 1)
                {
                    flag = false;
                }
                else
                {
                    flag = true;
                }
            }
            else if(nfac->in_dir == -1 || nfac->in_dir == 1)
            {
                flag = true;
            }
        }
    }
    else if(dir == 2 && tmp->p.pos_x == bl->middle.pos_x && tmp->p.pos_y == bl->middle.pos_y + SIZE / 2)
    {
        nxt = block[bl->id_x][bl->id_y + 1];
        if(!nxt->facility)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->type == 4)
            {
                Cutter * cutter = (Cutter*)nfac;
                if(tmp->type != 0 || cutter->part != 0 || cutter->in_dir != 2 || !cutter->empty || !cutter->Cutter_out() || cutter->mineral_outque || cutter->another->mineral_outque)
                {
                    flag = false;
                }
                else
                {
                    flag = true;
                }
            }
            else if(nfac->type == 7)
            {
                if(tmp->type != 2 || nfac->in_dir != 2)
                {
                    flag = false;
                }
                else
                {
                    flag = true;
                }
            }
            else if(nfac->in_dir == -1 || nfac->in_dir == 2)
            {
                flag = true;
            }
        }
    }
    else if(dir == 3 && tmp->p.pos_x == bl->middle.pos_x - SIZE / 2 && tmp->p.pos_y == bl->middle.pos_y)
    {
        nxt = block[bl->id_x - 1][bl->id_y];
        if(!nxt->facility)
        {
            flag = false;
        }
        else
        {
            nfac = nxt->facility;
            if(nfac->type == 4)
            {
                Cutter * cutter = (Cutter*)nfac;
                if(tmp->type != 0 || cutter->part != 0 || cutter->in_dir != 3 || !cutter->empty || !cutter->Cutter_out() || cutter->mineral_outque || cutter->another->mineral_outque)
                {
                    flag = false;
                }
                else
                {
                    flag = true;
                }
            }
            else if(nfac->type == 7)
            {
                if(tmp->type != 2 || nfac->in_dir != 3)
                {
                    flag = false;
                }
                else
                {
                    flag = true;
                }
            }
            else if(nfac->in_dir == -1 || nfac->in_dir == 3)
            {
                flag = true;
            }
        }
    }
    assert(tmp->p.pos_x % 25 == 0 && tmp->p.pos_y % 25 == 0);
    //qDebug()<<"out of situation error";
    return flag;
}

Mineral::Mineral(QObject *parent, Block *init_bl, int init_pos_x , int init_pos_y, int init_type)
    : QObject{parent}
{
    size = SIZE;
    bl = init_bl;
    p.pos_x = init_pos_x;
    p.pos_y = init_pos_y;
    type = init_type;
    dir = 0;
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
    int nxt_x = x, nxt_y = y;
    for(auto pia: mineral_all)
    {
        Mineral *tmp = pia.second;
        if(tmp == this) continue;
        if(tmp->p.pos_x - SIZE / 2 <= nxt_x && nxt_x <= tmp->p.pos_x + SIZE / 2 &&
            tmp->p.pos_y - SIZE / 2 <= nxt_y && nxt_y <= tmp->p.pos_y + SIZE / 2)
        {
            return false;
        }
    }
    switch(dir)
    {
    case 0:nxt_x = x; nxt_y = y - SIZE / 2 + dy[dir];break;
    case 1:nxt_x = x + SIZE / 2 + dx[dir]; nxt_y = y;break;
    case 2:nxt_x = x; nxt_y = y + SIZE / 2 + dy[dir];break;
    case 3:nxt_x = x - SIZE / 2 + dx[dir]; nxt_y = y;break;
    default:assert(0);
    }
    for(auto pia: mineral_all)
    {
        Mineral *tmp = pia.second;
        if(tmp == this) continue;
        if(tmp->p.pos_x - SIZE / 2 <= nxt_x && nxt_x <= tmp->p.pos_x + SIZE / 2 &&
            tmp->p.pos_y - SIZE / 2 <= nxt_y && nxt_y <= tmp->p.pos_y + SIZE / 2)
        {
            return false;
        }
    }
    return true;
}
