#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QHostAddress>
#include <QDebug>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QBuffer>
#include "client.h"
#include "CRDT.h"
#include "symbol.h"
#include <QSslConfiguration>
#include <QMessageBox>
#include <QtEndian>

Client::Client(QObject *parent, QString addr, quint16 port)
	: QObject(parent)
	, m_clientSocket(new QSslSocket(this))
	, m_loggedIn(false)
{
	this->addr = addr;
	this->port = port;

	// Forward the connected and disconnected signals
	connect(m_clientSocket, &QSslSocket::connected, this, &Client::connected);
	//    connect(m_clientSocket, &QSslSocket::disconnected, this, &Client::disconnected);
	//    connect(m_clientSocket, &QSslSocket::error, this, &Client::error);
	connect(m_clientSocket, static_cast<void (QSslSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
			this, &Client::error);

	connect(m_clientSocket, &QSslSocket::disconnected, this, [this]()->void{this->m_received_data.clear();this->m_exptected_json_size=0;});
    connect(m_clientSocket,&QSslSocket::stateChanged,this,[](QAbstractSocket::SocketState socketState){qDebug()<<socketState;});

	connect(this,&Client::connected,this, []()->void{/*qDebug()<<"New client Connected";*/});
	// connect readyRead() to the slot that will take care of reading the data in
	connect(m_clientSocket, &QSslSocket::readyRead, this, &Client::onReadyRead);


	// Reset the m_loggedIn variable when we disconnec. Since the operation is trivial we use a lambda instead of creating another slot

	connect(m_clientSocket, &QSslSocket::encrypted, this, [](){/*qDebug()<<"encrypted!";*/});
	//    connect(m_clientSocket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),this,&Client::sslErrors);



	/* QFile certFile("/Users/giuseppe.pastore/Documents/Programmazione di sistema/Progetto/SharedEditor/SharedEditor/certificates/server.pem");
	certFile.open(QIODevice::ReadOnly);
	QSslCertificate cert = QSslCertificate(certFile.readAll());
	certFile.close();
	*/

	// TO UNCOMMENT
	/*

	QSslConfiguration::defaultConfiguration().setCaCertificates(QSslConfiguration::systemCaCertificates());
	m_clientSocket->addCaCertificates(QSslConfiguration::systemCaCertificates());
	//qDebug()<<"CA certificates: ";
	for (QSslCertificate x: m_clientSocket->sslConfiguration().caCertificates()){

		//qDebug()<<"\n Common Name: "<<x.issuerInfo(QSslCertificate::CommonName)<<" SubjectName: "<<x.subjectInfo(QSslCertificate::CommonName);
	}

	*/
	m_clientSocket->addCaCertificates(":/resources/certificates/rootCA.crt");
	m_clientSocket->setPeerVerifyMode(QSslSocket::VerifyNone);

	profile=new QPixmap(":/images/anonymous");
}

void Client::sendByteArray(const QByteArray &byteArray){
    SerializeSize size;
    quint64 json_size = size(byteArray);

    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << json_size << byteArray;

}

void Client::login(const QString &username, const QString &password)
{
	connectToServer(QHostAddress(this->addr), this->port);
	//if (m_clientSocket->waitForEncrypted()) {
	//qDebug()<<"mandato login";
	// create a QDataStream operating on the socket
//	QDataStream clientStream(m_clientSocket);
	// set the version so that programs compiled with different versions of Qt can agree on how to serialise
    //clientStream.setVersion(QDataStream::Qt_5_7);
	// Create the JSON we want to send
	QJsonObject message;
	message["type"] = QStringLiteral("login");
	message["username"] = username;
	//aggiungere cifratura oppure passare a QSSLsocket
	message["password"] = password;
	// send the JSON using QDataStream
	//qDebug()<<QJsonDocument(message).toJson(QJsonDocument::Compact);
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
	//qDebug()<<"mandato msg login";
	//  }
}

void Client::signup(const QString &username, const QString &password, QPixmap* image)
{
	connectToServer(QHostAddress(this->addr), this->port);
	// if (m_clientSocket->waitForEncrypted()) {
	// create a QDataStream operating on the socket
    //QDataStream clientStream(m_clientSocket);
	// set the version so that programs compiled with different versions of Qt can agree on how to serialise
    //clientStream.setVersion(QDataStream::Qt_5_7);
	// Create the JSON we want to send
	QJsonObject message;
	message["type"] = QStringLiteral("signup");
	message["username"] = username;
	//aggiungere cifratura oppure passare a QSSLsocket
	message["password"] = password;
	// send the JSON using QDataStream
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    QByteArray obj =QJsonDocument(message).toJson(QJsonDocument::Compact);
	// }
    quint32 size_json = obj.size();
    QByteArray ba((const char *)&size_json, sizeof(size_json)); //depends on the endliness of the machine
    ba.append(obj);

    QByteArray bArray;
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::WriteOnly);
    image->save(&buffer, "PNG");
    quint32 size_img = bArray.size();
    qDebug()<<"Img size: "<<size_img<<" imag size sent: "<<bArray;
    QByteArray p((const char *)&size_img, sizeof(size_img));
    p.append(bArray);
    ba.append(p);

    sendByteArray(ba);

}

void Client::getFiles(bool shared){
	// if (m_clientSocket->waitForEncrypted()) {
	// create a QDataStream operating on the socket
    //QDataStream clientStream(m_clientSocket);
	// set the version so that programs compiled with different versions of Qt can agree on how to serialise
    //clientStream.setVersion(QDataStream::Qt_5_7);
	// Create the JSON we want to send
	QJsonObject message;
	if (shared) {
		message["type"] = QStringLiteral("list_shared_files");
	} else {
		message["type"] = QStringLiteral("list_files");
	}
	message["username"] = this->username;
	//aggiungere cifratura oppure passare a QSSLsocket
	// send the JSON using QDataStream
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
	//}
}

void Client::getFilenameFromLink(const QString& sharedLink) {
	// if (m_clientSocket->waitForEncrypted()) {
	// create a QDataStream operating on the socket
    //QDataStream clientStream(m_clientSocket);
	// set the version so that programs compiled with different versions of Qt can agree on how to serialise
    //clientStream.setVersion(QDataStream::Qt_5_7);
	// Create the JSON we want to send
	QJsonObject message;
	message["type"] = QStringLiteral("filename_from_sharedLink");
	message["sharedLink"] = sharedLink;
	//aggiungere cifratura oppure passare a QSSLsocket
	// send the JSON using QDataStream
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
	//}
}

void Client::updateNickname(const QString &nickname)
{
	//if (m_clientSocket->waitForEncrypted()) {
	// create a QDataStream operating on the socket
    //QDataStream clientStream(m_clientSocket);
	// set the version so that programs compiled with different versions of Qt can agree on how to serialise
    //clientStream.setVersion(QDataStream::Qt_5_7);
	// Create the JSON we want to send
	QJsonObject message;
	message["type"] = QStringLiteral("nickname");
	message["username"] = this->username;
	message["nickname"] = nickname;
	//aggiungere cifratura oppure passare a QSSLsocket
	// send the JSON using QDataStream
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
	this->nickname = nickname;
	// }
}

void  Client::updatePassword(const QString &oldpassword,const QString &newpassword){
	// if (m_clientSocket->waitForEncrypted()) {
	// create a QDataStream operating on the socket
    //QDataStream clientStream(m_clientSocket);
	// set the version so that programs compiled with different versions of Qt can agree on how to serialise
    //clientStream.setVersion(QDataStream::Qt_5_7);
	// Create the JSON we want to send
	QJsonObject message;
	message["type"] = QStringLiteral("password");
	message["username"] = this->username;
	message["oldpass"] = oldpassword;
	message["newpass"] = newpassword;
	//aggiungere cifratura oppure passare a QSSLsocket
	// send the JSON using QDataStream
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
	//}
}

// GUARDARE QUESTA IN CASO DI ERRORE PER RIPRISTINARE SE IN CASO LA SENDBYTEARRAY NON VA
void Client::checkOldPassword(const QString &old_password)
{
    //QDataStream clientStream(m_clientSocket);
    //clientStream.setVersion(QDataStream::Qt_5_7);

	QJsonObject message;
	message["type"] = QStringLiteral("check_old_password");
	message["username"] = this->username;
	message["old_password"] = old_password;
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
}

//Attempts to close the socket. If there is pending data waiting to be written, QAbstractSocket will enter ClosingState and wait until all data has been written.
void Client::disconnectFromHost()
{
    qDebug()<<m_clientSocket->bytesAvailable();
    if  (this->m_loggedIn==true){
        this->username.clear();
        this->nickname.clear();
        this->files.clear();
        this->m_loggedIn=false;
    }
	m_clientSocket->disconnectFromHost();
    if (m_clientSocket->state() == QAbstractSocket::UnconnectedState
        || m_clientSocket->waitForDisconnected(1000)) {
            qDebug("Disconnected!");
    }

}



void Client::jsonReceived(const QJsonObject &docObj)
{
	qDebug() << docObj;
	// actions depend on the type of message
	const QJsonValue typeVal = docObj.value(QLatin1String("type"));
	if (typeVal.isNull() || !typeVal.isString())
		return; // a message with no type was received so we just ignore it
    //qDebug().noquote() << QString::fromUtf8(QJsonDocument(docObj).toJson(QJsonDocument::Compact));

	if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) == 0) { //It's a login message
		if (m_loggedIn)
			return; // if we are already logged in we ignore
		// the success field will contain the result of our attempt to login

		const QJsonValue resultVal = docObj.value(QLatin1String("success"));
		if (resultVal.isNull() || !resultVal.isBool())
			return; // the message had no success field so we ignore
		const bool loginSuccess = resultVal.toBool();
		if (loginSuccess) {
			const QJsonValue user = docObj.value(QLatin1String("username"));

			if (user.isNull() || !user.isString())
				return;

			const QString username = user.toString().simplified();
			if (username.isEmpty()){
				return;
			}
			const QJsonValue nick = docObj.value(QLatin1String("nickname"));

			if (nick.isNull() || !nick.isString())
				return;

			const QString nickname = nick.toString().simplified();
			if (nickname.isEmpty()){
				return;
			}
			this->username=username;
			this->nickname=nickname;

			m_loggedIn=true;// we logged in succesfully and we notify it via the loggedIn signal
			emit loggedIn();
			return;
		}
		// the login attempt failed, we extract the reason of the failure from the JSON
		// and notify it via the loginError signal
		const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
		emit loginError(reasonVal.toString());
	}
	else if (typeVal.toString().compare(QLatin1String("signup"), Qt::CaseInsensitive) == 0) { //It's a login message
		if (m_loggedIn)
			return; // if we are already logged in we ignore
		// the success field will contain the result of our attempt to login
		const QJsonValue resultVal = docObj.value(QLatin1String("success"));
		if (resultVal.isNull() || !resultVal.isBool())
			return; // the message had no success field so we ignore
		const bool signupSuccess = resultVal.toBool();
		if (signupSuccess) {
			// we logged in succesfully and we notify it via the signedUp signal
			emit signedUp();
			return;
		}
		// the signup attempt failed, we extract the reason of the failure from the JSON
		// and notify it via the signupError signal
		const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
		emit signupError(reasonVal.toString());
	} else if (typeVal.toString().compare(QLatin1String("old_password_checked"), Qt::CaseInsensitive) == 0) {
		const QJsonValue resultVal = docObj.value(QLatin1String("success"));
		if (resultVal.isNull() || !resultVal.isBool())
			return;
		const bool oldPasswordCheckSuccess = resultVal.toBool();
		if (!oldPasswordCheckSuccess)
			emit wrongOldPassword();
		else
			emit correctOldPassword();
	} else if (typeVal.toString().compare(QLatin1String("operation"), Qt::CaseInsensitive) == 0) {

		int operation_type = docObj["operation_type"].toInt();
		//        //qDebug() << "operation" << operation_type;
		if (operation_type == INSERT) {
			QJsonObject symbol = docObj["symbol"].toObject();
			Symbol s = Symbol::fromJson(symbol);
			//qDebug() << s.to_string();
			//            //qDebug() << "INSERT";
			emit remoteInsert(s);
		} else if (operation_type == DELETE){
			QJsonArray symbols = docObj["symbols"].toArray();

			//qDebug() << s.to_string();
			//            //qDebug() << "ERASE";
			emit remoteErase(symbols);
		} else if (operation_type == CHANGE) {
			QJsonObject symbol = docObj["symbol"].toObject();
			Symbol s = Symbol::fromJson(symbol);
			//qDebug() << s.to_string();

			//qDebug() << "CHANGE";
			emit remoteChange(s);
		}
		else if (operation_type == PASTE) {
			QJsonArray symbols = docObj["symbols"].toArray();

			//qDebug() << "PASTE";
			emit remotePaste(symbols);
		}
		else if (operation_type == ALIGN) {
			QJsonObject symbol = docObj["symbol"].toObject();
			Symbol s = Symbol::fromJson(symbol);
			emit remoteAlignChange(s);
		} else if (operation_type == CURSOR) {
			QJsonObject symbol = docObj["symbol"].toObject();
			Symbol s = Symbol::fromJson(symbol);
			//qDebug() << s.to_string();

			int editor_id = docObj["editorId"].toInt();
			//qDebug() << "CURSOR" << editor_id;
			emit remoteCursor(editor_id, s);
		}
	}
	else if (typeVal.toString().compare(QLatin1String("list_files"), Qt::CaseInsensitive) == 0) {
		const QJsonValue resultVal = docObj.value(QLatin1String("success"));
		if (resultVal.isNull() || !resultVal.isBool())
			return;
		const bool getFile=resultVal.toBool();

		if (getFile){
			const QJsonValue array= docObj.value(QLatin1String("files"));
			if (array.isNull() || !array.isArray())
				return;
			const QJsonArray array_files=array.toArray();

			this->files.clear();
			foreach (const QJsonValue& v, array_files){
				//                //qDebug()<<"name: "<<v.toObject().value("name").toString()<<" owner: "<< v.toObject().value("owner").toString()<<endl;
				this->files.push_back(QPair<QString, QString>(v.toObject().value("name").toString(),v.toObject().value("owner").toString()));
			}

			const QJsonValue sharedJson = docObj.value(QLatin1String("shared"));
			if (sharedJson.isNull() || !sharedJson.isBool())
				return;
			const bool shared = sharedJson.toBool();
			emit filesReceived(shared);
		} else { // error handling
			const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
			emit openFilesError(reasonVal.toString());
		}
	} else if (typeVal.toString().compare(QLatin1String("new_file"), Qt::CaseInsensitive) == 0) {
		const QJsonValue resultVal = docObj.value(QLatin1String("success"));
		if (resultVal.isNull() || !resultVal.isBool())
			return;
		const bool success = resultVal.toBool();
		if (success) {
			sharedLink = docObj.value(QLatin1String("shared_link")).toString();
			emit addCRDTterminator();
			emit correctNewFile();
		} else {
			const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
			emit wrongNewFile(reasonVal.toString());
		}
	}
	else if (typeVal.toString().compare(QLatin1String("disconnection"), Qt::CaseInsensitive) == 0) {
		const QJsonValue file = docObj.value(QLatin1String("filename"));
		if (file.isNull() || !file.isString())
			return;
		const QJsonValue name = docObj.value(QLatin1String("user"));
		if (name.isNull() || !name.isString())
			return;
		const QJsonValue nickname = docObj.value(QLatin1String("nickname"));
		if (nickname.isNull() || !nickname.isString())
			return;
		if (!file.toString().compare(this->openfile)){
			emit userDisconnected(name.toString(),nickname.toString());
		}

	}
	else if (typeVal.toString().compare(QLatin1String("filename_from_sharedLink"), Qt::CaseInsensitive) == 0) {
		const QJsonValue resultVal = docObj.value(QLatin1String("success"));
		if (resultVal.isNull() || !resultVal.isBool())
			return;
		const bool success = resultVal.toBool();
		if (success) {
			QString filename = docObj.value(QLatin1String("filename")).toString();
			this->openFile(filename);
		} else {
			const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
			emit wrongSharedLink(reasonVal.toString());
		}
	}
	//    else if (typeVal.toString().compare(QLatin1String("server_stopped"), Qt::CaseInsensitive) == 0) {
	//        emit error(QAbstractSocket::ProxyConnectionClosedError);
	//    }
}

void Client::createNewFile(QString filename)
{
	// if (m_clientSocket->waitForEncrypted()) {
    //QDataStream clientStream(m_clientSocket);
    //clientStream.setVersion(QDataStream::Qt_5_7);

	QJsonObject message;
	message["type"] = QStringLiteral("new_file");
	message["filename"] = filename;
	message["author"] = this->username;

	//qDebug().noquote() << QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
	//}
}

void Client::connectToServer(const QHostAddress &address, quint16 port)
{
	QHostAddress localhost = QHostAddress::LocalHost;
	//qDebug()<<"coNNECTING TO: "<<localhost;
	m_clientSocket->connectToHost(localhost,port);
	if (m_clientSocket->waitForConnected()){
		//qDebug()<<"start handshake";
		m_clientSocket->startClientEncryption();
		//qDebug()<<"end handshake";
	}
	// wait 1 minute
	if (m_clientSocket->waitForEncrypted(60000)) {
		//qDebug()<<"Authentication Suceeded";
	}
	else {
		qDebug("Unable to connect to server");
		//            emit error(QAbstractSocket::HostNotFoundError);
	}
}

void Client::onReadyRead()
{
	//    qDebug()<<"onReadyRead";

	if (m_clientSocket->bytesAvailable()>0)
		m_received_data.append(m_clientSocket->readAll());
	else {
		//        qDebug()<<"Richiamata ma niente da leggere";
	}


	if (m_received_data.isNull()|| m_received_data.size()<8){
		//        qDebug()<<"return: "<<m_received_data.size()<<" , "<<m_exptected_json_size;
		return;
	}

	if(m_exptected_json_size == 0) {
		//        qDebug()<<"extract_content_size()";
		// Update m_received_data and m_exptected_json_size
		extract_content_size();
		//        qDebug()<<"After extract "<<m_exptected_json_size;
	}

	// If data completely received
	if (m_exptected_json_size > 0
			&& m_received_data.size() >= m_exptected_json_size+8 ) {
		//        qDebug()<<"received data before: "<<m_received_data.size();
		if(parseJson()) {
			m_exptected_json_size=0;
			onReadyRead();
		}
	}
	//        qDebug()<<"received data before: "<<m_received_data.size();

}

void Client::extract_content_size()
{
	//quint64 asize = m_clientSocket->bytesAvailable();
	m_received_data.append(m_clientSocket->readAll());
	QDataStream in;
	QBuffer in_buffer;
	in_buffer.setBuffer(&m_received_data);
	in_buffer.open(QIODevice::ReadOnly);
	in.setDevice(&in_buffer);
	in.setVersion(QDataStream::Qt_5_7);
	quint64 size = 0;
	in >> size;
	m_exptected_json_size = size;
	in_buffer.close();

}

bool Client::parseJson()
{
	//    qDebug()<<"parseJson";
	QByteArray json_data;
	QDataStream in;
	m_buffer.setBuffer(&m_received_data);
	if (!m_buffer.open(QIODevice::ReadOnly))
		return false;

	in.setDevice(&m_buffer);
	in.setVersion(QDataStream::Qt_5_7);
	in.startTransaction();
	quint64 json_size;
	in >> json_size >> json_data;
	json_data.truncate(json_size);

	if( !in.commitTransaction()) {
		m_buffer.close();
		return false;
	}
	m_buffer.close();

	QJsonParseError parseError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(json_data, &parseError);

	if (parseError.error == QJsonParseError::NoError) {

		if (jsonDoc.isObject()) {
			jsonReceived(jsonDoc.object());
		}
	} else {
        //qDebug()<<parseError.error;
		byteArrayReceived(json_data);
	}
	m_received_data.remove(0,8+json_size);
	return true;
}

void Client::byteArrayReceived(const QByteArray &doc){

	quint32 size = qFromLittleEndian<qint32>(reinterpret_cast<const uchar *>(doc.left(4).data()));
	//quint32 size = reinterpret_cast<quint32>(doc.left(4));

	//qDebug()<<"Byte Array, size json: "<<size;

	QByteArray json = doc.mid(4,size);
	QByteArray image_array = doc.mid(4+size,-1);
	//qDebug()<<"after image_array";

	QJsonParseError parseError;
	// we try to create a json document with the data we received
	const QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &parseError);
	//qDebug()<<"Parse error: "<<parseError.error;
	if (parseError.error == QJsonParseError::NoError) {

		// if the data was indeed valid JSON
		if (jsonDoc.isObject()) {// and is a JSON object
			// actions depend on the type of message
			QJsonObject docObj = jsonDoc.object();
			const QJsonValue typeVal = docObj.value(QLatin1String("type"));
			if (typeVal.isNull() || !typeVal.isString())
				return; // a message with no type was received so we just ignore it

			if (typeVal.toString().compare(QLatin1String("file_to_open"), Qt::CaseInsensitive) == 0) {

				const QJsonValue resultVal = docObj.value(QLatin1String("success"));
				if (resultVal.isNull() || !resultVal.isBool())
					return;
				const bool success = resultVal.toBool();
				if (success) {
					const QJsonValue cont = docObj.value(QLatin1String("content"));
					if (cont.isNull() || !cont.isArray())
						return;
					const QJsonArray symbols = cont.toArray();
					// read the symbols in the file and parse them into the editor
					//                foreach (const QJsonValue & symbol, symbols) {
					//                    Symbol s = Symbol::fromJson(symbol.toObject());
					//                    //qDebug() << "SYMBOL" << s.getValue();
					//                    emit remoteInsert(s);
					//                }
					// REVERSE ORDER to have '\0' as first char
                    //qDebug()<<"To insert";
					progress = new QProgressDialog(nullptr);
					progress->setWindowTitle("Loading...");
					progress->setRange(0, symbols.size() - 1);
					progress->setModal(true);
					progress->setMinimumDuration(500);
					progress->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
					progress->setCancelButton(nullptr);

					int progress_counter = 0;
					for (int i = symbols.size() - 1; i >= 0; i--) {
						Symbol s = Symbol::fromJson(symbols[i].toObject());


                        emit remoteInsert(s);
						if (s.getValue()=='\n' || s.getValue()=='\0')
							emit remoteAlignChange(s);

						progress_counter++;
						progress->setValue(progress_counter);
					}
					progress->hide();
					progress->cancel();


					const QJsonValue name = docObj.value(QLatin1String("filename"));
					if (name.isNull() || !name.isString())
						return;
					this->openfile=name.toString();

					const QJsonValue shared_link = docObj.value(QLatin1String("shared_link"));
					if (shared_link.isNull() || !shared_link.isString())
						return;
					this->sharedLink = shared_link.toString();




					const QJsonValue array= docObj.value(QLatin1String("users"));
					if (array.isNull() || !array.isArray())
						return;
					const QJsonArray array_users=array.toArray();
					QList<QPair<QPair<QString,QString>,QPixmap>> connected;


					foreach (const QJsonValue& v, array_users){
						quint32 img_size = qFromLittleEndian<qint32>(reinterpret_cast<const uchar *>(image_array.left(4).data()));
						//qDebug()<<"Img size: "<<img_size;
						QByteArray img = image_array.mid(4,img_size);
						image_array=image_array.mid(img_size+4);

						//qDebug()<<"username: "<<v.toObject().value("username").toString()<<" nickname: "<< v.toObject().value("nickname").toString()<<endl;
						QPixmap p;
						p.loadFromData(img);
						connected.append(QPair<QPair<QString,QString>,QPixmap>(QPair<QString,QString>(v.toObject().value("username").toString(),v.toObject().value("nickname").toString()),p));
					}
					//                emit contentReceived(cont.toString()); TODO: remove comment
					//qDebug()<<"emitted usersConnectedReceived";

					emit usersConnectedReceived(connected);
					emit correctOpenedFile();


				} else {
					this->openfile.clear();
					const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
					emit wrongListFiles(reasonVal.toString());
				}
			}
			else if (typeVal.toString().compare(QLatin1String("connection"), Qt::CaseInsensitive) == 0) {
				const QJsonValue file = docObj.value(QLatin1String("filename"));
				//qDebug()<<"connection "<<file.toString();
				if (file.isNull() || !file.isString())
					return;
				const QJsonValue name = docObj.value(QLatin1String("username"));
				if (name.isNull() || !name.isString())
					return;
				if (!file.toString().compare(this->openfile)){
					QList<QPair<QPair<QString,QString>,QPixmap>> connected;
					quint32 img_size = qFromLittleEndian<qint32>(reinterpret_cast<const uchar *>(image_array.left(4).data()));
					//qDebug()<<"Img size: "<<img_size;
					if (img_size==0){
						connected.append(QPair<QPair<QString,QString>,QPixmap>(QPair<QString,QString>(docObj.value("username").toString(),docObj.value("nickname").toString()),QPixmap(":/images/anonymous")));
					}
					else{
						QByteArray img = image_array.mid(4);
						QPixmap p;
						p.loadFromData(img);
						connected.append(QPair<QPair<QString,QString>,QPixmap>(QPair<QString,QString>(docObj.value("username").toString(),docObj.value("nickname").toString()),p));
					}

					emit usersConnectedReceived(connected);
				}


			}

		}
		else {
			//qDebug()<<"Not object";
		}

	}else{

		profile->loadFromData(doc);
        //qDebug() << "Profile image loaded";

	}



}

QString Client::getNickname(){
	return this->nickname;
}
QString Client::getUsername(){
	return this->username;
}
void Client::setNickname(const QString& nickname){
	this->nickname=nickname;
}

QPixmap* Client::getProfile(){
	return profile;
}

void Client::setProfileImage(const QString& filename)
{
	profile->load(filename);
}

void Client::sendProfileImage()
{
	// if (m_clientSocket->waitForEncrypted()) {
    /*QDataStream clientStream(m_clientSocket);
    clientStream.setVersion(QDataStream::Qt_5_7);*/

	QByteArray bArray;
	QBuffer buffer(&bArray);
	buffer.open(QIODevice::WriteOnly);
	profile->save(&buffer, "PNG");
    //clientStream << bArray;
    sendByteArray(bArray);
	// }
}

/*
void Client::sendProfileImage(const QString& name,QPixmap* image )
{
	// if (m_clientSocket->waitForEncrypted()) {
    //QDataStream clientStream(m_clientSocket);
    //clientStream.setVersion(QDataStream::Qt_5_7);
	QJsonObject message;
	message["type"] = QStringLiteral("image_signup");
	message["image_name"] = name;
	//qDebug().noquote() << QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
	QByteArray bArray;
	QBuffer buffer(&bArray);
	buffer.open(QIODevice::WriteOnly);
	image->save(&buffer, "PNG");
    //clientStream << bArray;
    sendByteArray(bArray);
	// }
}
*/

void Client::overrideProfileImage(const QPixmap& pixmap)
{
	*this->profile = pixmap;
}


QList<QPair<QString,QString>> Client::getActiveFiles(){
	return files;
}


void Client::sendJson(const QJsonObject& message)
{
    /*
	QDataStream clientStream(m_clientSocket);
	clientStream.setVersion(QDataStream::Qt_5_7);
*/
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
}

void Client::openFile(const QString& filename){
	//  if (m_clientSocket->waitForEncrypted()) {
    //QDataStream clientStream(m_clientSocket);
    //clientStream.setVersion(QDataStream::Qt_5_7);

	QJsonObject message;
	message["type"] = QStringLiteral("file_to_open");
	message["filename"] = filename;

	//qDebug().noquote() << QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
	//  }
}

void Client::closeFile(){
	//  if (m_clientSocket->waitForEncrypted()) {
    //QDataStream clientStream(m_clientSocket);
    //clientStream.setVersion(QDataStream::Qt_5_7);

	QJsonObject message;
	message["type"] = QStringLiteral("close");
	message["filename"] = this->openfile;
	message["username"]=this->username;
	message["nickname"]=this->nickname;

	//qDebug().noquote() << QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
    //clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    sendByteArray(QJsonDocument(message).toJson(QJsonDocument::Compact));
	// }
}

QString Client::getSharedLink()
{
	return this->sharedLink;
}

QString Client::getOpenedFile()
{
	return this->openfile;
}
void Client::setOpenedFile(const QString& name) {
	this->openfile = name;
}


