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
#include <QtEndian>

ServerWorker::ServerWorker(QObject *parent)
	: QObject(parent)
	, m_serverSocket(new QSslSocket(this))
{
	connect(m_serverSocket, &QSslSocket::readyRead,
			this, &ServerWorker::onReadyRead);
	connect(m_serverSocket, &QSslSocket::disconnected,
			this, &ServerWorker::disconnectedFromClient);
	connect(m_serverSocket, &QSslSocket::stateChanged,
			this, [](QAbstractSocket::SocketState socketState){/*qDebug()<<socketState;*/});
	connect(m_serverSocket, &QSslSocket::encrypted,
			[](){/*qDebug()<<"Encrypted   done!";*/});
	connect(m_serverSocket,
			QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
			this, &ServerWorker::sslErrors);
}

void ServerWorker::sslErrors(const QList<QSslError> &errors)
{
	foreach (const QSslError &error, errors)
        qDebug() <<"ERROR :"<< error.errorString();
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor,
									   QSslKey key, QSslCertificate cert) {
	if (m_serverSocket->setSocketDescriptor(socketDescriptor)) {
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
	m_serverSocket->disconnectFromHost();
}

void ServerWorker::onReadyRead()
{
	if (m_serverSocket->bytesAvailable()>0) {
        m_received_data.append(m_serverSocket->readAll());
    }

    if (m_received_data.isNull()|| m_received_data.size()<8){
        return;
    }

	if (m_exptected_json_size == 0) {
        extract_content_size();
    }

    // If data completely received
    if (m_exptected_json_size > 0
			&& m_received_data.size() >= m_exptected_json_size + 8) {
        if(parseJson()) {
            m_exptected_json_size=0;
            onReadyRead();
        }
    }
}

void ServerWorker::extract_content_size()
{
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
    QByteArray json_data;
    QDataStream in;
    m_buffer.setBuffer(&m_received_data);
	if (!m_buffer.open(QIODevice::ReadOnly)) {
        return false;
	}

    in.setDevice(&m_buffer);
    in.setVersion(QDataStream::Qt_5_7);
    in.startTransaction();
    quint64 json_size;
    in >> json_size >> json_data;
    json_data.truncate(json_size);

	if (!in.commitTransaction()) {
        m_buffer.close();
        return false;
    }
    m_buffer.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json_data, &parseError);

    if (parseError.error == QJsonParseError::NoError) {
		if (jsonDoc.isObject()) {
			emit jsonReceived(jsonDoc.object());
		} else {
            qDebug() << "Invalid message: " + QString::fromUtf8(json_data);
		}
    } else {
		emit signingUp_updatingImage(json_data);
    }
    m_received_data.remove(0,8+json_size);
    return true;
}


QString ServerWorker::getFilename(){
	return filename;
}

void ServerWorker::setFilename(const QString& filename){
	this->filename=filename;
}

void ServerWorker::closeFile(){
	this->filename.clear();
}
