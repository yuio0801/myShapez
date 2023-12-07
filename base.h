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
    virtual void Mineral_in(Mineral *tmp);
    virtual bool Mineral_tackle(Mineral *tmp);
    virtual bool Mineral_out(Mineral *tmp);\
        void settle();
        signals:
    void Mineral_trigger(Mineral *tmp);
};

#endif // BASE_H
