#include <QRandomGenerator>
#include <QDebug>
#include <sodium.h>
#include <nlohmann/json.hpp>
#include "mongo.h"
#include <iostream>
#include <mongocxx/exception/operation_exception.hpp>

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
	//qDebug()<<"Trying to insert new file in MongoDB: "<<filename<<","<<username;
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
	//qDebug()<<"symbols to inser: "<<symbols;
	mongocxx::collection collection = db["files"];
	QJsonDocument doc(symbols);
	QString strJson(doc.toJson(QJsonDocument::Compact));
	//qDebug()<<"Content that we're adding: "<<strJson;

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
	   //qDebug()<<json_object;

	   //qDebug()<<"size: "<<size;
	   for (int i=0;i<size;i++){
		   QString index = QString::number(i);
		   //qDebug()<<index;
		   QJsonValue e = json_object[index];
		   //qDebug()<<"element: "<<e;
		   symbols.append(e);
	   }

	   //qDebug()<<"Symbols: "<<symbols;
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

// Rollback is performed automatically when returning without commitng the
// transaction; 'find_one_and_update' is used to emulated SELECT ... FOR UPDATE
DatabaseError Mongo::signup(const QString &username,const QString &password) {
	auto session = conn.start_session();
	session.start_transaction();
	try {
		mongocxx::collection collection = this->conn["editor"]["users"];
		bsoncxx::builder::stream::document document{};

		auto cursor = collection.find_one_and_update(
					session,
					document << "username" << username.toStdString()
							 << finalize,
					document << "$set" << open_document << "lock"
							 << bsoncxx::oid() << close_document << finalize
		);

		if (cursor) {
			return ALREADY_EXISTING_USER;
		}

		char hashed_password[crypto_pwhash_STRBYTES];
		// Conversion from QString to char *
		QByteArray ba = password.toUtf8();
		const char *password_char = ba.constData();

		if (crypto_pwhash_str(hashed_password, password_char,
							  strlen(password_char),
							  crypto_pwhash_OPSLIMIT_SENSITIVE,
							  crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
			return CRYPTO_ERROR;
		}

		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc = builder
				<< "username" << username.toStdString()
				<< "nickname" << username.toStdString()
				<< "password" << hashed_password
				<< "lock" << bsoncxx::oid() << finalize;
		collection.insert_one(doc.view());

		session.commit_transaction();
		return SUCCESS;
	} catch(const mongocxx::operation_exception& e) {
		qDebug() << e.what();
		session.abort_transaction();
		return QUERY_ERROR;
	}
}

DatabaseError Mongo::login(const QString &username,const QString password,
						   QString& nickname) {
	try {
		mongocxx::collection collection = this->conn["editor"]["users"];
		bsoncxx::builder::stream::document document{};
		auto cursor = collection.find_one(document << "username"
										  << username.toStdString()
										  << finalize);

		if (!cursor) {
			return NON_EXISTING_USER;
		}

		std::string qry = bsoncxx::to_json(*cursor);
		std::string pass = json::parse(qry)["password"];

		QByteArray ba = password.toUtf8();
		const char *password_char = ba.constData();
		const char *hashed_password_char = pass.c_str();
		if (crypto_pwhash_str_verify(hashed_password_char, password_char,
									 strlen(password_char)) != 0) {
			return WRONG_PASSWORD;
		}

		std::string nick = json::parse(qry)["nickname"];
		nickname = QString::fromStdString(nick);
		return SUCCESS;
	} catch(...) {
		return QUERY_ERROR;
	}
}

DatabaseError Mongo::updateNickname(const QString &username,
									   const QString &nickname){
	DatabaseError err = SUCCESS;
//	if (!db.open())
//		err = CONNECTION_ERROR;

//	QSqlDatabase::database().transaction();
//	QSqlQuery qry;
//	qry.prepare("SELECT Username "
//				"FROM USER "
//				"WHERE Username=:username FOR UPDATE");
//	qry.bindValue(":username",username);

//	if (!qry.exec()) {
//		err = QUERY_ERROR;
//	} else if (!qry.next()) {
//		err = NON_EXISTING_USER;
//	} else {
//		QSqlQuery qry;
//		qry.prepare("UPDATE USER "
//					"SET Nickname=:nickname "
//					"WHERE Username=:username");
//		qry.bindValue(":username",username);
//		qry.bindValue(":nickname",nickname);
//		if (!qry.exec()){
//			err = QUERY_ERROR;
//		}
//	}
//	QSqlDatabase::database().commit();

//	db.close();
	return err;
}

DatabaseError  Mongo::updatePassword(const QString &username,
										const QString &oldpass,
										const QString &newpass){
	DatabaseError err = SUCCESS;
//	if (!db.open())
//		err = CONNECTION_ERROR;

//	QSqlDatabase::database().transaction();
//	QSqlQuery qry;
//	qry.prepare("SELECT Username,Password "
//				"FROM USER "
//				"WHERE Username=:username FOR UPDATE");
//	qry.bindValue(":username",username);

//	if (!qry.exec()) {
//		err = QUERY_ERROR;
//	} else if (!qry.next()) {
//		err = NON_EXISTING_USER;
//	} else {
//		const char *password_char = oldpass.toLocal8Bit().data();
//		QString hashed_password = qry.value(1).toString();
//		const char *hashed_password_char = hashed_password.toLocal8Bit().data();
//		if (crypto_pwhash_str_verify(hashed_password_char,
//									 password_char,
//									 strlen(password_char)) != 0) {
//			err = WRONG_PASSWORD;
//		}
//		QSqlQuery qry;
//		qry.prepare("UPDATE USER "
//					"SET Password=:newpassword "
//					"WHERE Username=:username");
//		qry.bindValue(":username",username);

//		char hashed_newpassword[crypto_pwhash_STRBYTES];
//		// Conversion from QString to char *
//		const char *password_newchar = newpass.toLocal8Bit().data();

//		if (crypto_pwhash_str(hashed_newpassword, password_newchar,
//							  strlen(password_newchar),
//							  crypto_pwhash_OPSLIMIT_SENSITIVE,
//							  crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
//			qDebug() << "Error while hashing...";
//		}

//		QString hashed_password_qstring = QString::fromUtf8(hashed_newpassword);
//		qry.bindValue(":newpassword",hashed_password_qstring);

//		if (!qry.exec()){
//			err = QUERY_ERROR;
//		}
//	}
//	QSqlDatabase::database().commit();

//	db.close();
	return err;
}

// Check if old password provided is correct (used before to update it)
DatabaseError Mongo::checkOldPassword(const QString &username,
										 const QString &oldpass) {
	try {
		auto collection = this->conn["editor"]["user"];
		bsoncxx::builder::stream::document document{};
		auto cursor = collection.find_one(document << "username"
										  << username.toStdString()
										  << finalize);

		if (!cursor) {
			return NON_EXISTING_USER;
		}

		std::string qry = bsoncxx::to_json(*cursor);
		std::string pass = json::parse(qry)["password"];

		const char *old_password_char = oldpass.toLocal8Bit().data();
		QString hashed_old_password = QString::fromStdString(pass);
		const char *hashed_password_char = hashed_old_password
										   .toLocal8Bit().data();
		if (crypto_pwhash_str_verify(hashed_password_char, old_password_char,
									 strlen(old_password_char)) != 0) {
			return WRONG_PASSWORD;
		}
		return SUCCESS;
	} catch(...) {
		return QUERY_ERROR;
	}
}

DatabaseError Mongo::getFiles(const QString &username,
								 QVector<QPair<QString,QString>> &files,
								 bool shared){
	DatabaseError err = SUCCESS;
//	if (!db.open())
//		err = CONNECTION_ERROR;

//	QSqlQuery qry;
//	if (!shared) {
//		// select own files (DISTINCT because every file can have 2 entries,
//		// corresponding to the public and private shared link)
//		qry.prepare("SELECT DISTINCT Name "
//					"FROM FILE "
//					"WHERE Owner=:username");
//	} else {
//		// select shared files (no distinction between public and private ones)
//		qry.prepare("SELECT Name, Owner "
//					"FROM FILE, FILE_USER "
//					"WHERE FILE.Link = FILE_USER.Link "
//					"AND User = :username");
//	}
//	qry.bindValue(":username", username);

//	if (!qry.exec()) {
//		err = QUERY_ERROR;
//	} else {
//		if (!shared) {
//			while (qry.next()) {
//				files.push_back(QPair<QString, QString>(qry.value(0).toString(),
//														username));
//			}
//		} else {
//			while (qry.next()) {
//				files.push_back(QPair<QString, QString>(qry.value(0).toString(),
//														qry.value(1).toString())
//								);
//			}
//		}

//		//        // add files--> public ones: the files that are public and that the user has already accessed: it means that PUBLIC=true and the fileis in the FILE_USER table
//		//        qry.prepare("SELECT Name, Owner "
//		//                    "FROM FILE, FILE_USER "
//		//                    "WHERE FILE.Link = FILE_USER.Link AND User = :username AND Public = TRUE");
//		//        qry.bindValue(":username", username);
//		//        if (!qry.exec())
//		//            err = QUERY_ERROR;
//		//        else {
//		//            while (qry.next()) {
//		//                files.push_back(QPair<QString, QString>(qry.value(0).toString(), qry.value(1).toString()));
//		//            }
//		//        }

//		//        // add files--> private ones: the files that are privat and that the user has already accessed: it means that PUBLIC=false and the file is in the FILE_USER table with FIRST_ACESS=FALSE
//		//        qry.prepare("SELECT Name, Owner "
//		//                    "FROM FILE, FILE_USER "
//		//                    "WHERE FILE.Link = FILE_USER.Link AND User = :username AND Public = FALSE and First_access = FALSE");
//		//        qry.bindValue(":username", username);
//		//        if (!qry.exec())
//		//            err = QUERY_ERROR;
//		//        else {
//		//            while (qry.next()) {
//		//                files.push_back(QPair<QString, QString>(qry.value(0).toString(), qry.value(1).toString()));
//		//            }
//		//        }

//		if (files.isEmpty()) {
//			err = NO_FILES_AVAILABLE;
//		}
//	}

//	db.close();
	return err;
}

DatabaseError Mongo::newFile(const QString &username,
								const QString &filename, QString &sharedLink)
{
	DatabaseError err = SUCCESS;
//	if (!db.open())
//		err = CONNECTION_ERROR;

//	QSqlDatabase::database().transaction();
//	QSqlQuery qry;
//	qry.prepare("SELECT Name "
//				"FROM FILE "
//				"WHERE Name=:filename "
//				"AND Owner=:username FOR UPDATE");
//	qry.bindValue(":filename", filename);
//	qry.bindValue(":username", username);

//	if (!qry.exec()) {
//		err = QUERY_ERROR;
//	}
//	else if (qry.next()) {
//		err = ALREADY_EXISTING_FILE;
//	} else {
//		bool alreadyExisitingLink = true;
//		while (alreadyExisitingLink) {
//			sharedLink = "shared_editor://file/" + generateRandomString();
//			QSqlQuery qry;
//			qry.prepare("SELECT * "
//						"FROM FILE "
//						"WHERE Link=:link FOR UPDATE");
//			qry.bindValue(":link", sharedLink);
//			if (!qry.exec()) {
//				err = QUERY_ERROR;
//			}

//			if (!qry.next()) {
//				alreadyExisitingLink = false;
//			}
//		}

//		QSqlQuery qry;
//		qry.prepare("INSERT INTO FILE (Link, Name, Owner, Public) "
//					"VALUES (:link, :filename, :username, TRUE)");
//		qry.bindValue(":link", sharedLink);
//		qry.bindValue(":filename", filename);
//		qry.bindValue(":username", username);
//		if (!qry.exec()){
//			err = QUERY_ERROR;
//		}
//	}
//	QSqlDatabase::database().commit();

//	db.close();
	return err;
}

DatabaseError Mongo::getSharedLink(const QString &author,
									  const QString &filename,
									  QString &sharedLink){
	DatabaseError err = SUCCESS;
//	if (!db.open())
//		err = CONNECTION_ERROR;

//	QSqlQuery qry;
//	qry.prepare("SELECT Link "
//				"FROM FILE "
//				"WHERE Owner=:owner AND Name=:name");
//	qry.bindValue(":owner", author);
//	qry.bindValue(":name", filename);
//	if (!qry.exec()) {
//		err = QUERY_ERROR;
//	} else if (!qry.next()) {
//		err = NON_EXISTING_FILE;
//	} else {
//		sharedLink = qry.value(0).toString();
//	}
//	db.close();
	return err;
}

DatabaseError Mongo::getFilenameFromSharedLink(const QString& sharedLink,
												  QString& filename,
												  const QString& user) {
	DatabaseError err = SUCCESS;
//	if (!db.open())
//		err = CONNECTION_ERROR;

//	QSqlDatabase::database().transaction();
//	QSqlQuery qry;
//	qry.prepare("SELECT Name, Owner "
//				"FROM FILE "
//				"WHERE Link=:link FOR UPDATE");
//	qry.bindValue(":link", sharedLink);

//	if (!qry.exec()) {
//		err = QUERY_ERROR;
//	} else if (!qry.next()) {
//		err = NON_EXISTING_FILE;
//	} else {
//		filename = qry.value(0).toString() + "," + qry.value(1).toString();

//		// Add file to list of shared files of 'user'
//		QSqlQuery qry;
//		qry.prepare("INSERT IGNORE INTO FILE_USER (Link, User, First_access) "
//					"VALUES (:link, :user, 1)");
//		qry.bindValue(":link", sharedLink);
//		qry.bindValue(":user", user);
//		if (!qry.exec()){
//			err = QUERY_ERROR;
//		}
//	}
//	QSqlDatabase::database().commit();

//	db.close();
	return err;
}

QString Mongo::generateRandomString() const
{
	const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcde"
									 "fghijklmnopqrstuvwxyz0123456789");

	QString randomString;
	for(int i = 0; i < SHARE_LINK_LENGTH; i++) {
		int index = QRandomGenerator::global()->generate()
				% possibleCharacters.length();
		QChar nextChar = possibleCharacters.at(index);
		randomString.append(nextChar);
	}
	return randomString;
}
