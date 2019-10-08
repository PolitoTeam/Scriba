#include <QtWidgets>
#include <QStackedWidget>
#include <QLayout>
#include "appMainWindow.h"
#include "ui_index.h"
#include "login.h"
#include "editor.h"
#include "home.h"
#include "signup.h"
#include "modify.h"

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
    connect(login, &Login::changeWidget, this, &AppMainWindow::on_changeWidget);

    signup = new Signup(stackedWidget,client);
    stackedWidget->addWidget(signup);
    connect(signup, &Signup::changeWidget, this, &AppMainWindow::on_changeWidget);

    home = new Home(stackedWidget,client);
    stackedWidget->addWidget(home);
    connect(home, &Home::changeWidget, this, &AppMainWindow::on_changeWidget);

    // editor is not included in stackedwidget, because stackedwidget doesn't support qmainwindow
    editor = new Editor(this, client);
    connect(editor, &Editor::changeWidget, this, &AppMainWindow::on_changeWidget);

    modify = new Modify(stackedWidget,client);
    stackedWidget->addWidget(modify);
    connect(modify, &Modify::changeWidget, this, &AppMainWindow::on_changeWidget);

    QObject::connect(home,&Home::modify,modify,&Modify::upload);
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
    editor->clear();
}

void AppMainWindow::on_changeWidget(int widget) {
    // editor is not included in stackedwidget, because stackedwidget doesn't support qmainwindow
    if (widget == EDITOR ) {
        this->hide();
        editor->show();
    }
    else
    {
        editor->hide();
        stackedWidget->setCurrentIndex(widget);
        this->show();
    }
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
