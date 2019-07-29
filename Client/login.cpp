#include "login.h"
#include "ui_login.h"
#include <QtWidgets>

Login::Login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    home = new Home(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButtonLogin_clicked()
{
    QString username = ui->lineEditUsername->text();
    QString password = ui->lineEditPassword->text();

    if (username == "test" && password == "test") {
        QMessageBox::information(this, "Login", "Success.");
        this->hide();
        home->show();
    } else {
        QMessageBox::warning(this, "Login", "Not correct.");
    }
}
