#include "login.h"
#include "ui_login.h"
#include <QHostAddress>
#include <QPixmap>
#include <QRegularExpression>
#include <QtWidgets>

Login::Login(QWidget *parent, Client *client)
    : QWidget(parent), ui(new Ui::Login), client(client) {
  ui->setupUi(this);

  clearError();

  connect(client, &Client::loggedIn, this, &Login::loggedIn);
  connect(client, &Client::loginError, this, &Login::loginFailed);

  // Try to login by clicking login button or pressing 'enter'
  connect(ui->pushButtonLogin, &QPushButton::clicked, this, &Login::try_to_log);
  connect(ui->lineEditPassword, &QLineEdit::returnPressed, this,
          &Login::try_to_log);
  connect(ui->lineEditPassword, &QLineEdit::returnPressed, this,
          &Login::try_to_log);
}

Login::~Login() { delete ui; }

void Login::enableAllButtons() {
  ui->pushButtonLogin->setEnabled(true);
  ui->t_pushButtonNewAccount->setEnabled(true);
}

void Login::try_to_log() {
  QString username = ui->lineEditUsername->text();
  QString password = ui->lineEditPassword->text();

  // Disable the connect button to prevent the user from clicking it again
  ui->pushButtonLogin->setEnabled(false);
  ui->t_pushButtonNewAccount->setEnabled(false);
  bool empty = false;
  if (username.isNull() || username.isEmpty()) {
    emit loginFailed("Empty username");
    empty = true;
  }
  if (password.isNull() || password.isEmpty()) {
    emit loginFailed("Empty password");
    empty = true;
  }

  if (empty)
    return;
  client->login(username, password);
}

void Login::loggedIn() {
  ui->pushButtonLogin->setEnabled(true);
  ui->t_pushButtonNewAccount->setEnabled(true);
  ui->lineEditUsername->clear();
  ui->lineEditPassword->clear();

  emit changeWidget(HOME);
}

void Login::loginFailed(const QString &reason) {
  ui->pushButtonLogin->setEnabled(true);
  ui->t_pushButtonNewAccount->setEnabled(true);
  addError(reason);

  client->disconnectFromHost();
}

void Login::clearLabel() { clearError(); }

void Login::clearLineEdit() {
  ui->lineEditPassword->clear();
  ui->lineEditUsername->clear();
  clearError();
}

void Login::on_t_pushButtonNewAccount_clicked() {
  this->clearLabel();
  this->clearLineEdit();
  emit changeWidget(SIGNUP);
}

void Login::disconnect() { client->disconnectFromHost(); }

void Login::on_lineEditUsername_textChanged(const QString &) {
  this->clearLabel();
}

void Login::correctlySignedup() {
  // Show popup for 1 second
}

void Login::addError(QString error) {
  ui->icon_error->setVisible(true);
  qDebug() << error;
  // Append if error not already signaled
  if (!ui->labelMessage->text().contains(error)) {
    if (ui->labelMessage->text().isEmpty() || error.contains("Invalid")) {
      ui->labelMessage->setText(error);
    } else {
      ui->labelMessage->setText(ui->labelMessage->text() + "\n" + error);
    }
  }
  if (error == "Empty email")
    AppMainWindow::errorLineEdit(ui->lineEditUsername, true);
  else if (error == "Empty password")
    AppMainWindow::errorLineEdit(ui->lineEditPassword, true);
  else {
    AppMainWindow::errorLineEdit(ui->lineEditUsername, true);
    AppMainWindow::errorLineEdit(ui->lineEditPassword, true);
  }
}

void Login::clearError() {
  ui->icon_error->setVisible(false);
  ui->labelMessage->clear();

  AppMainWindow::errorLineEdit(ui->lineEditUsername, false);
  AppMainWindow::errorLineEdit(ui->lineEditPassword, false);
}
