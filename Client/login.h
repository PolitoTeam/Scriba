#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include "client.h"

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    Login(QWidget *parent = 0,Client* client=0);
    ~Login();
    void enableAllButtons();

private slots:
    void on_pushButtonLogin_clicked();

    void on_pushButtonNewAccount_clicked();

    void loggedIn();
    void loginFailed(const QString &reason);
//    void messageReceived(const QString &sender, const QString &text);
//    void sendMessage();
//    void disconnectedFromServer();
//    void userJoined(const QString &username);
//    void userLeft(const QString &username);

    void on_lineEditUsername_textChanged(const QString &arg1);

public slots:
    void disconnect();

signals:
    void access(int i);

private:
    Ui::Login *ui;
    Client *client;
    void clearLabel();
    void clearLineEdit();
};

#endif // LOGIN_H
