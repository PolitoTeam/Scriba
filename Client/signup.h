#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>

namespace Ui {
class Signup;
}

class Signup : public QWidget
{
    Q_OBJECT

public:
    explicit Signup(QWidget *parent = nullptr);
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
};

#endif // SIGNUP_H
