#include "login.h"
#include "ui_login.h"
#include <QtWidgets>

Login::Login(QWidget *parent) :
    QWidget (parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    home = new Home(this);

    ui->stackedWidget->addWidget(home);
    connect(home, &Home::move_to_login_clicked, this, &Login::move_to_login);
    connect(home, &Home::move_to_editor_clicked, this, &Login::move_to_editor);
    connect(home, &Home::move_to_home_clicked, this, &Login::show_home);
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
//        this->hide();
//        home->show();
        ui->stackedWidget->setCurrentWidget(home);
    } else {
        QMessageBox::warning(this, "Login", "Not correct.");
    }
}

void Login::move_to_login()
{
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
}

void Login::move_to_editor()
{
    this->hide();
}

void Login::show_home()
{
    this->show();
}
