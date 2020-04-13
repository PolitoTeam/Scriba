#include "mongo.h"
#include <QRandomGenerator>
#include <QDebug>
#include <sodium.h>
#include <nlohmann/json.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using json = nlohmann::json;

Mongo::Mongo()
{

}

bool Mongo::insertNewFile(const QString filename, const QString username) {
	try {

		bsoncxx::builder::stream::document document{};
		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc = builder
				<< "filename" << filename.toStdString()
				<< "owner" << username.toStdString()
				<< bsoncxx::builder::stream::finalize;

		auto collection = this->conn["editor"]["files"];
		collection.insert_one(doc.view());

		return true;
	} catch(...) {
		return false;
	}
}

void Mongo::saveFile(const std::vector<std::string>& symbols_json) {
	std::vector<bsoncxx::document::value> documents;
	for (auto symbol : symbols_json) {
//		qDebug() << QString::fromStdString(a);
		auto s = bsoncxx::from_json(symbol);
		documents.push_back(s);
	}

	auto collection = this->conn["editor"]["files"];
	collection.insert_many(documents);
}

void Mongo::saveFile(const QString& symbols) {
	qDebug() << "SYMBOLS" << symbols;
	std::string symbols_str = symbols.toUtf8().constData();
	auto s = bsoncxx::from_json(symbols_str);

	auto collection = this->conn["editor"]["files"];
	collection.insert_one(s.view());
}

void Mongo::retrieveFile(QJsonArray& symbols) {

}

//bool Mongo::checkConnection() {
//	// Do a fake query to check for the connection
//	auto collection = this->conn["test"]["test"];
//	try {
//		auto result = collection.find_one({});
//	} catch (...) {
//		return false;
//	}
//	// TODO: remove fake collection created
//	return true;
//}
