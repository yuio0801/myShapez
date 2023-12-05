#ifndef BASE_H
#define BASE_H

#include <QObject>
#include "block.h"
#include <QDebug>

class Base : public Facility
{
    Q_OBJECT
public:
    explicit Base(QObject *parent = nullptr, Block *init_bl = NULL);
    ~Base();
    //static int speed;
    void Mineral_in(Mineral *tmp);
    bool Mineral_tackle(Mineral *tmp);
    bool Mineral_out(Mineral *tmp);
};

#endif // BASE_H
