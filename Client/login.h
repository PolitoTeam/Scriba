#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "home.h"

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

private slots:
    void on_pushButtonLogin_clicked();
    void move_to_login();
    void move_to_editor();
    void show_home();
//    public slots:
//    void move_to_home();

private:
    Ui::Login *ui;
    Home *home;
};

#endif // LOGIN_H
