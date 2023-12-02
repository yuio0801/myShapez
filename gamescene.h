#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QWidget>
#include <QDebug>
#include <block.h>



//int Maxx = 300, Maxy = 300;

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
    QTimer *timer1 = nullptr; //刷新屏幕
    Block *block[20][20];
    void draw_block(QPainter &painter, Block *bl);

};

#endif // GAMESCENE_H
