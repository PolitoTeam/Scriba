#include "home.h"
#include "ui_home.h"
#include "client.h"
#include "CRDT.h"
#include <QInputDialog>
#include <QDir>

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
    bool ok;
    QString text = QInputDialog::getText(this, tr("Shared Link"),
                                                 tr("Paste link here:"), QLineEdit::Normal,
                                                 tr(""), &ok);
    if (ok && !text.isEmpty()) {
        qDebug().nospace() << "Line read: " << text;
        changeWidget(EDITOR);
        //        emit openFromLink(text);
    }
}


void Home::on_pushButtonOpenFile_clicked()
{
    // MOMENTARILY used for tests
//    CRDT *crdt = new CRDT(0, client);
//    crdt->localInsert(0, 'T');
}
