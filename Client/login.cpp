#include "login.h"
#include "ui_login.h"
#include <QtWidgets>
#include <QPixmap>
#include <QRegularExpression>

Login::Login(QWidget *parent) :
    QWidget (parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    QPixmap pix(":/images/bold.png");
    int w=ui->label->width();
    int h=ui->label->height();
    ui->label->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
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
//        QMessageBox::information(this, "Login", "Success.");
        emit access(2);
    } else {
        ui->label_3->setText("Username/Password errati!");
    }
}

void Login::on_pushButtonLogin_2_clicked()
{
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
    emit access(1);
}

void Login::on_lineEditUsername_editingFinished()
{

    QString username = ui->lineEditUsername->text();
    QRegularExpression re("^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$");
    QRegularExpressionMatch match = re.match(username);
    bool hasMatch = match.hasMatch();

    if (!hasMatch)
        ui->label_3->setText("Username errato!");

}

void Login::on_lineEditUsername_textChanged(const QString &arg1)
{
    ui->label_3->clear();
}



void Login::on_lineEditPassword_textChanged(const QString &arg1)
{
    ui->label_3->clear();
}

