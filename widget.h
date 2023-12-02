#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "store.h"
#include "gamescene.h"
#include <string>
#include <fstream>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
    Store* mystore;
    GameScene* scene;
    QString filePath;
    std::ifstream savedata;

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:


    void on_newgamebtn_clicked();

    void on_loadbtn_clicked();

    void on_storebtn_clicked();

    void on_exitbtn_clicked();

private:
    Ui::Widget *ui;
    bool load_data();
};
#endif // WIDGET_H
