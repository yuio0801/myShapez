#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include <qpixmap.h>

class Harvestor;
class Mineral;
class Mine;
class Facility;

class Block : public QObject
{
    Q_OBJECT
public:

    explicit Block(QObject *parent = nullptr, int id_x = 0, int id_y = 0);
    ~Block();
    QPixmap icon;
    int x, y;
    int size;
    int pos_x, pos_y;
    Facility *facility;

signals:

};

class Facility : public QObject
{
    Q_OBJECT
public:
    explicit Facility(QObject *parent = nullptr, Block *init_bl = NULL, int init_type = 0, bool init_rotatable = false);
    virtual ~Facility();
    QPixmap icon;
    int size;
    int middle_x, middle_y;
    bool rotatable;
    //virtual bool settle_available()=0;
    void settle();
protected:
    Block *bl = NULL;
    int type;

signals:

};

class Mine : public Facility
{
    Q_OBJECT
public:
    explicit Mine(QObject *parent = nullptr, Block *init_bl = NULL, int init_id_mineral = 0);
    ~Mine();
private:
    int id_mineral;
    Harvestor *harvestor= NULL;
signals:

};

class Harvestor : public Facility
{
    Q_OBJECT
public:
    explicit Harvestor(QObject *parent = nullptr, Block *init_bl = NULL, int init_dir = 0);

    int dir = 0;
};

class Mineral : public QObject
{
    Q_OBJECT
public:
    explicit Mineral(QObject *parent = nullptr, int init_pos_x = 0, int init_pos_y = 0);
    QPixmap icon;
    int size;
    int pos_x, pos_y;
    int middle_x, middle_y;

signals:

};


#endif // BLOCK_H
