#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>

typedef enum {NON_EXISTING_USER, ALREADY_EXISTING_USER, WRONG_PASSWORD,
              QUERY_ERROR, CONNECTION_ERROR, NON_EXISTING_FILE,
              ALREADY_EXISTING_FILE, NO_FILES_AVAILABLE, SUCCESS} DatabaseError;
#define SHARE_LINK_LENGTH 30

class Database
{
public:
    Database();
    bool checkConnection();
    DatabaseError signup(const QString &username, const QString &password);
    DatabaseError login(const QString &username, const QString &password,
                        QString &nickname);
    DatabaseError updateNickname(const QString &username,
                                 const QString &nickname);
    DatabaseError updatePassword(const QString &username,
                                 const QString &oldpass,
                                 const QString &newpass);
    DatabaseError checkOldPassword(const QString &username,
                                   const QString &oldpass);
    DatabaseError getFiles(const QString &username,
                           QVector<QPair<QString,QString>> &files,
                           bool shared);
    DatabaseError newFile(const QString &username, const QString &filename,
                          QString &sharedLink);
    DatabaseError getSharedLink(const QString &author,const QString &password,
                                QString &sharedLink);
    DatabaseError getFilenameFromSharedLink(const QString& sharedLink,
                                            QString& filename,
                                            const QString& user);
private:
    QSqlDatabase db;
    QString generateRandomString() const;
};

#endif // DATABASE_H
