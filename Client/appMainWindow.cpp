#include "appMainWindow.h"
#include "editor.h"
#include "home.h"
#include "login.h"
#include "modify.h"
#include "signup.h"
#include "ui_index.h"
#include <QLayout>
#include <QStackedWidget>
#include <QtWidgets>

AppMainWindow::AppMainWindow(QWidget *parent, Client *c)
    : QMainWindow(parent), ui(new Ui::Index), client(c) {
  // Center window
  this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                        this->size(),
                                        qApp->desktop()->availableGeometry()));

  // Read stylesheet from css file
  QString stylesheet;
  QFile cssFile(":resources/style.css");
  if (cssFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    stylesheet.append(cssFile.readAll().data());
  }
  qApp->setStyleSheet(stylesheet);

  // Set fixed window size
  this->setFixedSize(QSize(640, 500));

  ui->setupUi(this);
  client->setParent(this);

  stackedWidget = new QStackedWidget(this);
  QWidget *w = ui->widget;
  QLayout *layout = new QGridLayout(w);
  w->setLayout(layout);
  layout->addWidget(stackedWidget);

  login = new Login(stackedWidget, client);
  stackedWidget->addWidget(login);
  connect(login, &Login::changeWidget, this, &AppMainWindow::on_changeWidget);

  signup = new Signup(stackedWidget, client);
  stackedWidget->addWidget(signup);
  connect(signup, &Signup::changeWidget, this, &AppMainWindow::on_changeWidget);
  connect(signup, &Signup::changeLoginLabel, this,
          &AppMainWindow::on_changeLoginLabel);

  home = new Home(stackedWidget, client);
  stackedWidget->addWidget(home);
  connect(home, &Home::changeWidget, this, &AppMainWindow::on_changeWidget);

  // Editor is not included in stackedwidget,
  // because stackedwidget doesn't support qmainwindow
  editor = new Editor(this, client);
  connect(editor, &Editor::changeWidget, this, &AppMainWindow::on_changeWidget);
  // Center
  editor->setGeometry(
      QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, editor->size(),
                          qApp->desktop()->availableGeometry()));

  modify = new Modify(stackedWidget, client);
  stackedWidget->addWidget(modify);
  connect(modify, &Modify::changeWidget, this, &AppMainWindow::on_changeWidget);

  connect(home, &Home::modify, modify, &Modify::upload);
  connect(home, &Home::logOut, this, &AppMainWindow::on_logOut);
  connect(client, &Client::error, this, &AppMainWindow::error);
}

AppMainWindow::~AppMainWindow() { delete ui; }

void AppMainWindow::on_logOut() {
  if (!editor->isHidden())
    editor->clear(true);
  login->disconnect();
}

void AppMainWindow::on_changeWidget(int widget) {
  // Editor is not included in stackedwidget, because stackedwidget doesn't
  // support qmainwindow
  if (widget == EDITOR) {
    this->hide();
    editor->peerYou(); // Set 'You' in the listWidget
                       // with the last nickname
    editor->show();
  } else {
    editor->hide();
    if (widget==LOGIN || widget==HOME){
        // Set fixed window size
        this->setFixedSize(QSize(640, 600));
    }
    else{
        // Set fixed window size
        this->setFixedSize(QSize(640, 700));
    }
    if (widget == HOME) {
      this->home->setProfile();
    }
    stackedWidget->setCurrentIndex(widget);
    this->show();
  }
}

void AppMainWindow::on_changeLoginLabel() {
  login->correctlySignedup();
}

void AppMainWindow::error(QAbstractSocket::SocketError socketError) {
  switch (socketError) {
  // This is always called in couple with QAbstractSocket::
  // ProxyConnectionClosedError (note that there is no break in the
  // 'case') when the server window is closed or the server crashes
  case QAbstractSocket::RemoteHostClosedError:
    // This is called alone (not in couple with QAbstractSocket::
    // RemoteHostClosedError) only when the button stop
    // is pressed in the server GUI
  case QAbstractSocket::ProxyConnectionClosedError: // stop
    this->on_logOut();
    QMessageBox::warning(this, tr("Disconnected"),
                         tr("The host terminated the connection"));
    on_changeWidget(LOGIN);
    break;

  case QAbstractSocket::ConnectionRefusedError:
    QMessageBox::critical(this, tr("Error"),
                          tr("The host refused the connection"));
    break;
  case QAbstractSocket::ProxyConnectionRefusedError:
    QMessageBox::critical(this, tr("Error"),
                          tr("The proxy refused the connection"));
    break;
  case QAbstractSocket::ProxyNotFoundError:
    QMessageBox::critical(this, tr("Error"), tr("Could not find the proxy"));
    break;
  case QAbstractSocket::HostNotFoundError:
    QMessageBox::critical(this, tr("Error"), tr("Could not find the server"));
    break;
  case QAbstractSocket::SocketAccessError:
    QMessageBox::critical(this, tr("Error"),
                          tr("You don't have permissions "
                             "to execute this operation"));
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
    QMessageBox::critical(this, tr("Error"),
                          tr("Your proxy requires authentication"));
    break;
  case QAbstractSocket::ProxyProtocolError:
    QMessageBox::critical(this, tr("Error"), tr("Proxy comunication failed"));
    break;
  case QAbstractSocket::TemporaryError:
  case QAbstractSocket::OperationError:
    QMessageBox::warning(this, tr("Error"),
                         tr("Operation failed, please try again"));
    return;
  default:
    Q_UNREACHABLE();
  }

  login->enableAllButtons();
  signup->enableAllButtons();
}

void AppMainWindow::errorLineEdit(QLineEdit *lineEdit, bool f) {
  lineEdit->setProperty("error", f);

  lineEdit->style()->unpolish(lineEdit);
  lineEdit->style()->polish(lineEdit);
  lineEdit->update();
}
