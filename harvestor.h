#ifndef HARVESTOR_H
#define HARVESTOR_H

#include <QObject>
#include "block.h"
#include <QDebug>

class Harvestor : public Facility
{
    Q_OBJECT
public:
    explicit Harvestor(QObject *parent = nullptr, Block *init_bl = NULL, int init_dir = 0);
    ~Harvestor();
    virtual bool settle_available();
    virtual void settle();
    //static int speed;
    void Mineral_in(Mineral *tmp);
    bool Mineral_tackle(Mineral *tmp);
    bool Mineral_out(Mineral *tmp);
};


#endif // HARVESTOR_H
