#ifndef STORE_H
#define STORE_H

#include <QWidget>

namespace Ui {
class Store;
}

class Store : public QWidget
{
    Q_OBJECT

public:
    explicit Store(QWidget *parent = nullptr);
    ~Store();
    void update();
    void WriteLine(int toline, QString data);
private slots:
    void on_updatebt_clicked();

    void on_exitbt_clicked();

    void on_list_currentChanged(int index);

private:
    Ui::Store *ui;
    int cost;
};

#endif // STORE_H
