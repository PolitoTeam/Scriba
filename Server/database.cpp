#include "database.h"
#include <QtSql>
#include <QDebug>
#include <QMessageBox>
#include <sodium/crypto_pwhash.h>

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
            char hashed_password[crypto_pwhash_STRBYTES];
            // conversion from QString to char *
            const char *password_char = password.toLocal8Bit().data();

            if (crypto_pwhash_str
                (hashed_password, password_char, strlen(password_char),
                 crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
                qDebug() << "Error while hashing...";
            }

            QString hashed_password_qstring = QString::fromUtf8(hashed_password);
            qry.prepare("INSERT INTO USER (Username, Nickname, Password,Icon) VALUES (:username, :nickname, :password, :icon)");
            qry.bindValue(":username",username);
            qry.bindValue(":nickname",username);
            qry.bindValue(":password", hashed_password_qstring);
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
    else if (!qry.next())
        //non esiste nessun utente con questo username
        err = NON_EXISTING_USER;
    else {
        const char *password_char = password.toLocal8Bit().data();
        QString hashed_password = qry.value(1).toString();
        const char *hashed_password_char = hashed_password.toLocal8Bit().data();
        if (crypto_pwhash_str_verify(hashed_password_char, password_char, strlen(password_char)) != 0) {
            err = WRONG_PASSWORD;
        }
    }

    db.close();
    return err;
}

DatabaseError Database::updateNickname(const QString &username,const QString &nickname){
    DatabaseError err = SUCCESS;
    if (!db.open())
        err = CONNECTION_ERROR;

    QSqlQuery qry;
    qry.prepare("SELECT Username FROM USER WHERE Username=:username");
    qry.bindValue(":username",username);
    if (!qry.exec())
        err = QUERY_ERROR;  //valutare se usare codici di errore o segnali
    else if (!qry.next())
        //non esiste nessun utente con questo username
        err = NON_EXISTING_USER;
    else {

        QSqlQuery qry;
        qry.prepare("UPDATE USER SET Nickname=:nickname WHERE Username=:username");
        qry.bindValue(":username",username);
        qry.bindValue(":nickname",nickname);
        if (!qry.exec()){
            err = QUERY_ERROR;
        }

    }

    db.close();
    return err;
}

DatabaseError  Database::updatePassword(const QString &username,const QString &oldpass,const QString &newpass){
    DatabaseError err = SUCCESS;
    if (!db.open())
        err = CONNECTION_ERROR;

    QSqlQuery qry;
    qry.prepare("SELECT Username,Password FROM USER WHERE Username=:username");
    qry.bindValue(":username",username);
    if (!qry.exec())
        err = QUERY_ERROR;  //valutare se usare codici di errore o segnali
    else if (!qry.next())
        //non esiste nessun utente con questo username
        err = NON_EXISTING_USER;
    else {
        const char *password_char = oldpass.toLocal8Bit().data();
        QString hashed_password = qry.value(1).toString();
        const char *hashed_password_char = hashed_password.toLocal8Bit().data();
        if (crypto_pwhash_str_verify(hashed_password_char, password_char, strlen(password_char)) != 0) {
            err = WRONG_PASSWORD;
        }
        QSqlQuery qry;
        qry.prepare("UPDATE USER SET Password=:newpassword WHERE Username=:username");
        qry.bindValue(":username",username);

        char hashed_newpassword[crypto_pwhash_STRBYTES];
        // conversion from QString to char *
        const char *password_newchar = newpass.toLocal8Bit().data();

        if (crypto_pwhash_str(hashed_newpassword, password_newchar, strlen(password_newchar),
             crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
            qDebug() << "Error while hashing...";
        }

        QString hashed_password_qstring = QString::fromUtf8(hashed_newpassword);
        qry.bindValue(":newpassword",hashed_password_qstring);

        if (!qry.exec()){
            err = QUERY_ERROR;
        }

    }

    db.close();
    return err;
}
