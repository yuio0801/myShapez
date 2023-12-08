#ifndef ROTATER_H
#define ROTATER_H

#include <QObject>
#include "conveyer.h"
#include <QDebug>

class Rotater : public Conveyer
{
    Q_OBJECT
public:
    explicit Rotater(QObject *parent = nullptr, Block *init_bl = NULL, int init_dir = 0);
    ~Rotater();
    //static int speed;
    virtual void Mineral_in(Mineral *tmp);
    virtual bool Mineral_tackle(Mineral *tmp);
    virtual void Mineral_move();
    void settle();
    virtual void resetdir();
signals:
    void Mineral_trigger(Mineral *tmp);
};

#endif // ROTATER_H
