#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QVector>
#include <QMap>
#include <QSslKey>
#include <QSslCertificate>
#include "database.h"
#ifndef Q_MOC_RUN
#include "mongo.h"
#endif

class QThread;
class ServerWorker;
class QJsonObject;

#define IMAGES_PATH "/profile_images"
#define DOCUMENTS_PATH "/user_documents"
#define SAVE_INTERVAL_SEC 5 // saving interval in seconds

class Server : public QTcpServer
{
	Q_OBJECT
	Q_DISABLE_COPY(Server)

public:
	Server(QObject *parent = nullptr,Database* db=0);
	~Server() override;
	bool tryConnectionToDatabase();

protected:
	void incomingConnection(qintptr socketDescriptor) override;

private:
	QSslKey key;
	QSslCertificate cert;
	const int m_idealThreadCount;
	QVector<QThread *> m_availableThreads;
	QVector<int> m_threadsLoad;
	QVector<ServerWorker *> m_clients;
	Database* db;
	// <filename, list_of_workers>
	QMap<QString,QList<ServerWorker*>*>* mapFileWorkers;
	// <filename, map_of_symbols>
	QMap<QString,QMap<QString, QJsonObject>*> symbols_list;
	// <filename, changed>
	QMap<QString, bool> changed;
    Mongo mongo_db;

private slots:
	void broadcast(const QJsonObject &message, ServerWorker *exclude);
	void broadcastByteArray(const QJsonObject &message_broadcast,const QByteArray &bArray,ServerWorker *sender);
	void jsonReceived(ServerWorker *sender, const QJsonObject &doc);
	void userDisconnected(ServerWorker *sender, int threadIdx);
	//    void userError(ServerWorker *sender);

public slots:
	void stopServer();

private:
	void jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &doc);
	QJsonObject signup(ServerWorker *sender,const QJsonObject &doc);
	QJsonObject login(ServerWorker *sender,const QJsonObject &doc);
	QJsonObject updateNick(ServerWorker *sender,const QJsonObject &doc);
	QJsonObject updatePass(const QJsonObject &doc);
	QJsonObject checkOldPass(const QJsonObject &doc);
	QJsonObject getFiles(const QJsonObject &doc, bool shared);
	QJsonObject getFilenameFromSharedLink(const QJsonObject& doc, const QString& user);
	QJsonObject createNewFile(const QJsonObject &doc, ServerWorker *sender);
	QJsonObject sendFile(const QJsonObject &doc, ServerWorker *sender, QVector<QByteArray>& v);
	QJsonObject closeFile(const QJsonObject &doc, ServerWorker *sender);
	QByteArray createByteArrayJsonImage(QJsonObject &message,QVector<QByteArray> &v);
	bool udpateSymbolListAndCommunicateDisconnection(QString filename, ServerWorker* sender);
	static QString fromJsonArraytoString(const QJsonArray& data);

	void jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &doc);
	void sendJson(ServerWorker *destination, const QJsonObject &message);
	void sendByteArray(ServerWorker *sender,const QByteArray &toSend);
	void sendProfileImage(ServerWorker *destination);
	void saveFile();

signals:
	void logMessage(const QString &msg);
	void stopAllClients();
};

#endif // SERVER_H
