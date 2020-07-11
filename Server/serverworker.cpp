#include "serverworker.h"
#include "../Utility/byte_reader.h"
//#include "network.h"
#include "server.h"
#include <QBuffer>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QThread>
#include <QWidget>
#include <QtEndian>

ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent), m_serverSocket(new QSslSocket(this)) {
  connect(m_serverSocket, &QSslSocket::readyRead, this,
          &ServerWorker::onReadyRead);
  connect(m_serverSocket, &QSslSocket::disconnected, this,
          &ServerWorker::disconnectedFromClient);
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor, QSslKey key,
                                       QSslCertificate cert) {
  if (m_serverSocket->setSocketDescriptor(socketDescriptor)) {
    if (m_serverSocket->waitForConnected()) {
      m_serverSocket->setPrivateKey(key);
      m_serverSocket->setLocalCertificate(cert);

      // "QSslSocket::VerifyPeer" not enable because doesn't work on macOS
      m_serverSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
      m_serverSocket->startServerEncryption();
    }
    return true;
  } else {
    return false;
  }
}

void ServerWorker::sendByteArray(const QByteArray &byteArray) {
  SerializeSize size;
  quint64 json_size = size(byteArray);

  QDataStream socketStream(m_serverSocket);
  socketStream.setVersion(QDataStream::Qt_5_7);
  socketStream << json_size << byteArray;
}

void ServerWorker::sendJson(const QJsonObject &json) {
  const QByteArray jsonData = QJsonDocument(json).toJson();
  sendByteArray(jsonData);
}

QString ServerWorker::getNickname() { return nickname; }

QString ServerWorker::getUsername() { return username; }

void ServerWorker::setUsername(const QString &username) {
  this->username = username;
}

void ServerWorker::setNickname(const QString &nickname) {
  this->nickname = nickname;
}

void ServerWorker::clearNickname() { nickname.clear(); }

void ServerWorker::disconnectFromClient() {
  m_serverSocket->disconnectFromHost();
}

void ServerWorker::onReadyRead() {
  onReadyRead_helper(m_serverSocket, m_received_data, m_exptected_json_size,
                     m_buffer, *this);
}

QString ServerWorker::getFilename() { return filename; }

void ServerWorker::setFilename(const QString &filename) {
  this->filename = filename;
}

void ServerWorker::closeFile() { this->filename.clear(); }
