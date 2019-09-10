#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include "client.h"

namespace Ui {
class Signup;
}

class Signup : public QWidget
{
    Q_OBJECT

public:
    Signup(QWidget *parent = nullptr,Client* client=0);
    ~Signup();
    void enableAllButtons();

private slots:
    void on_pushButtonClear_clicked();

    void on_pushButtonSignup_clicked();

    void on_lineEditUsername_editingFinished();

    void on_lineEditPassword_editingFinished();

    void on_lineEditConfirmPassword_editingFinished();

    void on_lineEditUsername_textChanged(const QString &arg1);

    void on_lineEditPassword_textChanged(const QString &arg1);

    void on_lineEditConfirmPassword_textChanged(const QString &arg1);

    void on_pushButtonBackLogin_clicked();

    void signedUp();
    void signupFailed(const QString &reason);

signals:
    void action(int i);

private:
    Ui::Signup *ui;
    bool valid;
    Client* client;

    bool checkUsername(const QString &username);
    void checkPassword(const QString &password);
    bool checkConfirmation(const QString &pass,const QString &conf);
    void clearLabel();
    void clearLineEdit();
};

#endif // SIGNUP_H
