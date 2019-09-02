#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>


class Database
{
public:
    Database();
    int signup(QString &username,QString &password);
    int login(QString &username,QString &password);

private:
    QSqlDatabase db;

};

#endif // DATABASE_H
