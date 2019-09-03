#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include "home.h"
#include "client.h"

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

//    void on_pushButtonLogin_2_clicked();

    void on_lineEditUsername_editingFinished();

    void on_lineEditUsername_textChanged(const QString &arg1);

    void on_lineEditPassword_textChanged(const QString &arg1);

    void connectedToServer();
    void attemptLogin(const QString &username, const QString &password);
    void loggedIn();
//    void loginFailed(const QString &reason);

signals:
    void access(int i);

private:
    Ui::Login *ui;
    Client *client;
};

#endif // LOGIN_H
