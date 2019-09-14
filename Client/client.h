#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class QHostAddress;
class QJsonDocument;
class Client : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Client)
public:
    explicit Client(QObject *parent = nullptr);
    QString getNickname();
    QString getUsername();
    void setNickname(const QString& nickname);
    QPixmap* getProfile();
    void send();

public slots:
    void connectToServer(const QHostAddress &address, quint16 port);
    void login(const QString &username, const QString &password);
    void signup(const QString &username, const QString &password);
    void updateNickname(const QString &nickname);
    void updatePassword(const QString &oldpassword,const QString &newpassword);
//    void sendMessage(const QString &text);
    void disconnectFromHost();
private slots:
    void onReadyRead();
signals:
    void connected();
    void loggedIn();
    void signedUp();
    void signupError(const QString &reason);
    void loginError(const QString &reason);
    void disconnected();
    void messageReceived(const QString &sender, const QString &text);
    void error(QAbstractSocket::SocketError socketError);
    void userJoined(const QString &username);
    void userLeft(const QString &username);
private:
    QTcpSocket *m_clientSocket;
    bool m_loggedIn;
    void jsonReceived(const QJsonObject &doc);

    QString username;   //valutare se puntatore o no!
    QString nickname;
    QPixmap* profile;

};

#endif // CLIENT_H
