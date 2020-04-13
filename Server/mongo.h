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

class Mongo
{
public:
	Mongo();
	void connect();
	bool checkConnection();
	bool insertNewFile(const QString filename, const QString username);
    void saveFile(const QString filename,const QJsonArray& symbols);
    bool retrieveFile(const QString filename, QJsonArray& symbols);

private:
	mongocxx::client conn{mongocxx::uri{}};
    mongocxx::database db;
};

#endif // MONGO_H
