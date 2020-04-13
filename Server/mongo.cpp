#include <QRandomGenerator>
#include <QDebug>
#include <sodium.h>
#include <nlohmann/json.hpp>
#include "mongo.h"

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

void Mongo::connect(){
    db = conn["editor"];
}

bool Mongo::insertNewFile(const QString filename, const QString username) {
    mongocxx::collection collection = db["files"];
    qDebug()<<"Trying to insert new file in MongoDB: "<<filename<<","<<username;
	try {
		bsoncxx::builder::stream::document document{};
		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc = builder
                << "filename" << filename.toStdString()+","+ username.toStdString()
				<< bsoncxx::builder::stream::finalize;
        bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(doc.view());
		return true;
	} catch(...) {
		return false;
	}
}


void Mongo::saveFile(const QString filename, const QJsonArray& symbols) {
	qDebug() << "SYMBOLS" << symbols;
    mongocxx::collection collection = db["files"];
    QJsonDocument doc(symbols);
    QString strJson(doc.toJson(QJsonDocument::Compact));

    std::string symbols_str = strJson.toUtf8().constData();
	auto s = bsoncxx::from_json(symbols_str);
    qDebug()<<"content: "<<QString::fromStdString(symbols_str);

    collection.update_one(document{} << "filename" << filename.toStdString() << finalize,
                          document{} << "$set" << open_document <<
                            "content" << s.view()<< close_document << finalize);
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
