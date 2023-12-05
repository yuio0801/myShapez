#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include <QDebug>
#include "block.h"
#include "mine.h"
#include "base.h"
#include "harvestor.h"
#include "conveyer.h"

namespace Ui {
class GameScene;
}

class GameScene : public QWidget
{
    Q_OBJECT

public:
    explicit GameScene(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void move_all_mineral();
    ~GameScene();

private:
    Ui::GameScene *ui;
    int GameTime;
    QTimer *timer0 = nullptr; //计时
    QTimer *timer1 = nullptr; //刷新屏幕
    QTimer *timer2 = nullptr;
    Block *block[20][30];
    void draw_block(QPainter &painter, Block *bl);
    bool Mineral_out(Block *bl = NULL, Facility *fac = NULL, Mineral *tmp = NULL);
};

#endif // GAMESCENE_H
