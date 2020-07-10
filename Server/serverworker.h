#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include "../Utility/serializesize.h"
#include <QObject>
#include <QReadWriteLock>
#include <QSslSocket>

class QJsonObject;
class ServerWorker : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(ServerWorker)
public:
  explicit ServerWorker(QObject *parent = nullptr);
  virtual bool setSocketDescriptor(qintptr socketDescriptor, QSslKey key,
                                   QSslCertificate cert);
  void sendJson(const QJsonObject &json);
  void sendByteArray(const QByteArray &byteArray);
  QString getNickname();
  QString getUsername();
  void setNickname(const QString &nickname);
  void clearNickname();
  void setUsername(const QString &username);
  QString getFilename();
  void setFilename(const QString &filename);
  void closeFile();

public slots:
  void disconnectFromClient();
  void sslErrors(const QList<QSslError> &errors);
  void onReadyRead();
  bool parseJson();

signals:
  void jsonReceived(const QJsonObject &jsonDoc);
  void disconnectedFromClient();
  void error();
  void logMessage(const QString &msg);
  void byte_array_received(const QByteArray &jsonDoc);

private:
  QSslSocket *m_serverSocket;
  QString username;
  QString nickname;
  QString filename;
  quint64 m_exptected_json_size = 0;
  QByteArray m_received_data;
  QBuffer m_buffer;

  void extract_content_size();
};

#endif // SERVERWORKER_H
