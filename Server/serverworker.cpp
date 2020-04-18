#include "serverworker.h"
#include "server.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QWidget>
#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QThread>

ServerWorker::ServerWorker(QObject *parent)
	: QObject(parent)
	, m_serverSocket(new QSslSocket(this))
{
    connect(m_serverSocket, &QSslSocket::readyRead, this, &ServerWorker::onReadyRead);
	connect(m_serverSocket, &QSslSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
	connect(m_serverSocket, &QSslSocket::stateChanged,this,[](QAbstractSocket::SocketState socketState){qDebug()<<socketState;});
	connect(m_serverSocket, &QSslSocket::encrypted, [](){qDebug()<<"Encrypted   done!";});
	connect(m_serverSocket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),this,&ServerWorker::sslErrors);
}

void ServerWorker::sslErrors(const QList<QSslError> &errors)
{
	foreach (const QSslError &error, errors)
        qDebug() <<"ERROR :"<< error.errorString();
}



bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor,QSslKey key,QSslCertificate cert)
{
	//qDebug() << "New Connection! ";

	if (m_serverSocket->setSocketDescriptor(socketDescriptor)) {
		//qDebug() << "Socket Descriptor Set! ";

		if (m_serverSocket->waitForConnected()){

			//qDebug()<<"key"<<key;
			//qDebug()<<"cert"<<cert;
			m_serverSocket->setPrivateKey(key);
			m_serverSocket->setLocalCertificate(cert);

			m_serverSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
			m_serverSocket->startServerEncryption();
		}
		return true;
	} else {
		//qDebug() << "Socket Descriptor Not Set! ";
		return false;
	}

}

void ServerWorker::sendByteArray(const QByteArray &byteArray){
	SerializeSize size;
	quint64 json_size = size(byteArray);

	QDataStream socketStream(m_serverSocket);
	socketStream.setVersion(QDataStream::Qt_5_7);
	socketStream << json_size << byteArray;
}

void ServerWorker::sendJson(const QJsonObject &json)
{
	const QByteArray jsonData = QJsonDocument(json).toJson();
	sendByteArray(jsonData);
}

void ServerWorker::sendProfileImage()
{
	QString image_path = QDir::currentPath() + IMAGES_PATH + "/"
			+ username + ".png";
	QFileInfo file(image_path);
	if (!file.exists()) {
		return;
	}

	// Read Image
	QPixmap p(image_path);
	QByteArray bArray;
	QBuffer buffer(&bArray);
	buffer.open(QIODevice::WriteOnly);
	p.save(&buffer, "PNG");

	sendByteArray(bArray);
}

QString ServerWorker::getNickname()
{
	return nickname;
}

QString ServerWorker::getUsername()
{
	return username;
}


void ServerWorker::setUsername(const QString &username) {
	this->username = username;
}

void ServerWorker::setNickname(const QString &nickname)
{
	this->nickname=nickname;
}


void ServerWorker::clearNickname()
{
	nickname.clear();
}


void ServerWorker::disconnectFromClient()
{
	//    QJsonObject message;
	//    message["type"] = QStringLiteral("server_stopped");
	//    this->sendJson(message);
	m_serverSocket->disconnectFromHost();
}

void ServerWorker::onReadyRead()
{
    //    qDebug()<<"onReadyRead";

    if (m_serverSocket->bytesAvailable()>0)
        m_received_data.append(m_serverSocket->readAll());
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

void ServerWorker::extract_content_size()
{
    //quint64 asize = m_clientSocket->bytesAvailable();
    m_received_data.append(m_serverSocket->readAll());
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

bool ServerWorker::parseJson()
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
        if (jsonDoc.isObject()){
            //qDebug().noquote() << QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
            QJsonObject docObj=jsonDoc.object();
            const QJsonValue typeVal = docObj.value(QLatin1String("type"));
            if (!typeVal.isNull() && typeVal.isString() && typeVal.toString().compare(QLatin1String("image_signup"), Qt::CaseInsensitive) == 0){
                const QJsonValue imageName = docObj.value(QLatin1String("image_name"));
                if (!imageName.isNull() && imageName.isString()){
                    QString im_name = imageName.toString().simplified();
                    //qDebug()<<im_name;
                    if (!im_name.isEmpty())
                        image_name=im_name;
                    //qDebug()<<image_name;
                }

            }

            else
                emit jsonReceived(jsonDoc.object());
        }
        else
            qDebug() << "Invalid message: " + QString::fromUtf8(json_data);
    } else {
        QPixmap p;
        p.loadFromData(json_data);
        //qDebug() << image_name;
        if (image_name.isEmpty() || image_name.isNull())
            image_name=username;
        QString image_path = QDir::currentPath() + IMAGES_PATH + "/" + image_name + ".png";
        image_name.clear();

        QFile file(image_path);
        if (file.exists()) // WriteOnly doesn't seem to override as it should be
            file.remove(); // according to the documentation, need to remove manually
        if (!file.open(QIODevice::WriteOnly))
            //qDebug() << "Unable to open the file specified";

            p.save(&file, "PNG");
        //qDebug().nospace() << "Overriding image " << image_path;
    }
    m_received_data.remove(0,8+json_size);
    return true;
}

/*void ServerWorker::receiveJson()
{
	QByteArray jsonData;
	QDataStream socketStream(m_serverSocket);

	// //qDebug()<<"Thread: "<<QThread::currentThreadId()<<endl;
	socketStream.setVersion(QDataStream::Qt_5_7);
	for (;;) {
		socketStream.startTransaction();
		socketStream >> jsonData;
		if (socketStream.commitTransaction()) {
			QJsonParseError parseError;
			const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
			if (parseError.error == QJsonParseError::NoError) {
				if (jsonDoc.isObject()){
					//qDebug().noquote() << QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
					QJsonObject docObj=jsonDoc.object();
					const QJsonValue typeVal = docObj.value(QLatin1String("type"));
					if (!typeVal.isNull() && typeVal.isString() && typeVal.toString().compare(QLatin1String("image_signup"), Qt::CaseInsensitive) == 0){
						const QJsonValue imageName = docObj.value(QLatin1String("image_name"));
						if (!imageName.isNull() && imageName.isString()){
							QString im_name = imageName.toString().simplified();
							//qDebug()<<im_name;
							if (!im_name.isEmpty())
								image_name=im_name;
							//qDebug()<<image_name;
						}

					}

					else
						emit jsonReceived(jsonDoc.object());
				}
				else
					qDebug() << "Invalid message: " + QString::fromUtf8(jsonData);
			} else {
				QPixmap p;
				p.loadFromData(jsonData);
				//qDebug() << image_name;
				if (image_name.isEmpty() || image_name.isNull())
					image_name=username;
				QString image_path = QDir::currentPath() + IMAGES_PATH + "/" + image_name + ".png";
				image_name.clear();

				QFile file(image_path);
				if (file.exists()) // WriteOnly doesn't seem to override as it should be
					file.remove(); // according to the documentation, need to remove manually
				if (!file.open(QIODevice::WriteOnly))
					//qDebug() << "Unable to open the file specified";

					p.save(&file, "PNG");
				//qDebug().nospace() << "Overriding image " << image_path;
			}
		} else {
			//qDebug()<<"break";
			break;
		}
	}
}
*/

QString ServerWorker::getFilename(){
	return filename;
}

void ServerWorker::setFilename(const QString& filename){
	this->filename=filename;
}

void ServerWorker::closeFile(){
	this->filename.clear();
}
/*
void ServerWorker::setColor(int color)
{
	this->color_rgb = color;
}
*/
