#ifndef CONVEYER_H
#define CONVEYER_H

#include <QObject>
#include "block.h"
#include <QDebug>

class Conveyer : public Facility
{
    Q_OBJECT
public:
    explicit Conveyer(QObject *parent = nullptr, Block *init_bl = NULL, int init_dir = 0);
    ~Conveyer();
    //static int speed;
    void Mineral_in(Mineral *tmp);
    bool Mineral_tackle(Mineral *tmp);
    void Mineral_move();
    void resetdir();
    void settle();
};
#endif // CONVEYER_H
