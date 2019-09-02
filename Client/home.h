#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include "editor.h"

namespace Ui {
class Home;
}

class Home : public QWidget
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = 0);
    ~Home();

private slots:
     void on_pushButtonLogOut_clicked();
     void on_pushButtonNewFile_clicked();

signals:
     void action(int);

private:
    Ui::Home *ui;

};

#endif // HOME_H
