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

typedef enum {NON_EXISTING_USER, ALREADY_EXISTING_USER, WRONG_PASSWORD,
			  QUERY_ERROR, CONNECTION_ERROR, NON_EXISTING_FILE,
			  ALREADY_EXISTING_FILE, NO_FILES_AVAILABLE,
			  CRYPTO_ERROR, SUCCESS} DatabaseError;
#define SHARE_LINK_LENGTH 30

class Mongo
{
public:
	Mongo();
	void connect();
	bool checkConnection();

	bool insertNewFile(const QString& filename);
	bool saveFile(const QString filename, QByteArray symbols);
    bool retrieveFile(const QString filename, QVector<QJsonObject>& symbols);
	void cleanBucket();

	void upsertImage(QString email, const QByteArray& image);
	QByteArray retrieveImage(QString email, bool& found);

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
    DatabaseError checkAlreadyExistingUsername(const QString &username);

private:
	// The mongocxx::instance constructor initialize the driver:
	// it must be created before using the driver and
	// must remain alive for as long as the driver is in use.
	mongocxx::instance inst{};

#ifdef DOCKER
	mongocxx::client conn{mongocxx::uri{"mongodb://shared_editor_db:27017"}};
#else
	mongocxx::client conn{mongocxx::uri{}};
#endif

    mongocxx::database db;
	mongocxx::gridfs::bucket bucket = conn["gridfs"].gridfs_bucket();
	mongocxx::database bucket_db = conn["gridfs"];

	QString generateRandomString() const;
	bsoncxx::types::b_binary fromQByteArrayToBSON(const QByteArray& image);
	bsoncxx::types::value getObjectID(const QString& filename, bool& found);
};

#endif // MONGO_H
