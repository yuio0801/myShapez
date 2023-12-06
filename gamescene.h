#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include <QDebug>
#include "block.h"
#include "mine.h"
#include "base.h"
#include "harvestor.h"
#include "conveyer.h"
#include "dustbin.h"
#include "cutter.h"

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


    ~GameScene();

private:
    Ui::GameScene *ui;
    int GameTime;
    QTimer *timer0 = nullptr; //计时
    QTimer *timer1 = nullptr; //Mine
    QTimer *timer2 = nullptr; //Conveyer
    QTimer *timer3 = nullptr; //Base
    QTimer *timer4 = nullptr; //刷新
    void draw_block(QPainter &painter, Block *bl);
    void create_mineral();
    void move_mineral();
    void settle_mode();
    bool Load();
    void Init();
};

#endif // GAMESCENE_H
