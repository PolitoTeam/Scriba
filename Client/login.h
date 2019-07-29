#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include "home.h"

namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

private slots:
    void on_pushButtonLogin_clicked();

private:
    Ui::Login *ui;
    Home *home;
};

#endif // LOGIN_H
