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
    explicit Signup(QWidget *parent = nullptr,Client* client=0);
    void setClient(Client* client);
    ~Signup();

private slots:
    void on_pushButtonLogin_2_clicked();

    void on_pushButtonLogin_clicked();

    void on_lineEditUsername_editingFinished();

    void on_lineEditPassword_editingFinished();

    void on_lineEditPassword_3_editingFinished();

    void on_lineEditUsername_textChanged(const QString &arg1);

    void on_lineEditPassword_textChanged(const QString &arg1);

    void on_lineEditPassword_3_textChanged(const QString &arg1);

signals:
    void action(int i);

private:
    Ui::Signup *ui;
    bool valid;
    Client* client;

    bool checkUsername(const QString &username);
    void checkPassword(const QString &password);
    bool checkConfirmation(const QString &pass,const QString &conf);
};

#endif // SIGNUP_H
