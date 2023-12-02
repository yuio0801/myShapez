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

private:
    Ui::Store *ui;
};

#endif // STORE_H
