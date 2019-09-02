#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
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

    void on_pushButtonLogin_2_clicked();

    void on_lineEditUsername_editingFinished();

    void on_lineEditUsername_textChanged(const QString &arg1);


    void on_lineEditPassword_textChanged(const QString &arg1);

signals:
    void access(int i);

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
