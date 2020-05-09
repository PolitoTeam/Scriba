#include <QRandomGenerator>
#include <QDebug>
#include <sodium.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <mongocxx/exception/operation_exception.hpp>
#include "mongo.h"

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
	try {
		mongocxx::collection collection = db["files"];
		bsoncxx::builder::stream::document document{};
		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc = builder
				<< "filename" << filename.toStdString() + ","
								 + username.toStdString()
				<< bsoncxx::builder::stream::finalize;
		collection.insert_one(doc.view());
		return true;
	} catch (std::exception& e) {
		qDebug() << e.what();
		return false;
	}
}

void Mongo::saveFile(const QString filename, const QJsonArray& symbols) {
	mongocxx::collection collection = db["files"];
	QJsonDocument doc(symbols);
	QString strJson(doc.toJson(QJsonDocument::Compact));

	std::string symbols_str = strJson.toUtf8().constData();
	bsoncxx::document::value s = bsoncxx::from_json(symbols_str);

	collection.update_one(document{} << "filename" << filename.toStdString()
									 << finalize,
						  document{} << "$set" << open_document
											<< "content" << s.view()
									 << close_document << finalize);
}

bool Mongo::retrieveFile(const QString filename, QJsonArray& symbols) {
	mongocxx::collection collection = db["files"];

	bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
	  collection.find_one(document{} << "filename" << filename.toStdString()
						  << finalize);
	if (maybe_result) {
		bsoncxx::document::element element = (*maybe_result).view()["content"];

		if(element.type() != bsoncxx::type::k_document) {
			return false;
		}

		QString q_content = QString::fromStdString(
								bsoncxx::to_json(element.get_document())
		);

	   QJsonDocument json_doc = QJsonDocument::fromJson(q_content.toUtf8());
	   QJsonObject  json_object = json_doc.object();
	   int size = json_object.size();

	   for (int i=0;i<size;i++){
		   QString index = QString::number(i);
		   QJsonValue e = json_object[index];
		   symbols.append(e);
	   }
	   return true;
	}
	else {
		return false;
	}
}

bool Mongo::checkConnection() {
	// Check connection: if exception raised, no connection can be established
	// Also create the collection 'users' (otherwise exception raised when
	// trying to create collection during transaction, e.g. in signup())
	try {
		if (!this->db.has_collection("users")) {
			this->db.create_collection("users");
		}
	} catch (std::exception& e) {
		qDebug() << e.what();
		return false;
	}
	return true;
}

// Rollback is performed automatically when returning without commitng the
// transaction; 'find_one_and_update' is used to emulated SELECT ... FOR UPDATE
DatabaseError Mongo::signup(const QString &username, const QString &password) {
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
				<< finalize;
		collection.insert_one(session, doc.view());

		session.commit_transaction();
		return SUCCESS;
	} catch (std::exception& e) {
		qDebug() << e.what();
		session.abort_transaction();
		return QUERY_ERROR;
	}
}

DatabaseError Mongo::login(const QString &username, const QString password,
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
	} catch (std::exception& e) {
		qDebug() << e.what();
		return QUERY_ERROR;
	}
}

DatabaseError Mongo::updateNickname(const QString &username,
									const QString &nickname) {
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

		if (!cursor) {
			return NON_EXISTING_USER;
		}

		collection.update_one(
					session,
					document << "username" << username.toStdString()
							 << finalize,
					document << "$set" << open_document << "nickname"
							 << nickname.toStdString() << close_document
							 << finalize
		);

		session.commit_transaction();
		return SUCCESS;
	} catch (std::exception& e) {
		qDebug() << e.what();
		session.abort_transaction();
		return QUERY_ERROR;
	}
}

DatabaseError  Mongo::updatePassword(const QString &username,
									 const QString &oldpass,
									 const QString &newpass) {
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

		if (!cursor) {
			return NON_EXISTING_USER;
		}

		std::string qry = bsoncxx::to_json(*cursor);
		std::string pass = json::parse(qry)["password"];

		QByteArray ba = oldpass.toUtf8();
		const char *password_char = ba.constData();
		const char *hashed_password_char = pass.c_str();
		if (crypto_pwhash_str_verify(hashed_password_char, password_char,
									 strlen(password_char)) != 0) {
			return WRONG_PASSWORD;
		}

		char hashed_newpassword[crypto_pwhash_STRBYTES];
		ba = newpass.toUtf8();
		const char *password_newchar = ba.constData();

		// Generate hash for the new password
		if (crypto_pwhash_str(hashed_newpassword, password_newchar,
							  strlen(password_newchar),
							  crypto_pwhash_OPSLIMIT_SENSITIVE,
							  crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
			return CRYPTO_ERROR;
		}

		collection.update_one(
					session,
					document << "username" << username.toStdString()
							 << finalize,
					document << "$set" << open_document << "password"
							 << hashed_newpassword << close_document
							 << finalize
		);

		session.commit_transaction();
		return SUCCESS;
	} catch(const mongocxx::operation_exception& e) {
		qDebug() << e.what();
		session.abort_transaction();
		return QUERY_ERROR;
	}
}

// Check if old password provided is correct (used before to update it)
DatabaseError Mongo::checkOldPassword(const QString &username,
									  const QString &oldpass) {
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

		QByteArray ba = oldpass.toUtf8();
		const char *old_password_char = ba.constData();
		QString hashed_old_password = QString::fromStdString(pass);
		const char *hashed_password_char = pass.c_str();
		if (crypto_pwhash_str_verify(hashed_password_char, old_password_char,
									 strlen(old_password_char)) != 0) {
			return WRONG_PASSWORD;
		}
		return SUCCESS;
	} catch (std::exception& e) {
		qDebug() << e.what();
		return QUERY_ERROR;
	}
}

DatabaseError Mongo::getFiles(const QString &username,
							  QVector<QPair<QString,QString>> &files,
							  bool shared) {
	try {
		mongocxx::collection collection = this->conn["editor"]["users"];
		bsoncxx::builder::stream::document document{};

		if (!shared) {
			mongocxx::options::find opts{};
			opts.projection(document << "files.name" << 1 << finalize);

			auto cursor = collection.find_one(document << "username"
											  << username.toStdString()
											  << finalize,
											  opts);

			if (!cursor) {
				return NON_EXISTING_USER;
			}

			std::string qry = bsoncxx::to_json(*cursor);
			json object = json::parse(qry);

			for (auto file : object["files"]) {
				QString file_qstr = QString::fromStdString(file["name"]);
				files.push_back(QPair<QString, QString>(file_qstr, username));
			}
		} else {
			mongocxx::options::find opts{};
			opts.projection(document << "shared_with_me" << 1 << finalize);

			auto cursor = collection.find_one(document << "username"
											  << username.toStdString()
											  << finalize,
											  opts);

			if (!cursor) {
				return NON_EXISTING_USER;
			}

			std::string qry = bsoncxx::to_json(*cursor);
			json object = json::parse(qry);

			for (auto file : object["shared_with_me"]) {
				QString file_qstr = QString::fromStdString(file["filename"]);
				QString owner = QString::fromStdString(file["owner"]);
				files.push_back(QPair<QString, QString>(file_qstr, owner));
			}
		}

		if (files.isEmpty()) {
			return NO_FILES_AVAILABLE;
		}

		return SUCCESS;
	} catch (std::exception& e) {
		qDebug() << e.what();
		return QUERY_ERROR;
	}
}

DatabaseError Mongo::newFile(const QString &username, const QString &filename,
							 QString &sharedLink) {
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

		if (!cursor) {
			return NON_EXISTING_USER;
		}

		cursor = collection.find_one_and_update(
					session,
					document << "username" << username.toStdString()
							 << "files" << open_document
								<< "$elemMatch" << open_document
									<< "name"
									<< filename.toStdString()
								<< close_document
							 << close_document << finalize,
					document << "$set" << open_document
								<< "lock"
								<< bsoncxx::oid()
							 << close_document << finalize
		);

		if (cursor) {
			return ALREADY_EXISTING_FILE;
		}

		bool alreadyExisitingLink = true;
		while (alreadyExisitingLink) {
			sharedLink = "shared_editor://file/" + generateRandomString();
			auto cursor = collection.find_one_and_update(
					session,
					document << "files" << open_document
								<< "$elemMatch" << open_document
									<< "link"
									<< sharedLink.toStdString()
								<< close_document
							 << close_document << finalize,
					document << "$set" << open_document
								<< "lock"
								<< bsoncxx::oid()
							 << close_document << finalize
			);

			if (!cursor) {
				alreadyExisitingLink = false;
			}
		}

		collection.update_one(
					session,
					document << "username" << username.toStdString()
							 << finalize,
					document << "$push" << open_document
								<< "files" << open_document
									<< "name" << filename.toStdString()
									<< "link" << sharedLink.toStdString()
								<< close_document
							 << close_document << finalize
		);

		session.commit_transaction();
		return SUCCESS;
	} catch(std::exception& e) {
		qDebug() << e.what();
		session.abort_transaction();
		return QUERY_ERROR;
	}
}

DatabaseError Mongo::getSharedLink(const QString &author,
								   const QString &filename,
								   QString &sharedLink) {
	sharedLink = "ERROR";
	try {
		mongocxx::collection collection = this->conn["editor"]["users"];
		bsoncxx::builder::stream::document document{};

		mongocxx::options::find opts{};
		opts.projection(document << "files" << open_document
										<< "$elemMatch" << open_document
											<< "name" << filename.toStdString()
										<< close_document
								 << close_document << finalize);

		auto cursor = collection.find_one(
					document << "username" << author.toStdString()
							 << "files" << open_document
									<< "$elemMatch" << open_document
										<< "name"
										<< filename.toStdString()
									<< close_document
							 << close_document << finalize,
					opts
		);

		if (!cursor) {
			return NON_EXISTING_FILE;
		}

		std::string qry = bsoncxx::to_json(*cursor);
		std::string link = json::parse(qry)["files"][0]["link"];

		sharedLink = QString::fromStdString(link);
		return SUCCESS;
	} catch (std::exception& e) {
		qDebug() << e.what();
		return QUERY_ERROR;
	}
}

DatabaseError Mongo::getFilenameFromSharedLink(const QString& sharedLink,
											   QString& filename,
											   const QString& user) {
	filename = "ERROR";

	auto session = conn.start_session();
	session.start_transaction();
	try {
		mongocxx::collection collection = this->conn["editor"]["users"];
		bsoncxx::builder::stream::document document{};

		mongocxx::options::find_one_and_update opts{};
		opts.projection(document
						<< "username" << 1
						<< "files" << open_document
										<< "$elemMatch" << open_document
											<< "link" << sharedLink.toStdString()
										<< close_document
								 << close_document << finalize);

		auto cursor = collection.find_one_and_update(
					session,
					document << "files" << open_document
								<< "$elemMatch" << open_document
									<< "link"
									<< sharedLink.toStdString()
								<< close_document
							 << close_document << finalize,
					document << "$set" << open_document
								<< "lock"
								<< bsoncxx::oid()
							 << close_document << finalize,
					opts
		);

		if (!cursor) {
			return NON_EXISTING_FILE;
		}

		std::string qry = bsoncxx::to_json(*cursor);
		std::string name = json::parse(qry)["files"][0]["name"];
		std::string owner = json::parse(qry)["username"];
		filename = QString::fromStdString(name)
				   + "," + QString::fromStdString(owner);

		collection.update_one(
					session,
					document << "username" << user.toStdString()
							 << finalize,
					document << "$addToSet" << open_document
								<< "shared_with_me" << open_document
									<< "filename" << name
									<< "owner" << owner
								<< close_document
							 << close_document << finalize
		);

		session.commit_transaction();
		return SUCCESS;
	} catch (std::exception& e) {
		qDebug() << e.what();
		session.abort_transaction();
		return QUERY_ERROR;
	}
}

QString Mongo::generateRandomString() const {
	const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcde"
									 "fghijklmnopqrstuvwxyz0123456789");

	QString randomString;
	for(int i = 0; i < SHARE_LINK_LENGTH; i++) {
		int index = (QRandomGenerator::global()->generate()
					 % possibleCharacters.length());
		QChar nextChar = possibleCharacters.at(index);
		randomString.append(nextChar);
	}
	return randomString;
}

DatabaseError Mongo::checkAlreadyExistingUsername(const QString &username) {
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
        return SUCCESS;
    } catch (std::exception& e) {
        qDebug() << e.what();
        session.abort_transaction();
        return QUERY_ERROR;
    }
}
