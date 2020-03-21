#include "serverworker.h"
#include "server.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QWidget>
#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QThread>

ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent)
    , m_serverSocket(new QSslSocket(this))
{
    connect(m_serverSocket, &QSslSocket::readyRead, this, &ServerWorker::receiveJson);
    connect(m_serverSocket, &QSslSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
    connect(m_serverSocket, &QSslSocket::stateChanged,this,[](QAbstractSocket::SocketState socketState){qDebug()<<socketState;});
    connect(m_serverSocket, &QSslSocket::encrypted, [](){qDebug()<<"Encrypted   done!";});
    connect(m_serverSocket, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),this,&ServerWorker::sslErrors);
}

void ServerWorker::sslErrors(const QList<QSslError> &errors)
{
    foreach (const QSslError &error, errors)
        qDebug() <<"ERROR :"<< error.errorString();
}



bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor,QSslKey key,QSslCertificate cert)
{
    qDebug() << "New Connection! ";

    if (m_serverSocket->setSocketDescriptor(socketDescriptor)) {
             qDebug() << "Socket Descriptor Set! ";

             if (m_serverSocket->waitForConnected()){

                 qDebug()<<"key"<<key;
                 qDebug()<<"cert"<<cert;
                 m_serverSocket->setPrivateKey(key);
                 m_serverSocket->setLocalCertificate(cert);

                 m_serverSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
                 m_serverSocket->startServerEncryption();
             }
             return true;
    } else {
             qDebug() << "Socket Descriptor Not Set! ";
             return false;
    }

}

void ServerWorker::sendJson(const QJsonObject &json)
{
    const QByteArray jsonData = QJsonDocument(json).toJson();
//    emit logMessage("Sending to " + userName() + " - " + QString::fromUtf8(jsonData));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << jsonData;
}

void ServerWorker::sendProfileImage()
{
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);

    QString image_path = QDir::currentPath() + IMAGES_PATH + "/" + username + ".png";

    QFileInfo file(image_path);
    if (!file.exists()) {
        qDebug() << "Image not found.";
        return;
    }

    QPixmap p(image_path);
    QByteArray bArray;
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::WriteOnly);
    p.save(&buffer, "PNG");

    socketStream << bArray;
}

QString ServerWorker::getNickname()
{
    return nickname;
}

QString ServerWorker::getUsername()
{
    return username;
}


void ServerWorker::setUsername(const QString &username) {
    this->username = username;
}

void ServerWorker::setNickname(const QString &nickname)
{
    this->nickname=nickname;
}


void ServerWorker::clearNickname()
{
    nickname.clear();
}


void ServerWorker::disconnectFromClient()
{
    QJsonObject message;
    message["type"] = QStringLiteral("close");
    this->sendJson(message);
    m_serverSocket->disconnectFromHost();
}

void ServerWorker::receiveJson()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);

    // qDebug()<<"Thread: "<<QThread::currentThreadId()<<endl;
    socketStream.setVersion(QDataStream::Qt_5_7);
    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject()){
                    qDebug().noquote() << QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
                    QJsonObject docObj=jsonDoc.object();
                    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
                    if (!typeVal.isNull() && typeVal.isString() && typeVal.toString().compare(QLatin1String("image_signup"), Qt::CaseInsensitive) == 0){
                        const QJsonValue imageName = docObj.value(QLatin1String("image_name"));
                        if (!imageName.isNull() && imageName.isString()){
                            QString im_name = imageName.toString().simplified();
                            qDebug()<<im_name;
                            if (!im_name.isEmpty())
                                image_name=im_name;
                            qDebug()<<image_name;
                        }

                    }

                    else
                        emit jsonReceived(jsonDoc.object());
                }
                else
                    qDebug() << "Invalid message: " + QString::fromUtf8(jsonData);
            } else {
                QPixmap p;
                p.loadFromData(jsonData);
                qDebug() << image_name;
                if (image_name.isEmpty() || image_name.isNull())
                    image_name=username;
                QString image_path = QDir::currentPath() + IMAGES_PATH + "/" + image_name + ".png";
                image_name.clear();

                QFile file(image_path);
                if (file.exists()) // WriteOnly doesn't seem to override as it should be
                    file.remove(); // according to the documentation, need to remove manually
                if (!file.open(QIODevice::WriteOnly))
                    qDebug() << "Unable to open the file specified";

                p.save(&file, "PNG");
                qDebug().nospace() << "Overriding image " << image_path;
            }
        } else {
            qDebug()<<"break";
            break;
        }
    }
}

QString ServerWorker::getFilename(){
    return filename;
}

void ServerWorker::setFilename(const QString& filename){
    this->filename=filename;
}

void ServerWorker::closeFile(){
    this->filename.clear();
}

void ServerWorker::setColor(int color)
{
    this->color_rgb = color;
}
