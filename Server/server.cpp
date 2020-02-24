#include "server.h"
#include "serverworker.h"
#include "colors.h"
#include "../Client/CRDT.h"
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
//    qDebug()<<"Numero di thread: "<<m_idealThreadCount<<endl;

    mapFileWorkers=new QMap<QString,QList<ServerWorker*>*>();

    // create folder to store profile images
    QString profile_images_path = QDir::currentPath() + IMAGES_PATH;
    QDir dir_images(profile_images_path);
    if (!dir_images.exists()){
        qDebug().nospace() << "Folder " << profile_images_path << " created";
        dir_images.mkpath(".");
    }

    // create folder to store documents
    QString user_documents_path = QDir::currentPath() + DOCUMENTS_PATH;
//    qDebug() << "document path: " << user_documents_path;
    QDir dir_documents(user_documents_path);
    if (!dir_documents.exists()){
        qDebug().nospace() << "Folder " << user_documents_path << " created";
        dir_documents.mkpath(".");
    }

//    DEBUG
//    QVector<QPair<QString, QString>> a;
//    db->getFiles("b@b", a);

    // testing the timer TODO: use slot to save all files
//    QTimer *timer = new QTimer(this);
//    connect(timer, &QTimer::timeout, [=]() { qDebug()<<"Timer!"<<endl;});
//    timer->start(1000 * SAVE_INTERVAL_SEC);
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
//    qDebug()<<"Client assegnato al thread: "<<threadIdx<<endl;
    worker->moveToThread(m_availableThreads.at(threadIdx)); //asssegnazione client al thread scelto.

    connect(m_availableThreads.at(threadIdx), &QThread::finished,[=]() { qDebug()<<"Thread: "<<threadIdx<<" terminato!"<<endl;});
    //appena il thread finisce, inserisce nella coda degli eventi la cancellazione del worker...domanda:viene tolto anche dal vettore?
    connect(m_availableThreads.at(threadIdx), &QThread::finished, worker, &QObject::deleteLater);
    //viene invocata userDIsconnected quando la connessione viene chiusa dal client.
    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&Server::userDisconnected, this, worker, threadIdx));

    //ONLY FOR NOTIFICATION
//    connect(worker, &ServerWorker::error, this, std::bind(&Server::userError, this, worker));

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

void Server::sendProfileImage(ServerWorker *destination)
{
    Q_ASSERT(destination);
    QTimer::singleShot(0, destination, std::bind(&ServerWorker::sendProfileImage, destination));
}

bool Server::tryConnectionToDatabase()
{
    return db->checkConnection();
}

void Server::broadcast(const QJsonObject &message, ServerWorker *exclude)
{
    qDebug() << "BROADCAST";
    for (ServerWorker *worker : m_clients) {
        Q_ASSERT(worker);
        if (worker == exclude)
            continue;
        sendJson(worker, message);
    }
}

void Server::jsonReceived(ServerWorker *sender, const QJsonObject &json)
{
//    qDebug() << json;
    if (sender->getNickname().isEmpty())
        return jsonFromLoggedOut(sender, json);
    else
        jsonFromLoggedIn(sender, json);
}

//rimuove client disconnesso e notifica
void Server::userDisconnected(ServerWorker *sender, int threadIdx)
{
    --m_threadsLoad[threadIdx];
    m_clients.removeAll(sender);

    if (!sender->getFilename().isNull() || !sender->getFilename().isNull()){
        mapFileWorkers->value(sender->getFilename())->removeOne(sender);
        if (mapFileWorkers->value(sender->getFilename())->isEmpty()){
            delete mapFileWorkers->value(sender->getFilename());
            mapFileWorkers->remove(sender->getFilename());
        }
    }

    // TODO: when is the message sent??
    const QString userName = sender->getNickname();
    if (!userName.isEmpty()) {
        QJsonObject disconnectedMessage;
        disconnectedMessage["type"] = QStringLiteral("userdisconnected");
        disconnectedMessage["username"] = userName;
        qDebug() << userName << " disconnected";
        sender->clearNickname();
    }
    sender->deleteLater();
}

//void Server::userError(ServerWorker *sender)
//{
//    Q_UNUSED(sender) // Indicates to the compiler that the parameter with the specified name is not used in the body of a function.
//    qDebug().nospace() << "Error from " << sender->getNickname();
//}

void Server::stopServer()
{
    emit stopAllClients();
    close();
}

void Server::jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &docObj)
{
    qDebug()<<"Json from loggeoud thread: "<<QThread::currentThreadId()<<endl;
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
        return;

    if (typeVal.toString().compare(QLatin1String("signup"), Qt::CaseInsensitive) == 0){
        QJsonObject message=this->signup(sender,docObj);
        this->sendJson(sender,message);
    }
    else if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) == 0){
        QJsonObject message=this->login(sender,docObj);
        if (message.value(QLatin1String("success")).toBool() == true)
            this->sendProfileImage(sender);
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

QJsonObject Server::signup(ServerWorker *sender,const QJsonObject &doc){
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
    DatabaseError result = this->db->signup(username,password);
    if (result == CONNECTION_ERROR || result == QUERY_ERROR){
        message["success"] = false;
        message["reason"] = QStringLiteral("Database error");
        return message;
    }
    if (result == ALREADY_EXISTING_USER){
        message["success"] = false;
        message["reason"] = QStringLiteral("The username already exists");
        return message;
    }
    message["success"] = true;
    return message;

}


QJsonObject Server::login(ServerWorker *sender,const QJsonObject &doc){
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
    QString nickname;
    int r=this->db->login(username,password,nickname);
    if (r == SUCCESS){
        message["success"] = true;
        message["username"]=username;
        message["nickname"]=nickname;
        sender->setUsername(username);
        sender->setNickname(nickname);
        return message;
    }
    else if (r == NON_EXISTING_USER){
        message["success"] = false;
        message["reason"] = QStringLiteral("No account found for this username");
        return message;
    }
    else if (r == WRONG_PASSWORD){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong password");
        return message;
    }
    else { // QUERY_ERROR or CONNECTION_ERROR
        message["success"] = false;
        message["reason"] = QStringLiteral("Database error");
        return message;
    }
}

void Server::jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &docObj)
{
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
        return;

    if (typeVal.toString().compare(QLatin1String("nickname"), Qt::CaseInsensitive) == 0){
        QJsonObject message=this->updateNick(docObj);
        this->sendJson(sender,message);
    }
    if (typeVal.toString().compare(QLatin1String("password"), Qt::CaseInsensitive) == 0){
        QJsonObject message=this->updatePass(docObj);
        this->sendJson(sender,message);
    }
    if (typeVal.toString().compare(QLatin1String("check_old_password"), Qt::CaseInsensitive) == 0){
        QJsonObject message = this->checkOldPass(docObj);
        this->sendJson(sender,message);
    }

    if (typeVal.toString().compare(QLatin1String("list_files"), Qt::CaseInsensitive) == 0){
        QJsonObject message = this->getFiles(docObj);
        this->sendJson(sender,message);
    }

    if (typeVal.toString().compare(QLatin1String("operation"), Qt::CaseInsensitive) == 0){
        QJsonObject symbol = docObj["symbol"].toObject();
        int operation_type = docObj["operation_type"].toInt();

        // update symbols in server memory and broadcast operation to other editors
        // TODO: handle changes (only deletion and insertion)
        if (operation_type == INSERT) {
            qDebug() << "insertion";
            symbols_list.append(symbol);
        } else if (operation_type == DELETE) {
            qDebug() << "deletion";
            symbols_list.removeOne(symbol);
        } else {

        }
        broadcast(docObj, sender);

        // TODO: store on disk -> CHANGE to save every X minutes
        // TODO: wrong, shouldn't be username but author!
        QString filePath = QDir::currentPath() + DOCUMENTS_PATH + "/" + sender->getFilename();
        qDebug() << "filename " << sender->getFilename();
//        if (QFile::exists(filePath))
//        {
//            QFile::remove(filePath);
//        }
//        qDebug() << filePath;
        QFile file(filePath);
        file.open(QIODevice::ReadWrite | QIODevice::Truncate);
        QJsonArray symbols_json;
        for (QJsonObject symbol : symbols_list) {
            symbols_json.append(symbol);
        }

        qDebug() << symbols_json;
        file.write(QJsonDocument(symbols_json).toJson());
        file.close();
    }

    if (typeVal.toString().compare(QLatin1String("new_file"), Qt::CaseInsensitive) == 0){
        QJsonObject message = this->createNewFile(docObj,sender);
        this->sendJson(sender,message);
    }
    if (typeVal.toString().compare(QLatin1String("file_to_open"), Qt::CaseInsensitive) == 0){
        // send symbols in file to client
        QJsonObject message = this->sendFile(docObj,sender);
        this->sendJson(sender,message);

        // store symbols in server memory
        foreach (const QJsonValue & symbol, message["content"].toArray()) {
            symbols_list.append(symbol.toObject());
        }
    }
    if (typeVal.toString().compare(QLatin1String("close"), Qt::CaseInsensitive) == 0){
        QJsonObject message = this->closeFile(docObj,sender);
        this->sendJson(sender,message);
    }
}

QJsonObject Server::updateNick(const QJsonObject &doc){
    const QJsonValue user = doc.value(QLatin1String("username"));
    QJsonObject message;
    message["type"] = QStringLiteral("nickname");

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
    const QJsonValue nick = doc.value(QLatin1String("nickname"));
    if (nick.isNull() || !nick.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong nickname format");
        return message;
    }
    const QString nickname = nick.toString().simplified();
    if (nickname.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty nickname");
        return message;
    }
//    qDebug()<<"nickname: "<<nickname;
    DatabaseError result = this->db->updateNickname(username,nickname);
    if (result == CONNECTION_ERROR || result == QUERY_ERROR){
        message["success"] = false;
        message["reason"] = QStringLiteral("Database error");
        return message;
    }
    if (result == NON_EXISTING_USER){
        message["success"] = false;
        message["reason"] = QStringLiteral("The username doens't exists");
        return message;
    }

    message["success"] = true;
    return message;
}


QJsonObject Server::updatePass(const QJsonObject &doc){
    const QJsonValue user = doc.value(QLatin1String("username"));
    QJsonObject message;
    message["type"] = QStringLiteral("password");

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

    const QJsonValue oldpass = doc.value(QLatin1String("oldpass"));
    if (oldpass.isNull() || !oldpass.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong oldpassword format");
        return message;
    }
    const QString oldpassword = oldpass.toString().simplified();
    if (oldpassword.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty old password");
        return message;
    }

    const QJsonValue newpass = doc.value(QLatin1String("newpass"));
    if (newpass.isNull() || !newpass.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong new password format");
        return message;
    }
    const QString newpassword = newpass.toString().simplified();
    if (newpassword.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty new password");
        return message;
    }

    DatabaseError result = this->db->updatePassword(username,oldpassword,newpassword);

    if (result == NON_EXISTING_USER){
        message["success"] = false;
        message["reason"] = QStringLiteral("The username doens't exists");
        return message;
    }
    else if (result == NON_EXISTING_USER){
        message["success"] = false;
        message["reason"] = QStringLiteral("No account found for this username");
        return message;
    }
    else if (result == WRONG_PASSWORD){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong password");
        return message;
    }
    else{

        message["success"] = true;
        return message;

    }
}

QJsonObject Server::checkOldPass(const QJsonObject &doc){
    QJsonObject message;
    message["type"] = QStringLiteral("old_password_checked");

    const QJsonValue user = doc.value(QLatin1String("username"));
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

    const QJsonValue old_pass = doc.value(QLatin1String("old_password"));
    if (old_pass.isNull() || !old_pass.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong password format");
        return message;
    }
    const QString old_password = old_pass.toString().simplified();
    if (old_password.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty password");
        return message;
    }

    DatabaseError result = this->db->checkOldPassword(username, old_password);
    if (result == CONNECTION_ERROR || result == QUERY_ERROR){
        message["success"] = false;
        message["reason"] = QStringLiteral("Database error");
        return message;
    } else if (result == NON_EXISTING_USER){
        message["success"] = false;
        message["reason"] = QStringLiteral("The username doens't exists");
        return message;
    } else if (result == WRONG_PASSWORD) {
        message["success"] = false;
        message["reason"] = QStringLiteral("The old password doens't match");
        return message;
    }

    message["success"] = true;
    return message;
}


QJsonObject Server::getFiles(const QJsonObject &doc){
    const QJsonValue user = doc.value(QLatin1String("username"));
    QJsonObject message;
    message["type"] = QStringLiteral("list_files");

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

    QVector<QPair<QString, QString>> files;
    DatabaseError result = this->db->getFiles(username, files);
    if (result == CONNECTION_ERROR || result == QUERY_ERROR){
        message["success"] = false;
        message["reason"] = QStringLiteral("Database error.");
        return message;
    }
    if (result == NO_FILES_AVAILABLE){
        message["success"] = false;
        message["reason"] = QStringLiteral("You don't have files yet.");
        return message;
    }

    QJsonArray array_files;

    QVector<QPair<QString, QString>>::iterator i;
    for (i = files.begin(); i != files.end(); ++i){
        // use initializer list to construct QJsonObject
        auto data = QJsonObject({
            qMakePair(QString("name"), QJsonValue(i->first)),
            qMakePair(QString("owner"), QJsonValue(i->second))
        });

        array_files.push_back(QJsonValue(data));
    }


    message["success"] = true;
    message["files"]=array_files;
    auto d = QJsonDocument(message);
//    qDebug()<< d.toJson().constData()<<endl;
    return message;
}

QJsonObject Server::createNewFile(const QJsonObject &doc, ServerWorker *sender)
{
    QJsonObject message;
    message["type"] = QStringLiteral("new_file");

    const QJsonValue user = doc.value(QLatin1String("author"));
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

    const QJsonValue name = doc.value(QLatin1String("filename"));
    if (name.isNull() || !name.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong filename format");
        return message;
    }
    const QString filename = name.toString().simplified();
    if (filename.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty old password");
        return message;
    }

    QString sharedLink;
    DatabaseError result = this->db->newFile(username, filename, sharedLink);
    if (result == CONNECTION_ERROR || result == QUERY_ERROR){
        message["success"] = false;
        message["reason"] = QStringLiteral("Database error.");
        return message;
    }
    if (result == ALREADY_EXISTING_FILE){
        message["success"] = false;
        message["reason"] = QStringLiteral("This file already exists. Please enter a new filename.");
        return message;
    }

    // save the name of the file used by the worker
    sender->setFilename(filename + "," + username);

    // create empty file for the specified user
    QString documents_path = QDir::currentPath() + DOCUMENTS_PATH + "/" + filename + "," + username;
    QFile file(documents_path);
    if (file.exists()) {
        throw new std::runtime_error("File shouldn't already exist.");
    }
    file.open(QIODevice::WriteOnly);

    // add current worker to the map <file, list_of_workers>
    QList<ServerWorker*>* list=new QList<ServerWorker*>();
    list->append(sender);
    mapFileWorkers->insert(filename + "," + username,list);
    qDebug() << mapFileWorkers;

//    symbols_list.insert()
    message["success"] = true;
    message["shared_link"] = sharedLink;
    return message;
}

QJsonObject Server::sendFile(const QJsonObject &doc, ServerWorker *sender){
    QJsonObject message;
    message["type"] = QStringLiteral("file_to_open");

    const QJsonValue name = doc.value(QLatin1String("filename"));
    if (name.isNull() || !name.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong filename format");
        return message;
    }
    const QString filename = name.toString().simplified();
    if (filename.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty filename");
        return message;
    }

    int pos = filename.lastIndexOf(QChar(','));
    QString file=filename.left(pos);
    QString author = filename.right(filename.length() - pos - 1);

    sender->setFilename(filename);
    int index = 0;
//    qDebug()<<"sender->setFilename() "<<file<<endl;

    if (mapFileWorkers->contains(filename)){
        index = mapFileWorkers->value(filename)->size();
        mapFileWorkers->value(filename)->append(sender);
    }
    else{
        QList<ServerWorker*>* list=new QList<ServerWorker*>();
        list->append(sender);
        mapFileWorkers->insert(filename,list);
    }

//    qDebug() << "index " << index;
    int color = color_palette[index % color_palette.size()];
    sender->setColor(color);

    QList<ServerWorker*>* list=mapFileWorkers->value(filename);
    QJsonArray array_users;

    //per ora manda il contenuto del file insieme alla lista di chi è connesso;
    //d agestire il caso in cui le connessioni cambiano mentre o dopo il messaggio è inviato


    for (int i = 0; i <list->count(); i++){
        // use initializer list to construct QJsonObject
        if (sender->getUsername()==list->at(i)->getUsername())
            continue;
        auto data = QJsonObject({
            qMakePair(QString("username"), QJsonValue(list->at(i)->getUsername())),
            qMakePair(QString("nickname"), QJsonValue(list->at(i)->getNickname()))
        });

        array_users.push_back(QJsonValue(data));
    }

    // read symbols from file...
    QString filePath = QDir::currentPath() + DOCUMENTS_PATH + "/" + filename;
    QFile f(filePath);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray json_data = f.readAll();
    f.close();
    // ...and check for errors in the format
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(json_data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        message["success"] = false;
        message["reason"] = QStringLiteral("Json parsing error");
    }
    if (!document.isArray()) {
        message["success"] = false;
        message["reason"] = QStringLiteral("File content different form json array");
    }
    QJsonArray symbols = document.array();

    // retrieve shared link
    QString sharedLink;
    db->getSharedLink(author, file, sharedLink);

    message["success"] = true;
    message["content"] = symbols;
    message["filename"]= filename;
    message["users"] = array_users;
//    message["shared_link"] = "fake_shared_link";
    message["shared_link"] = sharedLink;
    message["color"] = color;

    auto d = QJsonDocument(message);
//    qDebug()<< d.toJson().constData()<<endl;
    return message;
}

QJsonObject Server::closeFile(const QJsonObject &doc, ServerWorker *sender){

    QJsonObject message;
    message["type"] = QStringLiteral("close");

    const QJsonValue name = doc.value(QLatin1String("filename"));
    if (name.isNull() || !name.isString()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Wrong filename format");
        return message;
    }
    const QString filename = name.toString().simplified();
    if (filename.isEmpty()){
        message["success"] = false;
        message["reason"] = QStringLiteral("Empty old password");
        return message;
    }

    const QJsonValue user = doc.value(QLatin1String("username"));

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

    // remove client from list of clients using current file
    if (mapFileWorkers->contains(filename)){
        sender->closeFile();
        mapFileWorkers->value(filename)->removeOne(sender);
        if (mapFileWorkers->value(filename)->isEmpty()){
            delete mapFileWorkers->value(filename);
            mapFileWorkers->remove(filename);
        }
    } else{
        message["success"] = false;
        message["reason"] = QStringLiteral("File not exist");
        return message;
    }

    // empty symbol list
    symbols_list.clear();

    QJsonObject message_broadcast;
    message_broadcast["type"] = QStringLiteral("disconnection");
    message_broadcast["filename"]=filename;
    message_broadcast["user"]=username;
    this->broadcast(message_broadcast,sender);

    message["success"] = true;
    return message;
}
