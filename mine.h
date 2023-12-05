#ifndef MINE_H
#define MINE_H
#include <QObject>
#include <qpixmap.h>
#include <map>
#include "block.h"
#include <QDebug>

class Mine : public QObject
{
    Q_OBJECT
public:
    explicit Mine(QObject *parent = nullptr, Block *init_bl = NULL, int init_id_mineral = 0);
    ~Mine();
    Harvestor *harvestor= NULL;
    int id_mineral;
    QPixmap icon;
    int size;
    Pos middle;
    bool settle_available();
    void settle();
    Block *bl = NULL;
private:


signals:

};
#endif // MINE_H
