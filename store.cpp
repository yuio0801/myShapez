#include "store.h"
#include "ui_store.h"
#include <QFile>
#include<fstream>
#include <iostream>
#include <string>
using namespace std;
extern bool ifload;
extern QString filePath;
extern int earned;


//当前地图最大长宽
extern int Maxx, Maxy;
extern int mineral_value[5];

extern int num_mine[2];
extern int num_base;
extern bool isnew;
Store::Store(QWidget *parent) :
    QWidget(parent),

    ui(new Ui::Store)
{
    cost = 0;
    ui->setupUi(this);
    ui->cost->setText("cost:0");
    ui->exitbt->setText("Exit");
    ui->updatebt->setText("Update");
    ui->updatebt->setEnabled(false);
}

Store::~Store()
{
    qDebug()<<"商店析构";
    delete ui;
}

void Store::WriteLine(int toline, QString data)
{
    ifstream savedata;
    if(filePath.isEmpty())
    {
        filePath = "C:\\Users\\ouyan\\Desktop\\myShapez\\savedata.txt";
    }
    savedata.open(filePath.toStdString());

    string strFileData = "";
    int line = 0;
    char tmpLineData[1024] = {0};
    while(savedata.getline(tmpLineData, sizeof(tmpLineData)))
    {
        if (line == toline)
        {
            strFileData += data.toStdString();
            strFileData += "\n";
        }
        else
        {
            strFileData += string(tmpLineData);
            strFileData += "\n";
        }
        line++;
    }
    savedata.close();
    ofstream out;
    if(filePath.isEmpty())
    {
        filePath = "C:\\Users\\ouyan\\Desktop\\myShapez\\savedata.txt";
    }
    out.open(filePath.toStdString());
    out.flush();
    out<<strFileData;
    out.close();
}

void Store::on_updatebt_clicked()
{
    switch(ui->list->currentIndex())
    {
        case 0:
        if(num_base == 2)
        {
            WriteLine(4, "3");
            earned -= cost;
            ui->money->setText("money:"+QString::number(earned));
            ui->basesize->setText("");
            num_base = 3;
        }
        break;
        case 1:
        if(Maxx == 10 && Maxy == 10)
        {
            WriteLine(1, "20 20");
            earned -= cost;
            ui->money->setText("money:"+QString::number(earned));
            ui->mapsize->setText("");
            Maxx = Maxy = 20;
        }
            break;
        case 2:
            if(num_mine[0]==3&&num_mine[1]==3)
            {
                WriteLine(3, "4 4");
                earned -= cost;
                ui->money->setText("money:"+QString::number(earned));
                ui->mine0->setText("");
                ui->mine1->setText("");
                num_mine[0] = num_mine[1]=4;
            }
            break;
        case 3:
            if(mineral_value[0] == 20 && mineral_value[1] == 10 && mineral_value[2] == 50 && mineral_value[3] == 30 && mineral_value[4] == 50){
            WriteLine(2, "30 20 60 40 60");
            earned -= cost;
            ui->money->setText("money:"+QString::number(earned));

            ui->money0->setText("");
            ui->money1->setText("");
            ui->money2->setText("");
            ui->money3->setText("");
            ui->money4->setText("");
            for(int i=0;i<5;++i)
            {
                mineral_value[i]+=10;
            }
            }
            break;
        }
        ui->updatebt->setEnabled(false);
}


void Store::on_exitbt_clicked()
{
        WriteLine(5, QString::number(earned));
    this->hide();
}


void Store::on_list_currentChanged(int index)
{
    ui->money->setText("money:"+QString::number(earned));
    switch(index)
    {
        case 0:
        if(num_base == 2)
        {
            ui->basesize->setText(QString::number(num_base) + "*" + QString::number(num_base) +" -> "+ QString::number(num_base+1)+ "*" + QString::number(num_base+1));
            cost = 1000;
            ui->cost->setText("cost:1000");
            if(cost > earned)
            {
                ui->updatebt->setEnabled(false);
            }
            else
            {
                ui->updatebt->setEnabled(true);
            }
        }
            break;
        case 1:
        if(Maxx == 10 && Maxy == 10)
            {
            ui->mapsize->setText("10 * 10 -> "+QString::number(Maxx + 10) +" * "+QString::number(Maxy + 10));
            cost = 1000;
            ui->cost->setText("cost:1000");
            if(cost > earned)
            {
                ui->updatebt->setEnabled(false);
            }
            else
            {
                ui->updatebt->setEnabled(true);
            }
            }
            break;
        case 2:
            if(num_mine[0] == 3 && num_mine[1] == 3)
            {
            ui->mine0->setText("3 -> "+QString::number(num_mine[0]+2));
            ui->mine1->setText("3 -> "+QString::number(num_mine[1]+2));
            }
            cost = 2000;
            ui->cost->setText("cost:2000");
            if(cost > earned)
            {
            ui->updatebt->setEnabled(false);
            }
            else
            {
            ui->updatebt->setEnabled(true);
            }
            break;
        case 3:
            if(mineral_value[0] == 20 && mineral_value[1] == 10 && mineral_value[2] == 50 && mineral_value[3] == 30 && mineral_value[4] == 50){
            ui->money0->setText("20 -> 30");
            ui->money1->setText("10 -> 20");
            ui->money2->setText("50 -> 60");
            ui->money3->setText("30 -> 40");
            ui->money4->setText("50 -> 60");
            cost = 10000;
            ui->cost->setText("cost:10000");
            if(cost > earned)
            {
            ui->updatebt->setEnabled(false);
            }
            else
            {
            ui->updatebt->setEnabled(true);
            }
            }
            break;
    }
}

