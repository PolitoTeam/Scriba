#include "login.h"
#include "ui_login.h"
#include <QtWidgets>
#include <QPixmap>
#include <QRegularExpression>
#include <QHostAddress>

Login::Login(QWidget *parent) :
    QWidget (parent),
    ui(new Ui::Login),
    client(new Client(this))
{
    ui->setupUi(this);
    QPixmap pix(":/images/bold.png");
    int w=ui->label->width();
    int h=ui->label->height();
    ui->label->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

    connect(client, &Client::connected, this, &Login::connectedToServer);
    connect(client, &Client::loggedIn, this, &Login::loggedIn);
//    connect(client, &Client::loginError, this, &Login::loginFailed);
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButtonLogin_clicked()
{
    QString username = ui->lineEditUsername->text();
    QString password = ui->lineEditPassword->text();

//    if (username == "test" && password == "test") {
////        QMessageBox::information(this, "Login", "Success.");
//        emit access(2);
//    } else {
//        ui->label_3->setText("Username/Password errati!");
//    }

    // disable the connect button to prevent the user clicking it again
    ui->pushButtonLogin->setEnabled(false);
    // tell the client to connect to the host using the port 1967
    client->connectToServer(QHostAddress::Any, 1500);
    client->login(username, password);
}

void Login::connectedToServer()
{
    const QString username = ui->lineEditUsername->text();
    const QString password = ui->lineEditPassword->text();
    qDebug() << username << " " << password;
    attemptLogin(username, password);
}

void Login::attemptLogin(const QString &username, const QString &password)
{
    // use the client to attempt a log in with the given username
    client->login(username, password);
}

void Login::loggedIn()
{
    ui->pushButtonLogin->setEnabled(true);
    emit access(2);

}

//void Login::on_pushButtonLogin_2_clicked()
//{
//    ui->lineEditUsername->clear();
//    ui->lineEditPassword->clear();
//    emit access(1);
//}

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

