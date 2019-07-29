#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include "editor.h"

namespace Ui {
class Home;
}

class Home : public QMainWindow
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = 0);
    ~Home();

private slots:
     void on_pushButtonLogOut_clicked();
     void on_pushButtonNewFile_clicked();

private:
    Ui::Home *ui;
    Editor *editor;
};

#endif // HOME_H
