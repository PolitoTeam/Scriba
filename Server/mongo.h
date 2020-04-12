#ifndef MONGO_H
#define MONGO_H
#include <QString>
#include <QPair>

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
	bool checkConnection();
	DatabaseError signup(const QString& username, const QString& password);
	DatabaseError login(const QString& username, const QString& password,
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
	DatabaseError getSharedLink(const QString& author,const QString& password,
								QString& sharedLink);
	DatabaseError getFilenameFromSharedLink(const QString& sharedLink,
											QString& filename,
											const QString& user);

private:
	QString generateRandomString() const;
	mongocxx::instance inst{};
	mongocxx::client conn{mongocxx::uri{}};
};

#endif // MONGO_H
