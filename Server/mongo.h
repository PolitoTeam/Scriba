#ifndef MONGO_H
#define MONGO_H
#include <QString>
#include <QPair>
#include <QJsonArray>
#include <QJsonDocument>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

class Mongo
{
public:
	Mongo();
	bool insertNewFile(const QString filename, const QString username);

    void saveFile(const QString filename,const QJsonArray& symbols);
	void retrieveFile(QJsonArray& symbols);
    void connect();


private:
	mongocxx::client conn{mongocxx::uri{}};
    mongocxx::database db;
};

#endif // MONGO_H
