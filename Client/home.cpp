#include "home.h"
#include "ui_home.h"
#include "client.h"
#include "CRDT.h"
#include <QInputDialog>
#include <QDir>
#include <QMessageBox>

Home::Home(QWidget *parent,Client* client) :
    QWidget(parent),
    ui(new Ui::Home),
    client(client)
{
    ui->setupUi(this);
    connect(client, &Client::filesReceived, this, &Home::showActiveFiles);
    connect(client, &Client::openFilesError, this, &Home::on_openFilesError);
    connect(client, &Client::correctNewFile, this, &Home::newFileCompleted);
    connect(client, &Client::wrongNewFile, this, &Home::newFileError);
    connect(client, &Client::correctOpenedFile, this, &Home::openFileCompleted);
    connect(client, &Client::wrongListFiles, this, &Home::on_openFilesError);
    connect(client, &Client::wrongSharedLink, this, &Home::on_openFilesError);
    connect(this, &Home::fileChosen,client,&Client::openFile);
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
    bool ok;
    QString text = QInputDialog::getText(this, tr("New File"),
                                         tr("File name:"), QLineEdit::Normal,
                                         tr(""), &ok);

    if (ok && !text.isEmpty()) {
        // to update the window title in the editor
        client->setOpenedFile(text + "," + client->getUsername());

        client->createNewFile(text);
    }
}

void Home::newFileCompleted() {
    emit addCRDTterminator();
    emit changeWidget(EDITOR);
}

void Home::openFileCompleted() {
    emit changeWidget(EDITOR);
}

void Home::newFileError(const QString& reason) {
    QMessageBox::critical(this, tr("Error"),
                          reason, QMessageBox::Close);
    on_pushButtonNewFile_clicked();
}

void Home::on_openFilesError(const QString& reason) {
    QMessageBox::critical(this, tr("Error"), reason, QMessageBox::Close);
}

void Home::on_pushButtonModify_clicked()
{
    emit modify();
    emit changeWidget(MODIFY);
}

void Home::on_pushButtonSharedLink_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Shared Link"),
                                                 tr("Paste link here:"), QLineEdit::Normal,
                                                 tr(""), &ok);
    if (ok && !text.isEmpty()) {
        qDebug().nospace() << "Line read: " << text;
        // TODO: same as open file
        client->getFilenameFromLink(text);
    }
}


void Home::on_pushButtonOpenFile_clicked()
{

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
    QList<QPair<QString,QString>> map_files = client->getActiveFiles();

    for (auto i = map_files.begin(); i != map_files.end(); ++i){
        items << QString(i->first);
    }
    items.sort();

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Open File"),
                                         tr("Files:"), items, 0, false, &ok);
    if (ok && !item.isEmpty()) {
        QString choice = item + "," + client->getUsername();
        qDebug() << "File chosen:" << choice << endl;
        emit fileChosen(choice);
    }
}

