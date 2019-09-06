#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>


class Database
{
public:
    Database();
    int signup(const QString &username,const QString &password);
    int login(const QString &username,const QString &password);

private:
    QSqlDatabase db;

};

#endif // DATABASE_H
