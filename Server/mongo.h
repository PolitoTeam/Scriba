#ifndef MONGO_H
#define MONGO_H

#include <QString>
#include <QPair>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "database.h"
//typedef enum {NON_EXISTING_USER, ALREADY_EXISTING_USER, WRONG_PASSWORD,
//			  QUERY_ERROR, CONNECTION_ERROR, NON_EXISTING_FILE,
//			  ALREADY_EXISTING_FILE, NO_FILES_AVAILABLE, SUCCESS} DatabaseError;
//#define SHARE_LINK_LENGTH 30

class Mongo
{
public:
	Mongo();
	void connect();
	bool checkConnection();

	bool insertNewFile(const QString filename, const QString username);
    void saveFile(const QString filename,const QJsonArray& symbols);
    bool retrieveFile(const QString filename, QJsonArray& symbols);

	DatabaseError signup(const QString& username, const QString& password);
    DatabaseError login(const QString& username, const QString password,
						QString& nickname);
	DatabaseError updateNickname(const QString& username,
								 const QString& nickname);
	DatabaseError updatePassword(const QString& username,
								 const QString& oldpass,
									 const QString& newpass);
	DatabaseError checkOldPassword(const QString& username,
								   const QString& oldpass);
	DatabaseError getFiles(const QString& username,
						   QVector<QPair<QString,QString>>& files,
							   bool shared);
	DatabaseError newFile(const QString& username, const QString& filename,
						  QString& sharedLink);
	DatabaseError getSharedLink(const QString& author,const QString& filename,
								QString& sharedLink);
	DatabaseError getFilenameFromSharedLink(const QString& sharedLink,
											QString& filename,
											const QString& user);

private:
	QString generateRandomString() const;

	// The mongocxx::instance constructor initialize the driver:
	// it must be created before using the driver and
	// must remain alive for as long as the driver is in use.
	mongocxx::instance inst{};
//	mongocxx::client conn{mongocxx::uri{"mongodb://localhost:27000"}};
	mongocxx::client conn{mongocxx::uri{}};
    mongocxx::database db;
};

#endif // MONGO_H
