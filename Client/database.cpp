#include "database.h"
#include <QtSql>
#include <QDebug>
#include <QMessageBox>



//decidere dove creare la connessione...all'inizio dell'aertura della finestra principale oppure ogni volta pagina di login e signup
Database::Database()
{
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("editor");
    db.setUserName("root");
    bool ok = db.open();
    if (!ok)
        qDebug()<<"false";
}

int Database::signup(QString &username,QString &password){
    QSqlQuery qry;
    qry.prepare("INSERT INTO USER (Username, Nickname, Password,Icon) VALUES (:username, :nickname, :password, :icon)");
    qry.bindValue(":username",username);
    qry.bindValue(":nickname",username);
    qry.bindValue(":password",password); //va cifrata
    qry.bindValue(":icon","cane.png"); //scelta a caso tra quelle disponibili?


}

int Database::login(QString &username, QString &password){

}
