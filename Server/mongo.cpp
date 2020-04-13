#include <QRandomGenerator>
#include <QDebug>
#include <sodium.h>
#include <nlohmann/json.hpp>
#include "mongo.h"
#include <iostream>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using json = nlohmann::json;

Mongo::Mongo() { }

void Mongo::connect() {
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
    qDebug()<<"symbols to inser: "<<symbols;
    mongocxx::collection collection = db["files"];
    QJsonDocument doc(symbols);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<<"Content that we're adding: "<<strJson;

    std::string symbols_str = strJson.toUtf8().constData();
    bsoncxx::document::value s = bsoncxx::from_json(symbols_str);


    collection.update_one(document{} << "filename" << filename.toStdString() << finalize,
                          document{} << "$set" << open_document <<
                            "content" << s.view() << close_document << finalize);
}

bool Mongo::retrieveFile(const QString filename, QJsonArray& symbols) {
    mongocxx::collection collection = db["files"];

    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
      collection.find_one(document{} << "filename" << filename.toStdString() << finalize);
    if(maybe_result) {
       bsoncxx::document::element element = (*maybe_result).view()["content"];

       if(element.type() != bsoncxx::type::k_document) {
         return false;
       }

       QString q_content = QString::fromStdString(bsoncxx::to_json(element.get_document()));

       QJsonDocument json_doc = QJsonDocument::fromJson(q_content.toUtf8());
       QJsonObject  json_object = json_doc.object();
       int size = json_object.size();
       qDebug()<<json_object;

       qDebug()<<"size: "<<size;
       for (int i=0;i<size;i++){
           QString index = QString::number(i);
           qDebug()<<index;
           QJsonValue e = json_object[index];
           qDebug()<<"element: "<<e;
           symbols.append(e);
       }

       qDebug()<<"Symbols: "<<symbols;
       return true;
    }
    else {
        return false;
    }

}

bool Mongo::checkConnection() {
	// Do a fake query to check for the connection
	auto collection = this->conn["editor"]["files"];
	try {
		auto result = collection.find_one({});
	} catch (...) {
		return false;
	}
	return true;
}
