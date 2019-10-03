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
    connect(client,&Client::filesReceived,this,&Home::showActiveFiles);

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
<<<<<<< HEAD
    //manda richiesta al server di ricevere la lista di file
    client->getFiles();
    //disabilitare tutto?

    //2- apri una dialogbox per permettere all'utente di selezionare quale file
   /* QStringList items;
       items << tr("Spring") << tr("Summer") << tr("Fall") << tr("Winter");

       bool ok;
       QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
                                            tr("Season:"), items, 0, false, &ok);
       if (ok && !item.isEmpty())
           itemLabel->setText(item); */

    //3- manda la richiesta al server per quel file
    //4- una volta che hai ricevuto il file:
            //aggiorna in "background" il texteditor"
            //cambia pagina sul texteditor
}

void Home::showActiveFiles(){
    QStringList items;
    QMap<QString,QString> map_files = client->getActiveFiles();
    QMap<QString, QString>::iterator i;
    for (i = map_files.begin(); i != map_files.end(); ++i){
        //gestire poi meglio le due informazioni per la visualizzazione: nome del file e owner
        items<<QString(i.key()+","+i.value());
    }

    bool ok;
    QString item = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
                                                tr("Files:"), items, 0, false, &ok);
     if (ok && !item.isEmpty())
         qDebug()<<"File scelto: "<<item<<endl;

}

=======
    // MOMENTARILY used for tests
//    CRDT *crdt = new CRDT(0, client);
//    crdt->localInsert(0, 'T');
}
>>>>>>> 7384c5abf31ef81d923f525692df82955af25405
