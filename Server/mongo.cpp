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

bool Mongo::checkConnection() {
	// Do a fake query to check for the connection
	auto collection = this->conn["test"]["test"];
	try {
		auto result = collection.find_one({});
	} catch (...) {
		return false;
	}
	// TODO: remove fake collection created
	return true;
}

DatabaseError Mongo::signup(const QString &username,const QString &password){
	// TODO: make operation atomic
	try {
		auto collection = this->conn["editor"]["user"];
		bsoncxx::builder::stream::document document{};
		auto cursor = collection.find_one(document << "username"
										  << username.toStdString()
										  << finalize);

		if (cursor) {
			return ALREADY_EXISTING_USER;
		}

		char hashed_password[crypto_pwhash_STRBYTES];
		// Conversion from QString to char *
		const char *password_char = password.toLocal8Bit().data();

		if (crypto_pwhash_str(hashed_password, password_char,
							  strlen(password_char),
							  crypto_pwhash_OPSLIMIT_SENSITIVE,
							  crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
			return CRYPTO_ERROR;
		}

		QString hashed_pass_qstring = QString::fromUtf8(hashed_password);
		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc = builder
				<< "username" << username.toStdString()
				<< "nickname" << username.toStdString()
				<< "password" << hashed_pass_qstring.toStdString()
				<< bsoncxx::builder::stream::finalize;
		collection.insert_one(doc.view());

		return SUCCESS;
	} catch(...) {
		return QUERY_ERROR;
	}
}

DatabaseError Mongo::login(const QString &username,const QString &password,
						   QString& nickname) {
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

		const char *password_char = password.toLocal8Bit().data();
		QString hashed_password = QString::fromStdString(pass);
		const char *hashed_password_char = hashed_password.toLocal8Bit().data();
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
			return = WRONG_PASSWORD;
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
