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
    static int speed;
    bool Mineral_tackle();
};

#endif // BASE_H
