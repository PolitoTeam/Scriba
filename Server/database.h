#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>

enum Error {NON_EXISTING_USER, WRONG_PASSWORD, DB_QUERY_ERROR, SUCCESS};

class Database
{
public:
    Database();
    enum Error signup(const QString &username,const QString &password);
    enum Error login(const QString &username,const QString &password);

private:
    QSqlDatabase db;

};

#endif // DATABASE_H
