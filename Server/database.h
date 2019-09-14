#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>

typedef enum {NON_EXISTING_USER, ALREADY_EXISTING_USER, WRONG_PASSWORD, QUERY_ERROR, CONNECTION_ERROR, SUCCESS} DatabaseError;

class Database
{
public:
    Database();
    bool checkConnection();
    DatabaseError signup(const QString &username,const QString &password);
    DatabaseError login(const QString &username,const QString &password,QString &nickname);
    DatabaseError updateNickname(const QString &username,const QString &nickname); //da implementare
    DatabaseError updatePassword(const QString &username,const QString &oldpass,const QString &newpass);
private:
    QSqlDatabase db;

};

#endif // DATABASE_H
