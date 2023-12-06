#ifndef DUSTBIN_H
#define DUSTBIN_H

#include <QObject>
#include "base.h"
#include <QDebug>

class Dustbin : public Base
{
    Q_OBJECT
public:
    explicit Dustbin(QObject *parent = nullptr, Block *bl = NULL);
    ~Dustbin();
    void Mineral_in(Mineral *tmp);
    bool Mineral_tackle(Mineral *tmp);
signals:
    void Mineral_trigger(Mineral *tmp);
};

#endif // DUSTBIN_H
