#include "home.h"
#include "ui_home.h"
#include "client.h"

Home::Home(QWidget *parent,Client* client) :
    QWidget(parent),
    ui(new Ui::Home),
    client(client)
{
    ui->setupUi(this);
}

Home::~Home()
{
    delete ui;
}

void Home::setClient(Client *client){
    this->client=client;
}

void Home::on_pushButtonLogOut_clicked()
{
    emit logOut();
    emit action(0);
}

void Home::on_pushButtonNewFile_clicked()
{
    //this->hide();
    //editor->show();
    emit action(3);
}

void Home::on_pushButtonModify_clicked()
{
    emit modify();
    emit action(4);
}



void Home::on_pushButton_4_clicked()
{
    client->send();
}
