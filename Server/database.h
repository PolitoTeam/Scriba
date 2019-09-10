#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>

typedef enum {NON_EXISTING_USER, WRONG_PASSWORD, DB_QUERY_ERROR, DB_CONNECTION_ERROR, SUCCESS} DatabaseError;

class Database
{
public:
    Database();
    bool checkConnection();
    DatabaseError signup(const QString &username,const QString &password);
    DatabaseError login(const QString &username,const QString &password);

private:
    QSqlDatabase db;

};

#endif // DATABASE_H
