#ifndef CUTTER_H
#define CUTTER_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include "block.h"

class Cutter : public Facility
{
    Q_OBJECT
public:
    explicit Cutter(QObject *parent = nullptr, Block *bl = NULL, int init_part = 0, int init_dir = 0, Cutter *init_another = NULL);
    ~Cutter();
    int part;
    QTimer *cd = NULL; //计时
    Cutter *another;
    bool empty;
    void Mineral_in(Mineral *tmp);
    bool Mineral_tackle(Mineral *tmp);
    bool settle_available();
    void resetdir();
    void Cutter_settle();
    bool Cutter_out();
private:
    void cut_mineral();

signals:
    void Mineral_trigger(Mineral *tmp);
};

#endif // CUTTER_H
