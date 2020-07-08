#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QVector>
#include <QMap>
#include <QSslKey>
#include <QSslCertificate>
#include "../Utility/common.h"
#include "mongo.h"
#include "../Utility/symbol.h"

class QThread;
class ServerWorker;
class QJsonObject;

#define IMAGES_PATH "/profile_images"
#define SAVE_INTERVAL_SEC 5 // saving interval in seconds

class Server : public QTcpServer
{
	Q_OBJECT
	Q_DISABLE_COPY(Server)

public:
	Server(QObject *parent=nullptr);
	~Server() override;
	bool tryConnectionToDatabase();
	bool tryConnectionToMongo();

protected:
	void incomingConnection(qintptr socketDescriptor) override;

private slots:
	void broadcast(const QJsonObject &message, ServerWorker *exclude);
	void broadcastByteArray(const QJsonObject &message_broadcast,
							const QByteArray &bArray, ServerWorker *sender);
	void jsonReceived(ServerWorker *sender, const QJsonObject &doc);
	void userDisconnected(ServerWorker *sender, int threadIdx);

public slots:
	void stopServer();

signals:
	void logMessage(const QString &msg);
	void stopAllClients();

private:
	QSslKey key;
	QSslCertificate cert;
	const int m_idealThreadCount;
	QVector<QThread *> m_availableThreads;
	QVector<int> m_threadsLoad;
	QVector<ServerWorker *> m_clients;
	Mongo db;
	// <filename, list_of_workers>
	QMap<QString,QList<ServerWorker*>*>* mapFileWorkers;
	// <filename, map_of_symbols>
    QMap<QString,QMap<QString, Symbol>*> symbols_list;
	// <filename, changed>
	QMap<QString, bool> changed;

	void jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &doc);
    void signup_updateImage(ServerWorker *sender,const QByteArray &doc);
    QJsonObject checkCredentials(ServerWorker *sender,const QJsonObject &doc);
	QJsonObject updateNick(ServerWorker *sender,const QJsonObject &doc);
	QJsonObject updatePass(const QJsonObject &doc);
	QJsonObject checkOldPass(const QJsonObject &doc);
	QJsonObject getFiles(const QJsonObject &doc, bool shared);
	QJsonObject getFilenameFromSharedLink(const QJsonObject& doc,
										  const QString& user);
    QJsonObject checkAlreadyExistingUsername(const QJsonObject &doc);
	QJsonObject createNewFile(const QJsonObject &doc, ServerWorker *sender);
	QJsonObject sendFile(const QJsonObject &doc,
						 ServerWorker *sender, QVector<QByteArray>& v);
	QJsonObject closeFile(const QJsonObject &doc, ServerWorker *sender);
	QByteArray createByteArrayJsonImage(QJsonObject &message,
										QVector<QByteArray> &v);
    QByteArray createByteArrayFileContentImage(QJsonObject &message,QVector<Symbol> &c,QVector<QByteArray> &v);
    void storeSymbolsServerMemory(ServerWorker* sender,QVector<Symbol> array);
	bool udpateSymbolListAndCommunicateDisconnection(QString filename,
													 ServerWorker* sender);
	static QString fromJsonArraytoString(const QJsonArray& data);
    static QString fromVectorIdentifiertoString(const QVector<Identifier>& data);
	void jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &doc);
	void sendJson(ServerWorker *destination, const QJsonObject &message);
	void sendByteArray(ServerWorker *sender,const QByteArray &toSend);
	void saveFile();
};

#endif // SERVER_H
