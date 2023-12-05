#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include <qpixmap.h>
#include <map>
#include <QDebug>
//#include "mine.h"


using namespace std;

extern const int SIZE;

class Mineral;
class Base;         //2
class Harvestor;    //1
class Mine;
class Facility;
class Conveyer;

struct Pos{
    int pos_x, pos_y;
};

class Block : public QObject
{
    Q_OBJECT
public:

    explicit Block(QObject *parent = nullptr, int init_id_x = 0, int init_id_y = 0);
    ~Block();
    QPixmap icon;
    int id_x, id_y;
    int size;
    Pos p, middle;
    Facility *facility;
    Mine *mine;
signals:

};

class Facility : public QObject
{
    Q_OBJECT
public:
    explicit Facility(QObject *parent = nullptr, Block *init_bl = NULL, int init_type = 0, int init_dir = 0, bool init_rotatable = false);
    virtual ~Facility();
    QPixmap icon;
    int size;
    Pos middle;
    int dir;
    int in_dir, out_dir;
    bool rotatable;
    virtual bool settle_available();
    virtual void settle();
    int type;
    Mineral *mineral_inque;
    Mineral *mineral_outque;
    Block *bl = NULL;
    virtual void Mineral_in(Mineral *tmp) = 0;
    virtual bool Mineral_tackle(Mineral *tmp) = 0;
    virtual bool Mineral_out(Mineral *tmp);
    virtual void resetdir();

signals:
    void Mineral_trigger(Mineral *tmp);
};



class Mineral : public QObject
{
    Q_OBJECT
public:
    explicit Mineral(QObject *parent = nullptr, Block *init_bl = NULL, int init_pos_x = 0, int init_pos_y = 0, int init_type = 0);
    ~Mineral();
    QPixmap icon;
    int size;
    Pos p;
    Block *bl;
    int type;
    bool moveable(int x = 0, int y = 0, int dir = 0);
signals:

};


#endif // BLOCK_H
