#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QReadWriteLock>
class QJsonObject;
class ServerWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWorker)
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);
    void sendJson(const QJsonObject &json);
    void sendProfileImage();
    QString getNickname();
    QString getUsername();
    void setNickname(const QString &nickname);
    void clearNickname();
    void setUsername(const QString &username);
    QString getFilename();
    void setFilename(const QString& filename);
    void closeFile();
    void setColor(int color);

public slots:
    void disconnectFromClient();

private slots:
    void receiveJson();

signals:
    void jsonReceived(const QJsonObject &jsonDoc);
    void disconnectedFromClient();
    void error();
    void logMessage(const QString &msg);

private:
    QTcpSocket *m_serverSocket;
    QString username;
    QString nickname;
    QString shared_link;
    QString image_name;
    QString filename;
    int color_rgb;
};

#endif // SERVERWORKER_H
