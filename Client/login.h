#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include "appMainWindow.h"
#include "client.h"

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr,Client* client = nullptr);
    ~Login();
    void enableAllButtons();

private slots:
    void try_to_log();
    void on_pushButtonNewAccount_clicked();
    void loggedIn();
    void loginFailed(const QString &reason);
//    void messageReceived(const QString &sender, const QString &text);
//    void sendMessage();
//    void disconnectedFromServer();
//    void userJoined(const QString &username);
//    void userLeft(const QString &username);
    void on_lineEditUsername_textChanged(const QString &arg);

public slots:
    void disconnect();

signals:
    void changeWidget(int i);

private:
    Ui::Login *ui;
    Client *client;
    void clearLabel();
    void clearLineEdit();
};

#endif // LOGIN_H
