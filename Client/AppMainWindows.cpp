#include "AppMainWindow.h"
#include "ui_index.h"
#include <QtWidgets>
#include <QStackedWidget>
#include <QLayout>
#include "login.h"
#include "editor.h"
#include "home.h"
#include "signup.h"

AppMainWindow::AppMainWindow(QWidget *parent,Client* c) :
    QMainWindow(parent),
    ui(new Ui::Index),
    client(c)
{
    ui->setupUi(this);
    client->setParent(this);

    stackedWidget = new QStackedWidget(this);
    QWidget* w=ui->widget;
    QLayout* layout=new QGridLayout(w);
    w->setLayout(layout);
    layout->addWidget(stackedWidget);

    login = new Login(stackedWidget,client);
    stackedWidget->addWidget(login);
    QObject::connect(login, &Login::access,stackedWidget,&QStackedWidget::setCurrentIndex);

    signup = new Signup(stackedWidget,client);
    stackedWidget->addWidget(signup);
    QObject::connect(signup, &Signup::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    home = new Home(stackedWidget);
    home->setClient(client);
    stackedWidget->addWidget(home);
    QObject::connect(home, &Home::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    editor = new Editor(stackedWidget);
    editor->setClient(client);
    stackedWidget->addWidget(editor);
    QObject::connect(editor, &Editor::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    connect(home, &Home::logOut, this, &AppMainWindow::on_logOut);
    connect(client, &Client::error, this, &AppMainWindow::error);

}

AppMainWindow::~AppMainWindow()
{
    delete ui;
}

void AppMainWindow::on_logOut()
{
    login->disconnect();
}

void AppMainWindow::error(QAbstractSocket::SocketError socketError)
{
    // show a message to the user that informs of what kind of error occurred
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        QMessageBox::warning(this, tr("Disconnected"), tr("The host terminated the connection"));
        stackedWidget->setCurrentWidget(login);
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

    login->enableAllButtons();
    signup->enableAllButtons();
}
