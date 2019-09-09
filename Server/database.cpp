#include "database.h"
#include <QtSql>
#include <QDebug>
#include <QMessageBox>



//decidere dove creare la connessione...all'inizio dell'aertura della finestra principale oppure ogni volta pagina di login e signup
Database::Database()
{
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("editor");
    db.setUserName("root");
    bool ok = db.open();
    if (!ok)
        qDebug()<<"false";
}

int Database::signup(const QString &username,const QString &password){
    QSqlQuery qry;
    qry.prepare("INSERT INTO USER (Username, Nickname, Password,Icon) VALUES (:username, :nickname, :password, :icon)");
    qry.bindValue(":username",username);
    qry.bindValue(":nickname",username);
    qry.bindValue(":password",password); //va cifrata
    qry.bindValue(":icon","cane.png"); //scelta a caso tra quelle disponibili?
    if (!qry.exec()){
        qDebug()<<"PROBlemi qui";
        return -1;
    }
    return 1;
}

int Database::login(const QString &username,const QString &password){
    QSqlQuery qry;
    qry.prepare("SELECT Username,Password FROM USER WHERE Username=:username");
    qry.bindValue(":username",username);
    if (!qry.exec())
        return -2;  //valutare se usare codici di errore o segnali
    if (!qry.next())
        //non esiste nessun utente con questo username
        return -1;

    if (QString::compare(password,qry.value(1).toString(),Qt::CaseSensitive)==0)
            return 1;
    return 0;
}
