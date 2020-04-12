#ifndef MONGO_H
#define MONGO_H
#include <QString>
#include <QPair>
#include <QJsonArray>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

class Mongo
{
public:
	Mongo();
	bool insertNewFile(const QString filename, const QString username);
	void saveFile(const std::vector<std::string>& symbols_json);
	void retrieveFile(const std::vector<std::string>& symbols_json);

private:
	mongocxx::instance inst{};
	mongocxx::client conn{mongocxx::uri{}};
};

#endif // MONGO_H
