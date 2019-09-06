#include "home.h"
#include "ui_home.h"
#include "client.h"

Home::Home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home)
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
