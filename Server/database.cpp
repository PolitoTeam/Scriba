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
}

bool Database::checkConnection() {
    if (!db.open()) {
        qDebug() << db.lastError();
        return false;
    } else {
        db.close();
        return true;
    }
}

DatabaseError Database::signup(const QString &username,const QString &password){
    DatabaseError err = SUCCESS;
    if (!db.open())
        err = CONNECTION_ERROR;

    QSqlDatabase::database().transaction();
    QSqlQuery qry;
    qry.prepare("SELECT Username "
                "FROM USER WHERE "
                "Username = :username FOR UPDATE");
    qry.bindValue(":username", username);
    if (qry.exec()) {
        if (!qry.next()) {
            qDebug() << "AAA";
            qry.prepare("INSERT INTO USER (Username, Nickname, Password,Icon) VALUES (:username, :nickname, :password, :icon)");
            qry.bindValue(":username",username);
            qry.bindValue(":nickname",username);
            qry.bindValue(":password",password); //va cifrata
            qry.bindValue(":icon","cane.png"); //scelta a caso tra quelle disponibili?
            if (!qry.exec()){
                err = QUERY_ERROR;
            }
        } else {
            err = ALREADY_EXISTING_USER;
        }
    } else {
        err = QUERY_ERROR;
    }
    QSqlDatabase::database().commit();

    db.close();
    qDebug().nospace() << "Error " << err;
    return err;
}

DatabaseError Database::login(const QString &username,const QString &password){
    DatabaseError err = SUCCESS;
    if (!db.open())
        err = CONNECTION_ERROR;

    QSqlQuery qry;
    qry.prepare("SELECT Username,Password FROM USER WHERE Username=:username");
    qry.bindValue(":username",username);
    if (!qry.exec())
        err = QUERY_ERROR;  //valutare se usare codici di errore o segnali
    if (!qry.next())
        //non esiste nessun utente con questo username
        err = NON_EXISTING_USER;

    if (QString::compare(password,qry.value(1).toString(),Qt::CaseSensitive) != 0)
        err = WRONG_PASSWORD;

    db.close();
    return err;
}
