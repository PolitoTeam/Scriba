#include "database.h"
#include <QtSql>
#include <QDebug>
#include <QMessageBox>
#include <sodium.h>

Database::Database()
{
    srand(time(NULL));

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
            qry.prepare("INSERT INTO USER (Username, Nickname, Password) VALUES (:username, :nickname, :password)");
            qry.bindValue(":username",username);
            qry.bindValue(":nickname",username);
            qry.bindValue(":password", hashed_password_qstring);

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

DatabaseError Database::login(const QString &username,const QString &password,QString &nickname){
    DatabaseError err = SUCCESS;
    if (!db.open())
        err = CONNECTION_ERROR;

    QSqlQuery qry;
    qry.prepare("SELECT Username,Password,Nickname FROM USER WHERE Username=:username");
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
        else{
            nickname.append(qry.value(2).toString());
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

DatabaseError Database::checkOldPassword(const QString &username, const QString &oldpass)
{
    DatabaseError err = SUCCESS;
    if (!db.open())
        err = CONNECTION_ERROR;

    QSqlQuery qry;
    qry.prepare("SELECT Password FROM USER WHERE Username=:username");
    qry.bindValue(":username", username);
    if (!qry.exec())
        err = QUERY_ERROR;
    else if (!qry.next())
        err = NON_EXISTING_USER;
    else {
        const char *old_password_char = oldpass.toLocal8Bit().data();
        QString hashed_old_password = qry.value(0).toString();
        const char *hashed_password_char = hashed_old_password.toLocal8Bit().data();
        if (crypto_pwhash_str_verify(hashed_password_char, old_password_char, strlen(old_password_char)) != 0) {
            err = WRONG_PASSWORD;
        }
    }

    db.close();
    return err;
}

DatabaseError Database::getFiles(const QString &username, QMap<QString,QString> &files){
    DatabaseError err = SUCCESS;
    if (!db.open())
        err = CONNECTION_ERROR;

    //PER PROVARE
    QString ciao("ciao");
    QString hello("hello");
    files.insert(ciao,"Giuseppe");
    files.insert(hello,"Giuseppe");


    QSqlQuery qry;
    qry.prepare("SELECT Name,Owner FROM FILE, FILE_USER WHERE FILE.Link=FILE_USER.Link and User=:Username and First_access=false");
    qry.bindValue(":username", username);
    if (!qry.exec())
        err = QUERY_ERROR;

    else if (!qry.next())
        err = NON_EXISTING_USER;
    else {
        while (qry.next()) {
            files.insert(qry.value(0).toString(),qry.value(1).toString());
        }
    }

    db.close();
    return err;
}

DatabaseError Database::newFile(const QString &username, const QString &filename)
{
    DatabaseError err = SUCCESS;
    if (!db.open())
        err = CONNECTION_ERROR;

    QSqlQuery qry;
    qry.prepare("SELECT Name FROM FILE WHERE Name=:filename AND Owner=:username");
    qry.bindValue(":filename", filename);
    qry.bindValue(":username", username);

    if (!qry.exec())
        err = QUERY_ERROR;
    else if (qry.next())
        err = ALREADY_EXISTING_FILE;
    else {
        QString link;
        bool alreadyExisitingLink = true;
        while (alreadyExisitingLink) {
            link = generateRandomString();

            QSqlQuery qry;
            qry.prepare("SELECT * FROM FILE WHERE Link=:link");
            qry.bindValue(":link", link);

            if (!qry.next())
                alreadyExisitingLink = false;
        }

        QSqlQuery qry;
        qry.prepare("INSERT INTO FILE (Link, Name, Owner, Public) "
                    "VALUES (:link, :filename, :username, TRUE)");
        qry.bindValue(":link", link);
        qry.bindValue(":filename", filename);
        qry.bindValue(":username", username);
        if (!qry.exec()){
            err = QUERY_ERROR;
        }
    }

    db.close();
    return err;
}

QString Database::generateRandomString() const
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

   QString randomString;
   for(int i = 0; i < SHARE_LINK_LENGTH; i++) {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}
