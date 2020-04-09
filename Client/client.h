#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QSslSocket>
#include "symbol.h"
#include "remotecursor.h"

class QHostAddress;
class QJsonDocument;

class Client : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Client)

public:
    explicit Client(QObject *parent=nullptr, QString addr=nullptr, quint16 port=0);
    QString getNickname();
    QString getUsername();
    void setNickname(const QString& nickname);
    QPixmap* getProfile();
    void setProfileImage(const QString& filename);
    void sendProfileImage();
    void sendProfileImage(const QString& name,QPixmap* pixmap);
    void overrideProfileImage(const QPixmap& pixmap);

    void getFiles(bool shared);
    void getFilenameFromLink(const QString& sharedLink);
    QList<QPair<QString,QString>> getActiveFiles();

    void sendJson(const QJsonObject& message);
    void createNewFile(QString filename);
    void closeFile();

    QString getSharedLink();
    QString getOpenedFile();
    void setOpenedFile(const QString& name);
    int getColor();

public slots:
    void connectToServer(const QHostAddress &address, quint16 port);
    void login(const QString &username, const QString &password);
    void signup(const QString &username, const QString &password);
    void updateNickname(const QString &nickname);
    void updatePassword(const QString &oldpassword,const QString &newpassword);
    void checkOldPassword(const QString &old_password);
    void disconnectFromHost();
    void openFile(const QString& filename);
    void sslErrors(const QList<QSslError> &errors);
    void on_disconnected();

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
    void wrongOldPassword();
    void correctOldPassword();

    void filesReceived(bool shared);
    void openFilesError(const QString& reason);

    void remoteInsert(Symbol s);
    void remotePaste(QJsonArray s);
    void remoteErase(QJsonArray s);
    void remoteChange(Symbol s);
    void remoteAlignChange(Symbol s);
    void correctNewFile();
    void correctOpenedFile();
    void wrongNewFile(const QString& reason);
    void wrongListFiles(const QString& reason);
    void usersConnectedReceived(QList<QPair<QPair<QString,QString>,QPixmap>>);
    void contentReceived(const QString text);
    void userDisconnected(const QString& username, const QString& nickname);
    void wrongSharedLink(const QString& filename);
    void addCRDTterminator();
    void remoteCursor(int editor_id, Symbol s);

private:
    QString addr;
    quint16 port;
    QSslSocket *m_clientSocket;
    bool m_loggedIn;
    QString username;   //valutare se puntatore o no!
    QString nickname;
    QPixmap* profile;
    QList<QPair<QString,QString>> files;
    QString openfile;
    QString sharedLink;
    int cursor_color_rgb;

    void jsonReceived(const QJsonObject &doc);
    void byteArrayReceived(const QByteArray &doc);
};

#endif // CLIENT_H
