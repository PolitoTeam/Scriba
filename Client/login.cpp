#include "login.h"
#include "ui_login.h"
#include <QtWidgets>
#include <QPixmap>
#include <QRegularExpression>
#include <QHostAddress>

Login::Login(QWidget *parent,Client* client) :
    QWidget (parent),
    ui(new Ui::Login),
    client(client)
{
    ui->setupUi(this);
    QPixmap pix(":/images/bold.png");
    int w=ui->label->width();
    int h=ui->label->height();
    ui->label->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

    connect(client, &Client::loggedIn, this, &Login::loggedIn);
    connect(client, &Client::loginError, this, &Login::loginFailed);
//    connect(client, &Client::messageReceived, this, &Login::messageReceived);
//    connect(client, &Client::disconnected, this, &Login::disconnectedFromServer);
//    connect(client, &Client::userJoined, this, &Login::userJoined);
//    connect(client, &Client::userLeft, this, &Login::userLeft);

    // try to login by clicking login button or pressing 'enter'
    connect(ui->pushButtonLogin, &QPushButton::clicked, this, &Login::try_to_log);
    connect(ui->lineEditPassword, &QLineEdit::returnPressed, this, &Login::try_to_log);

    // TODO: remove this line, used for DEBUG, allowing automatic login
//    ui->lineEditUsername->setText("enrico@gmail.com");
//    ui->lineEditPassword->setText("Admin11.");
//    try_to_log();
}

Login::~Login()
{
    delete ui;
}

void Login::enableAllButtons()
{
    ui->pushButtonLogin->setEnabled(true);
    ui->pushButtonNewAccount->setEnabled(true);
}

void Login::try_to_log()
{
    QString username = ui->lineEditUsername->text();
    QString password = ui->lineEditPassword->text();

    // disable the connect button to prevent the user clicking it again
    ui->pushButtonLogin->setEnabled(false);
    ui->pushButtonNewAccount->setEnabled(false);
    client->login(username, password);
}

void Login::loggedIn()
{
    qDebug() << "Login succeeded.";
    ui->pushButtonLogin->setEnabled(true);
    ui->pushButtonNewAccount->setEnabled(true);
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
   // client->getProfile()->load();  caricare l'immagine scaricata dal server


    emit changeWidget(HOME);
}

void Login::loginFailed(const QString &reason)
{
    qDebug() << "Login failed.";
    ui->pushButtonLogin->setEnabled(true);
    ui->pushButtonNewAccount->setEnabled(true);
    ui->labelMessage->setText(reason);

    client->disconnectFromHost();
}

void Login::clearLabel(){
    ui->labelMessage->clear();
}

void Login::clearLineEdit(){
    ui->lineEditPassword->clear();
    ui->lineEditUsername->clear();
}

void Login::on_pushButtonNewAccount_clicked()

{
    this->clearLabel();
    this->clearLineEdit();
    emit changeWidget(SIGNUP);
}

void Login::disconnect() {
    qDebug() << "Logging out.";
    client->disconnectFromHost();
}

void Login::on_lineEditUsername_textChanged(const QString&)
{
    this->clearLabel();
}
