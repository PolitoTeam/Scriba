#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QHostAddress>
#include <QDebug>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QBuffer>
#include "client.h"

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
    m_clientSocket->disconnectFromHost();
}

void Client::jsonReceived(const QJsonObject &docObj)
{
    // actions depend on the type of message
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
        return; // a message with no type was received so we just ignore it
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

void Client::overrideProfileImage(const QPixmap& pixmap)
{
    *this->profile = pixmap;
}

