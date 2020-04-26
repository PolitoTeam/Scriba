#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QSslSocket>
#include <QReadWriteLock>
#include "../Utility/serializesize.h"

class QJsonObject;
class ServerWorker : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ServerWorker)
public:
	explicit ServerWorker(QObject *parent = nullptr);
	virtual bool setSocketDescriptor(qintptr socketDescriptor,QSslKey key,QSslCertificate cert);
	void sendJson(const QJsonObject &json);
	void sendByteArray(const QByteArray &byteArray);
	void sendProfileImages(QVector<QByteArray> &v);
	void sendProfileImage();
	QString getNickname();
	QString getUsername();
	void setNickname(const QString &nickname);
	void clearNickname();
	void setUsername(const QString &username);
	QString getFilename();
	void setFilename(const QString& filename);
	void closeFile();
    //void setColor(int color);

public slots:
	void disconnectFromClient();
	void sslErrors(const QList<QSslError> &errors);
    void onReadyRead();
    bool parseJson();

private slots:
    //void receiveJson();

signals:
	void jsonReceived(const QJsonObject &jsonDoc);
	void disconnectedFromClient();
	void error();
	void logMessage(const QString &msg);
    void signingUp_updatingImage(const QByteArray &jsonDoc);

private:
	QSslSocket *m_serverSocket;
	QString username;
	QString nickname;
	QString shared_link;
	QString image_name;
	QString filename;
    //int color_rgb;
    quint64 m_exptected_json_size = 0;
    QByteArray m_received_data;
    QBuffer m_buffer;

    void extract_content_size();

};

#endif // SERVERWORKER_H
