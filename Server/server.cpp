#include "server.h"
#include "serverworker.h"
#include <QThread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>

Server::Server(QObject *parent,Database* db)
    : QTcpServer(parent)
    , m_idealThreadCount(qMax(QThread::idealThreadCount(), 1))  //numero ideale di thread in  base al numero di core del processore
    , db(db)
{
    m_availableThreads.reserve(m_idealThreadCount); //pool di thread disponibili: ogni thread gestisce un certo numero di client
    m_threadsLoad.reserve(m_idealThreadCount);     //vettore parallelo al pool di thread per ...
}

Server::~Server()
{
    for (QThread *singleThread : m_availableThreads) {
        singleThread->quit();
        singleThread->wait();
    }
}

//Override from QTcpServer. This gets executed every time a client attempts a connection with the server
void Server::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker;
    //Sets the socket descriptor this server should use when listening for incoming connections to socketDescriptor. Returns true if the socket is set successfully; otherwise returns false.
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
    worker->moveToThread(m_availableThreads.at(threadIdx)); //asssegnazione client al thread scelto.

    //appena il thread finisce, inserisce nella coda degli eventi la cancellazione del worker...domanda:viene tolto anche dal vettore?
    connect(m_availableThreads.at(threadIdx), &QThread::finished, worker, &QObject::deleteLater);
    //viene invocata userDIsconnected quando la connessione viene chiusa dal client.
    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&Server::userDisconnected, this, worker, threadIdx));

    //ONLY FOR NOTIFICATION
    connect(worker, &ServerWorker::error, this, std::bind(&Server::userError, this, worker));

    connect(worker, &ServerWorker::jsonReceived, this, std::bind(&Server::jsonReceived, this, worker, std::placeholders::_1));

    connect(this, &Server::stopAllClients, worker, &ServerWorker::disconnectFromClient);
    m_clients.append(worker);
    qDebug() << "New client Connected";
}
void Server::sendJson(ServerWorker *destination, const QJsonObject &message)
{
    Q_ASSERT(destination);
    QTimer::singleShot(0, destination, std::bind(&ServerWorker::sendJson, destination, message));
}

bool Server::tryConnectionToDatabase()
{
    return db->checkConnection();
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
    qDebug().nospace() << "JSON received " << QString::fromUtf8(QJsonDocument(json).toJson());
    if (sender->getNickname().isEmpty())
        return jsonFromLoggedOut(sender, json);
//    jsonFromLoggedIn(sender, json);
}

//rimuove client disconnesso e notifica
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
    Q_UNUSED(sender) // Indicates to the compiler that the parameter with the specified name is not used in the body of a function.
    qDebug().nospace() << "Error from " << sender->getNickname();
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

    if (typeVal.toString().compare(QLatin1String("signup"), Qt::CaseInsensitive) == 0){
        QJsonObject message=this->signup(docObj);
        this->sendJson(sender,message);
    }
    else if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) == 0){
        QJsonObject message=this->login(docObj);
        this->sendJson(sender,message);
    }



   /*
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
*/
}

QJsonObject Server::signup(const QJsonObject &doc){
    const QJsonValue user = doc.value(QLatin1String("username"));
    QJsonObject message;
    message["type"] = QStringLiteral("signup");

    if (user.isNull() || !user.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong username format");
        return message;
    }
    const QString username = user.toString().simplified();
    if (username.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty username");
        return message;
    }
    const QJsonValue pass = doc.value(QLatin1String("password"));
    if (pass.isNull() || !pass.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong password format");
        return message;
    }
    const QString password = pass.toString().simplified();
    if (password.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty password");
        return message;
    }
    if (this->db->signup(username,password) == DB_QUERY_ERROR){
        message["success"] = false;
        message["reason"] = QStringLiteral("Database error");
        return message;
    }

    message["success"] = true;
    return message;

}


QJsonObject Server::login(const QJsonObject &doc){
    const QJsonValue user = doc.value(QLatin1String("username"));
    QJsonObject message;
    message["type"] = QStringLiteral("login");

    if (user.isNull() || !user.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong username format");
        return message;
    }
    const QString username = user.toString().simplified();
    if (username.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty username");
        return message;
    }
    const QJsonValue pass = doc.value(QLatin1String("password"));
    if (pass.isNull() || !pass.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong password format");
        return message;
    }
    const QString password = pass.toString().simplified();
    if (password.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty password");
        return message;
    }
    int r=this->db->login(username,password);
    if (r == SUCCESS){
        message["success"] = true;
        return message;
    }
    else if (r == NON_EXISTING_USER){
        message["success"] = false;
        message["reason"] = QStringLiteral("No account found for this username");
        return message;
    }
    else if (r == WRONG_PASSWORD){
        message["success"] = false;
        message["reason"] = QStringLiteral("Password is wrong");
        return message;
    }
    else { // DB_QUERY_ERROR
        message["success"] = false;
        message["reason"] = QStringLiteral("Database error");
        return message;
    }
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


