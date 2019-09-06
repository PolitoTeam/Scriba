#include "login.h"
#include "ui_login.h"
#include <QtWidgets>
#include <QPixmap>
#include <QRegularExpression>
#include <QHostAddress>

Login::Login(QWidget *parent) :
    QWidget (parent),
    ui(new Ui::Login),
    client(new Client(this))
{
    ui->setupUi(this);
    QPixmap pix(":/images/bold.png");
    int w=ui->label->width();
    int h=ui->label->height();
    ui->label->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

    connect(client, &Client::connected, this, &Login::connectedToServer);
    connect(client, &Client::loggedIn, this, &Login::loggedIn);
    connect(client, &Client::loginError, this, &Login::loginFailed);
//    connect(client, &Client::messageReceived, this, &Login::messageReceived);
//    connect(client, &Client::disconnected, this, &Login::disconnectedFromServer);
    connect(client, &Client::error, this, &Login::error);
//    connect(client, &Client::userJoined, this, &Login::userJoined);
//    connect(client, &Client::userLeft, this, &Login::userLeft);
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButtonLogin_clicked()
{
    QString username = ui->lineEditUsername->text();
    QString password = ui->lineEditPassword->text();

    // disable the connect button to prevent the user clicking it again
    ui->pushButtonLogin->setEnabled(false);
    // tell the client to connect to the host using the port 1967
    client->connectToServer(QHostAddress::Any, 1500);
    client->login(username, password);
}

void Login::connectedToServer()
{
    const QString username = ui->lineEditUsername->text();
    const QString password = ui->lineEditPassword->text();
    qDebug().noquote().nospace() << "Trying login: " << username << ":" << password;
    attemptLogin(username, password);
}

void Login::attemptLogin(const QString &username, const QString &password)
{
    // use the client to attempt a log in with the given username
    client->login(username, password);
}

void Login::loggedIn()
{
    qDebug() << "Login succeeded.";
    ui->pushButtonLogin->setEnabled(true);
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
    emit access(2);
}

void Login::loginFailed(const QString &reason)
{
    qDebug() << "Login failed.";
    client->disconnectFromHost();
    ui->labelMessage->setText(reason);
    ui->pushButtonLogin->setEnabled(true);
}

void Login::error(QAbstractSocket::SocketError socketError)
{
    // show a message to the user that informs of what kind of error occurred
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        QMessageBox::warning(this, tr("Disconnected"), tr("The host terminated the connection"));
        emit access(0);
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The host refused the connection"));
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The proxy refused the connection"));
        break;
    case QAbstractSocket::ProxyNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the proxy"));
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the server"));
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::critical(this, tr("Error"), tr("You don't have permissions to execute this operation"));
        break;
    case QAbstractSocket::SocketResourceError:
        QMessageBox::critical(this, tr("Error"), tr("Too many connections opened"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::warning(this, tr("Error"), tr("Operation timed out"));
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy timed out"));
        break;
    case QAbstractSocket::NetworkError:
        QMessageBox::critical(this, tr("Error"), tr("Unable to reach the network"));
        break;
    case QAbstractSocket::UnknownSocketError:
        QMessageBox::critical(this, tr("Error"), tr("An unknown error occured"));
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        QMessageBox::critical(this, tr("Error"), tr("Operation not supported"));
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        QMessageBox::critical(this, tr("Error"), tr("Your proxy requires authentication"));
        break;
    case QAbstractSocket::ProxyProtocolError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy comunication failed"));
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        QMessageBox::warning(this, tr("Error"), tr("Operation failed, please try again"));
        return;
    default:
        Q_UNREACHABLE();
    }
    // enable the button to connect to the server again
    ui->pushButtonLogin->setEnabled(true);
}

void Login::on_pushButtonNewAccount_clicked()
{
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
    emit access(1);
}

void Login::disconnect() {
    qDebug() << "Logging out.";
    client->disconnectFromHost();
}

void Login::on_lineEditUsername_editingFinished()
{
    QString username = ui->lineEditUsername->text();
    QRegularExpression re("^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$");
    QRegularExpressionMatch match = re.match(username);
    bool hasMatch = match.hasMatch();

    if (!hasMatch)
        ui->labelMessage->setText("Username errato!");

}

void Login::on_lineEditUsername_textChanged(const QString &arg1)
{
    ui->labelMessage->clear();
}



void Login::on_lineEditPassword_textChanged(const QString &arg1)
{
    ui->labelMessage->clear();
}

