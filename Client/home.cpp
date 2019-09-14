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
    emit changeWidget(LOGIN);
}

void Home::on_pushButtonNewFile_clicked()
{
    emit changeWidget(EDITOR);
}

void Home::on_pushButtonModify_clicked()
{
    emit modify();
    emit changeWidget(MODIFY);
}

void Home::on_pushButtonSharedLink_clicked()
{
    //client->sendProfileImage();
}

