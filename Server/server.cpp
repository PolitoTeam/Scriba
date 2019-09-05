#include "server.h"
#include "serverworker.h"
#include <QThread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>

Server::Server(QObject *parent)
    : QTcpServer(parent)
    , m_idealThreadCount(qMax(QThread::idealThreadCount(), 1))
{
    m_availableThreads.reserve(m_idealThreadCount);
    m_threadsLoad.reserve(m_idealThreadCount);
}

Server::~Server()
{
    for (QThread *singleThread : m_availableThreads) {
        singleThread->quit();
        singleThread->wait();
    }
}
void Server::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker;
    if (!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }
    int threadIdx = m_availableThreads.size();
    if (threadIdx < m_idealThreadCount) { //we can add a new thread
        m_availableThreads.append(new QThread(this));
        m_threadsLoad.append(1);
        m_availableThreads.last()->start();
    } else {
        // find the thread with the least amount of clients and use it
        threadIdx = std::distance(m_threadsLoad.cbegin(), std::min_element(m_threadsLoad.cbegin(), m_threadsLoad.cend()));
        ++m_threadsLoad[threadIdx];
    }
    worker->moveToThread(m_availableThreads.at(threadIdx));
    connect(m_availableThreads.at(threadIdx), &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&Server::userDisconnected, this, worker, threadIdx));
    connect(worker, &ServerWorker::error, this, std::bind(&Server::userError, this, worker));
    connect(worker, &ServerWorker::jsonReceived, this, std::bind(&Server::jsonReceived, this, worker, std::placeholders::_1));
    connect(this, &Server::stopAllClients, worker, &ServerWorker::disconnectFromClient);
    m_clients.append(worker);
    emit logMessage("New client Connected");
}
void Server::sendJson(ServerWorker *destination, const QJsonObject &message)
{
    Q_ASSERT(destination);
    QTimer::singleShot(0, destination, std::bind(&ServerWorker::sendJson, destination, message));
}
//void Server::broadcast(const QJsonObject &message, ServerWorker *exclude)
//{
//    for (ServerWorker *worker : m_clients) {
//        Q_ASSERT(worker);
//        if (worker == exclude)
//            continue;
//        sendJson(worker, message);
//    }
//}

void Server::jsonReceived(ServerWorker *sender, const QJsonObject &json)
{
    Q_ASSERT(sender);
//    emit logMessage("JSON received " + QString::fromUtf8(QJsonDocument(json).toJson()));
    if (sender->getNickname().isEmpty())
        return jsonFromLoggedOut(sender, json);
//    jsonFromLoggedIn(sender, json);
}

void Server::userDisconnected(ServerWorker *sender, int threadIdx)
{
    --m_threadsLoad[threadIdx];
    m_clients.removeAll(sender);
    const QString userName = sender->getNickname();
    if (!userName.isEmpty()) {
        QJsonObject disconnectedMessage;
        disconnectedMessage["type"] = QStringLiteral("userdisconnected");
        disconnectedMessage["username"] = userName;
//        broadcast(disconnectedMessage, nullptr);
        qDebug() << userName << " disconnected";
    }
    sender->deleteLater();
}

void Server::userError(ServerWorker *sender)
{
    Q_UNUSED(sender)
    emit logMessage("Error from " + sender->getNickname());
}

void Server::stopServer()
{
    emit stopAllClients();
    close();
}

void Server::jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &docObj)
{
    Q_ASSERT(sender);
    qDebug().noquote() << QString::fromUtf8(QJsonDocument(docObj).toJson(QJsonDocument::Compact));

    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
        return;
    if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) != 0)
        return;
    const QJsonValue user = docObj.value(QLatin1String("username"));
    if (user.isNull() || !user.isString())
        return;
    const QString username = user.toString().simplified();
    if (username.isEmpty())
        return;
//    for (ServerWorker *worker : qAsConst(m_clients)) {
//        if (worker == sender)
//            continue;
//        if (worker->userName().compare(newUserName, Qt::CaseInsensitive) == 0) {
//            QJsonObject message;
//            message["type"] = QStringLiteral("login");
//            message["success"] = false;
//            message["reason"] = QStringLiteral("duplicate username");
//            sendJson(sender, message);
//            return;
//        }
//    }
//    sender->setUserName(newUserName);

    const QJsonValue pass = docObj.value(QLatin1String("password"));
    if (pass.isNull() || !pass.isString())
        return;
    const QString password = pass.toString().simplified();
    if (password.isEmpty())
        return;

    // login failed
    if (username != "test" || password != "test") {
        QJsonObject message;
        message["type"] = QStringLiteral("login");
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong username/password");
        qDebug().noquote() << QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
        sendJson(sender, message);
        return;
    }

    QJsonObject successMessage;
    successMessage["type"] = QStringLiteral("login");
    successMessage["success"] = true;
    qDebug().noquote() << QString::fromUtf8(QJsonDocument(successMessage).toJson(QJsonDocument::Compact));
    sendJson(sender, successMessage);
//    QJsonObject connectedMessage;
//    connectedMessage["type"] = QStringLiteral("newuser");
//    connectedMessage["username"] = newUserName;
//    broadcast(connectedMessage, sender);
}

//void Server::jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &docObj)
//{
//    Q_ASSERT(sender);
//    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
//    if (typeVal.isNull() || !typeVal.isString())
//        return;
//    if (typeVal.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) != 0)
//        return;
//    const QJsonValue textVal = docObj.value(QLatin1String("text"));
//    if (textVal.isNull() || !textVal.isString())
//        return;
//    const QString text = textVal.toString().trimmed();
//    if (text.isEmpty())
//        return;
//    QJsonObject message;
//    message["type"] = QStringLiteral("message");
//    message["text"] = text;
//    message["sender"] = sender->userName();
//    broadcast(message, sender);
//}


