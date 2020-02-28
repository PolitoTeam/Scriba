#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QHostAddress>
#include <QDebug>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QBuffer>
#include "client.h"
#include "CRDT.h"
#include "symbol.h"

Client::Client(QObject *parent)
    : QObject(parent)
    , m_clientSocket(new QTcpSocket(this))
    , m_loggedIn(false)
{
    // Forward the connected and disconnected signals
    connect(m_clientSocket, &QTcpSocket::connected, this, &Client::connected);
    connect(m_clientSocket, &QTcpSocket::disconnected, this, &Client::disconnected);

    connect(this,&Client::connected,this, []()->void{qDebug()<<"New client Connected";});
    // connect readyRead() to the slot that will take care of reading the data in
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &Client::onReadyRead);

    // Forward the error signal, QOverload is necessary as error() is overloaded, see the Qt docs
    connect(m_clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client::error);

    // Reset the m_loggedIn variable when we disconnec. Since the operation is trivial we use a lambda instead of creating another slot
    connect(m_clientSocket, &QTcpSocket::disconnected, this, [this]()->void{m_loggedIn = false;});

    profile=new QPixmap(":/images/anonymous");
}

void Client::login(const QString &username, const QString &password)
{
    connectToServer(QHostAddress::Any, 1500); //porta da stabilire
    if (m_clientSocket->waitForConnected()) {
        // create a QDataStream operating on the socket
        QDataStream clientStream(m_clientSocket);
        // set the version so that programs compiled with different versions of Qt can agree on how to serialise
        clientStream.setVersion(QDataStream::Qt_5_7);
        // Create the JSON we want to send
        QJsonObject message;
        message["type"] = QStringLiteral("login");
        message["username"] = username;
        //aggiungere cifratura oppure passare a QSSLsocket
        message["password"] = password;
        // send the JSON using QDataStream
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    }
}

void Client::signup(const QString &username, const QString &password)
{
    connectToServer(QHostAddress::Any, 1500); //porta da stabilire
    if (m_clientSocket->waitForConnected()) {
        // create a QDataStream operating on the socket
        QDataStream clientStream(m_clientSocket);
        // set the version so that programs compiled with different versions of Qt can agree on how to serialise
        clientStream.setVersion(QDataStream::Qt_5_7);
        // Create the JSON we want to send
        QJsonObject message;
        message["type"] = QStringLiteral("signup");
        message["username"] = username;
        //aggiungere cifratura oppure passare a QSSLsocket
        message["password"] = password;
        // send the JSON using QDataStream
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);

    }
}

void Client::getFiles(){
    if (m_clientSocket->waitForConnected()) {
        // create a QDataStream operating on the socket
        QDataStream clientStream(m_clientSocket);
        // set the version so that programs compiled with different versions of Qt can agree on how to serialise
        clientStream.setVersion(QDataStream::Qt_5_7);
        // Create the JSON we want to send
        QJsonObject message;
        message["type"] = QStringLiteral("list_files");
        message["username"] = this->username;
        //aggiungere cifratura oppure passare a QSSLsocket
        // send the JSON using QDataStream
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    }
}

void Client::getFilenameFromLink(const QString& sharedLink) {
    if (m_clientSocket->waitForConnected()) {
        // create a QDataStream operating on the socket
        QDataStream clientStream(m_clientSocket);
        // set the version so that programs compiled with different versions of Qt can agree on how to serialise
        clientStream.setVersion(QDataStream::Qt_5_7);
        // Create the JSON we want to send
        QJsonObject message;
        message["type"] = QStringLiteral("filename_from_sharedLink");
        message["sharedLink"] = sharedLink;
        //aggiungere cifratura oppure passare a QSSLsocket
        // send the JSON using QDataStream
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    }
}

void Client::updateNickname(const QString &nickname)
{
    if (m_clientSocket->waitForConnected()) {
        // create a QDataStream operating on the socket
        QDataStream clientStream(m_clientSocket);
        // set the version so that programs compiled with different versions of Qt can agree on how to serialise
        clientStream.setVersion(QDataStream::Qt_5_7);
        // Create the JSON we want to send
        QJsonObject message;
        message["type"] = QStringLiteral("nickname");
        message["username"] = this->username;
        message["nickname"] = nickname;
        //aggiungere cifratura oppure passare a QSSLsocket
        // send the JSON using QDataStream
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);

        this->nickname = nickname;
    }
}

void  Client::updatePassword(const QString &oldpassword,const QString &newpassword){
    if (m_clientSocket->waitForConnected()) {
        // create a QDataStream operating on the socket
        QDataStream clientStream(m_clientSocket);
        // set the version so that programs compiled with different versions of Qt can agree on how to serialise
        clientStream.setVersion(QDataStream::Qt_5_7);
        // Create the JSON we want to send
        QJsonObject message;
        message["type"] = QStringLiteral("password");
        message["username"] = this->username;
        message["oldpass"] = oldpassword;
        message["newpass"] = newpassword;
        //aggiungere cifratura oppure passare a QSSLsocket
        // send the JSON using QDataStream
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    }
}

void Client::checkOldPassword(const QString &old_password)
{
    QDataStream clientStream(m_clientSocket);
    clientStream.setVersion(QDataStream::Qt_5_7);

    QJsonObject message;
    message["type"] = QStringLiteral("check_old_password");
    message["username"] = this->username;
    message["old_password"] = old_password;

    clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
}
//void Client::sendMessage(const QString &text)
//{
//    if (text.isEmpty())
//        return; // We don't send empty messages
//    // create a QDataStream operating on the socket
//    QDataStream clientStream(m_clientSocket);
//    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
//    clientStream.setVersion(QDataStream::Qt_5_7);
//    // Create the JSON we want to send
//    QJsonObject message;
//    message["type"] = QStringLiteral("message");
//    message["text"] = text;
//    // send the JSON using QDataStream
//    clientStream << QJsonDocument(message).toJson();
//}


//Attempts to close the socket. If there is pending data waiting to be written, QAbstractSocket will enter ClosingState and wait until all data has been written.
void Client::disconnectFromHost()
{
    this->username.clear();
    this->nickname.clear();
    this->files.clear();
    m_clientSocket->disconnectFromHost();
}

void Client::jsonReceived(const QJsonObject &docObj)
{
    // actions depend on the type of message
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
        return; // a message with no type was received so we just ignore it
    qDebug().noquote() << QString::fromUtf8(QJsonDocument(docObj).toJson(QJsonDocument::Compact));

    if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) == 0) { //It's a login message
        if (m_loggedIn)
            return; // if we are already logged in we ignore
        // the success field will contain the result of our attempt to login
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
            return; // the message had no success field so we ignore
        const bool loginSuccess = resultVal.toBool();
        if (loginSuccess) {
            const QJsonValue user = docObj.value(QLatin1String("username"));

            if (user.isNull() || !user.isString())
                return;

            const QString username = user.toString().simplified();
            if (username.isEmpty()){
                return;
            }
            const QJsonValue nick = docObj.value(QLatin1String("nickname"));

            if (nick.isNull() || !nick.isString())
                return;

            const QString nickname = nick.toString().simplified();
            if (nickname.isEmpty()){
                return;
            }
            this->username=username;
            this->nickname=nickname;

            m_loggedIn=true;// we logged in succesfully and we notify it via the loggedIn signal
            emit loggedIn();
            return;
        }
        // the login attempt failed, we extract the reason of the failure from the JSON
        // and notify it via the loginError signal
        const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
        emit loginError(reasonVal.toString());
    }
    else if (typeVal.toString().compare(QLatin1String("signup"), Qt::CaseInsensitive) == 0) { //It's a login message
        if (m_loggedIn)
            return; // if we are already logged in we ignore
        // the success field will contain the result of our attempt to login
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
            return; // the message had no success field so we ignore
        const bool signupSuccess = resultVal.toBool();
        if (signupSuccess) {
            // we logged in succesfully and we notify it via the signedUp signal
            emit signedUp();
            return;
        }
        // the signup attempt failed, we extract the reason of the failure from the JSON
        // and notify it via the signupError signal
        const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
        emit signupError(reasonVal.toString());
    } else if (typeVal.toString().compare(QLatin1String("old_password_checked"), Qt::CaseInsensitive) == 0) {
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
            return;
        const bool oldPasswordCheckSuccess = resultVal.toBool();
        if (!oldPasswordCheckSuccess)
            emit wrongOldPassword();
        else
            emit correctOldPassword();
    } else if (typeVal.toString().compare(QLatin1String("operation"), Qt::CaseInsensitive) == 0) {   
        QJsonObject symbol = docObj["symbol"].toObject();
        Symbol s = Symbol::fromJson(symbol);
        qDebug() << s.to_string();

        int operation_type = docObj["operation_type"].toInt();
//        qDebug() << "operation" << operation_type;
        if (operation_type == INSERT) {
//            qDebug() << "INSERT";
            emit remoteInsert(s);
        } else if (operation_type == INSERT){
//            qDebug() << "ERASE";
            emit remoteErase(s);
        }
        else{
            int align_type = docObj["alignment"].toInt();
            emit remoteAlignChange(s,align_type);
        }
    }
    else if (typeVal.toString().compare(QLatin1String("list_files"), Qt::CaseInsensitive) == 0) {
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
            return;
        const bool getFile=resultVal.toBool();
        if (getFile){
            const QJsonValue array= docObj.value(QLatin1String("files"));
            if (array.isNull() || !array.isArray())
                return;
            const QJsonArray array_files=array.toArray();

            this->files.clear();
            foreach (const QJsonValue& v, array_files){
//                qDebug()<<"name: "<<v.toObject().value("name").toString()<<" owner: "<< v.toObject().value("owner").toString()<<endl;
                this->files.push_back(QPair<QString, QString>(v.toObject().value("name").toString(),v.toObject().value("owner").toString()));
            }

            emit filesReceived();
        } else { // error handling
            const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
            emit openFilesError(reasonVal.toString());
        }
    } else if (typeVal.toString().compare(QLatin1String("new_file"), Qt::CaseInsensitive) == 0) {
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
            return;
        const bool success = resultVal.toBool();
        if (success) {
            sharedLink = docObj.value(QLatin1String("shared_link")).toString();
            emit correctNewFile();
        } else {
            const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
            emit wrongNewFile(reasonVal.toString());
        }
    }
    else if (typeVal.toString().compare(QLatin1String("file_to_open"), Qt::CaseInsensitive) == 0) {
            const QJsonValue resultVal = docObj.value(QLatin1String("success"));
            if (resultVal.isNull() || !resultVal.isBool())
                return;
            const bool success = resultVal.toBool();
            if (success) {
                const QJsonValue cont = docObj.value(QLatin1String("content"));
                if (cont.isNull() || !cont.isArray())
                    return;
                const QJsonArray symbols = cont.toArray();
                // read the symbols in the file and parse them into the editor
                foreach (const QJsonValue & symbol, symbols) {
                    Symbol s = Symbol::fromJson(symbol.toObject());
                    emit remoteInsert(s);
                }
                emit moveCursorToEnd();

                const QJsonValue name = docObj.value(QLatin1String("filename"));
                if (name.isNull() || !name.isString())
                    return;
                this->openfile=name.toString();

                const QJsonValue shared_link = docObj.value(QLatin1String("shared_link"));
                if (shared_link.isNull() || !shared_link.isString())
                    return;
                this->sharedLink = shared_link.toString();

                const QJsonValue color= docObj.value(QLatin1String("color"));
                if (color.isNull() || !color.isDouble())
                    return;
                this->cursor_color_rgb = color.toInt();

                const QJsonValue array= docObj.value(QLatin1String("users"));
                if (array.isNull() || !array.isArray())
                    return;
                const QJsonArray array_users=array.toArray();
                QList<QPair<QString,QString>> connected;
                foreach (const QJsonValue& v, array_users){
                    qDebug()<<"username: "<<v.toObject().value("username").toString()<<" nickname: "<< v.toObject().value("nickname").toString()<<endl;
                    connected.append(QPair<QString,QString>(v.toObject().value("username").toString(),v.toObject().value("nickname").toString()));
                }
//                emit contentReceived(cont.toString()); TODO: remove comment
                emit usersConnectedReceived(connected);
                emit correctOpenedFile();
            } else {
                this->openfile.clear();
                const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
                emit wrongListFiles(reasonVal.toString());
            }
        }
    else if (typeVal.toString().compare(QLatin1String("disconnection"), Qt::CaseInsensitive) == 0) {
            const QJsonValue file = docObj.value(QLatin1String("filename"));
            if (file.isNull() || !file.isString())
                return;
            const QJsonValue name = docObj.value(QLatin1String("user"));
            if (name.isNull() || !name.isString())
                return;
            if (!file.toString().compare(this->openfile)){
                emit userDisconnected(name.toString());
            }

        }
    else if (typeVal.toString().compare(QLatin1String("filename_from_sharedLink"), Qt::CaseInsensitive) == 0) {
        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
        if (resultVal.isNull() || !resultVal.isBool())
            return;
        const bool success = resultVal.toBool();
        if (success) {
            QString filename = docObj.value(QLatin1String("filename")).toString();
            this->openFile(filename);
        } else {
            const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
            emit wrongSharedLink(reasonVal.toString());
        }
    }

    /*else if (typeVal.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) == 0) { //It's a chat message
        // we extract the text field containing the chat text
        const QJsonValue textVal = docObj.value(QLatin1String("text"));
        // we extract the sender field containing the username of the sender
        const QJsonValue senderVal = docObj.value(QLatin1String("sender"));
        if (textVal.isNull() || !textVal.isString())
            return; // the text field was invalid so we ignore
        if (senderVal.isNull() || !senderVal.isString())
            return; // the sender field was invalid so we ignore
        // we notify a new message was received via the messageReceived signal
        emit messageReceived(senderVal.toString(), textVal.toString());
    } else if (typeVal.toString().compare(QLatin1String("newuser"), Qt::CaseInsensitive) == 0) { // A user joined the chat
        // we extract the username of the new user
        const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return; // the username was invalid so we ignore
        // we notify of the new user via the userJoined signal
        emit userJoined(usernameVal.toString());
    } else if (typeVal.toString().compare(QLatin1String("userdisconnected"), Qt::CaseInsensitive) == 0) { // A user left the chat
         // we extract the username of the new user
        const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
        if (usernameVal.isNull() || !usernameVal.isString())
            return; // the username was invalid so we ignore
        // we notify of the user disconnection the userLeft signal
        emit userLeft(usernameVal.toString());
    }*/
}

void Client::createNewFile(QString filename)
{
    if (m_clientSocket->waitForConnected()) {
        QDataStream clientStream(m_clientSocket);
        clientStream.setVersion(QDataStream::Qt_5_7);

        QJsonObject message;
        message["type"] = QStringLiteral("new_file");
        message["filename"] = filename;
        message["author"] = this->username;

        qDebug().noquote() << QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    }
}

void Client::connectToServer(const QHostAddress &address, quint16 port)
{
    m_clientSocket->connectToHost(address, port);
}

void Client::onReadyRead()
{
    // prepare a container to hold the UTF-8 encoded JSON we receive from the socket
    QByteArray jsonData;
    // create a QDataStream operating on the socket
    QDataStream socketStream(m_clientSocket);
    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
    socketStream.setVersion(QDataStream::Qt_5_7);
    // start an infinite loop
    for (;;) {
        // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
        socketStream.startTransaction();
        // we try to read the JSON data
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            // we successfully read some data
            // we now need to make sure it's in fact a valid JSON
            QJsonParseError parseError;
            // we try to create a json document with the data we received
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                // if the data was indeed valid JSON
                if (jsonDoc.isObject()) // and is a JSON object
                    jsonReceived(jsonDoc.object()); // parse the JSON
            } else { // profile image received
                qDebug() << "Profile image received";
                profile->loadFromData(jsonData);
            }
            // loop and try to read more JSONs if they are available
        } else {
            // the read failed, the socket goes automatically back to the state it was in before the transaction started
            // we just exit the loop and wait for more data to become available
            break;
        }
    }
}

QString Client::getNickname(){
    return this->nickname;
}
QString Client::getUsername(){
    return this->username;
}
void Client::setNickname(const QString& nickname){
    this->nickname=nickname;
}

QPixmap* Client::getProfile(){
    return profile;
}

void Client::setProfileImage(const QString& filename)
{
    profile->load(filename);
}

void Client::sendProfileImage()
{
    if (m_clientSocket->waitForConnected()) {
        QDataStream clientStream(m_clientSocket);
        clientStream.setVersion(QDataStream::Qt_5_7);

        QByteArray bArray;
        QBuffer buffer(&bArray);
        buffer.open(QIODevice::WriteOnly);
        profile->save(&buffer, "PNG");
        clientStream << bArray;
    }
}

void Client::sendProfileImage(const QString& name,QPixmap* image )
{
    if (m_clientSocket->waitForConnected()) {
        QDataStream clientStream(m_clientSocket);
        clientStream.setVersion(QDataStream::Qt_5_7);
        QJsonObject message;
        message["type"] = QStringLiteral("image_signup");
        message["image_name"] = name;
        qDebug().noquote() << QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
        QByteArray bArray;
        QBuffer buffer(&bArray);
        buffer.open(QIODevice::WriteOnly);
        image->save(&buffer, "PNG");
        clientStream << bArray;
    }
}

void Client::overrideProfileImage(const QPixmap& pixmap)
{
    *this->profile = pixmap;
}


QList<QPair<QString,QString>> Client::getActiveFiles(){
    return files;
}


void Client::sendJson(const QJsonObject& message)
{
    QDataStream clientStream(m_clientSocket);
    clientStream.setVersion(QDataStream::Qt_5_7);

    clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
}

void Client::openFile(const QString& filename){
    if (m_clientSocket->waitForConnected()) {
        QDataStream clientStream(m_clientSocket);
        clientStream.setVersion(QDataStream::Qt_5_7);

        QJsonObject message;
        message["type"] = QStringLiteral("file_to_open");
        message["filename"] = filename;

        qDebug().noquote() << QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    }
}

void Client::closeFile(){
    if (m_clientSocket->waitForConnected()) {
        QDataStream clientStream(m_clientSocket);
        clientStream.setVersion(QDataStream::Qt_5_7);

        QJsonObject message;
        message["type"] = QStringLiteral("close");
        message["filename"] = this->openfile;
        message["username"]=this->username;

        qDebug().noquote() << QString::fromUtf8(QJsonDocument(message).toJson(QJsonDocument::Compact));
        clientStream << QJsonDocument(message).toJson(QJsonDocument::Compact);
    }
}

QString Client::getSharedLink()
{
    return this->sharedLink;
}

QString Client::getOpenedFile()
{
    return this->openfile;
}
void Client::setOpenedFile(const QString& name) {
    this->openfile = name;
}
int Client::getColor()
{
    return cursor_color_rgb;
}


