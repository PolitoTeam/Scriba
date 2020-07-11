#include "modify.h"
#include "buttonhoverwatcher.h"
#include "signup.h"
#include "ui_modify.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>

Modify::Modify(QWidget *parent, Client *client)
    : QWidget(parent), ui(new Ui::modify), client(client) {
  ui->setupUi(this);

  ui->lineEditConfirmPass->setDisabled(true);
  ui->lineEditNewPass->setDisabled(true);
  ui->icon_error_pass->setVisible(false);
  ui->icon_old_pass->setVisible(false);
  ui->icon_new_pass->setVisible(false);
  AppMainWindow::errorLineEdit(ui->lineEditOldPass, false);
  AppMainWindow::errorLineEdit(ui->lineEditNewPass, false);
  AppMainWindow::errorLineEdit(ui->lineEditConfirmPass, false);

  ButtonHoverWatcher *watcher = new ButtonHoverWatcher(
      ":/images/back_button.png", ":/images/back_button_hover.png", this);
  ui->t_pushButtonFinish->installEventFilter(watcher);

  profile_photo_temp = new QPixmap();

  connect(client, &Client::wrongOldPassword, this,
          &Modify::on_wrongOldPasswordEntered);
  connect(client, &Client::correctOldPassword, this,
          &Modify::on_correctOldPasswordEntered);
  connect(client, &Client::failedUpdatePassword, this,
          &Modify::on_failedUpdatePassword);
  connect(client, &Client::successUpdatePassword, this,
          &Modify::on_successUpdatePassword);
}

Modify::~Modify() { delete ui; }

void Modify::on_profile_image_clicked() {
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::homePath(),
                                   tr("Image Files (*.png *.jpg *.bmp)"));

  if (!fileName.isEmpty() && !fileName.isNull()) {
    profile_photo_temp->load(fileName);
    ui->profile_image->setCustomPixmap(*profile_photo_temp);
    save_photo();
  }
}

void Modify::upload() {
  QString nickname = client->getNickname();
  ui->profile_image->setCustomPixmap(*client->getProfile());
  ui->lineEditNickname->setText(nickname);
  this->clearLabels();
}

void Modify::clearLabels() {
  ui->lineEditNewPass->clear();
  ui->lineEditOldPass->clear();
  ui->lineEditConfirmPass->clear();
}

void Modify::on_lineEditNickname_editingFinished() {
  QString nickname = ui->lineEditNickname->text();
  checkNickname(nickname);
}

void Modify::on_lineEditNickname_textChanged(const QString &) {
  ui->labelInfoNick->clear();
}

bool Modify::checkNickname(const QString &nickname) {
  if (nickname.isEmpty() || nickname.isNull()) {
    ui->labelInfoNick->setText("Empty nickname");
    return false;
  }
  return true;
}

void Modify::on_pushSaveNickname_clicked() {
  QString nickname = ui->lineEditNickname->text();
  QString original = client->getNickname();

  if (nickname.compare(original) == 0) {
    ui->labelInfoNick->setText("Nickname not modified");
    return;
  }

  if (checkNickname(nickname)) {
    QMessageBox msgbox;
    msgbox.setText("Are you sure?");
    msgbox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    msgbox.setDefaultButton(QMessageBox::Save);
    if (msgbox.exec() == QMessageBox::Save) {
      client->updateNickname(nickname);
    }
  }
}

void Modify::save_photo() {
    client->overrideProfileImage(*this->profile_photo_temp);
    client->sendProfileImage();
}

void Modify::clearForm() {
  ui->lineEditOldPass->clear();
  ui->lineEditNewPass->clear();
  ui->lineEditConfirmPass->clear();
  ui->labelInfoPass->clear();
  ui->icon_error_pass->setVisible(false);
  ui->icon_old_pass->setVisible(false);
  ui->icon_new_pass->setVisible(false);

  AppMainWindow::errorLineEdit(ui->lineEditOldPass, false);
  AppMainWindow::errorLineEdit(ui->lineEditNewPass, false);
  AppMainWindow::errorLineEdit(ui->lineEditConfirmPass, false);
}

void Modify::on_t_pushButtonFinish_clicked() {
  clearForm();
  changeWidget(HOME);
}

// PASSWORD CHECKING

void Modify::checkPassword(const QString &password) {
  if (password.size() > 0) {
    QString msg;
    bool success = Signup::isValidPassword(password, msg, valid_new_password);
    if (!success) {
      addPasswordError(msg);
    }
  }
}

bool Modify::checkConfirmation(const QString &pass, const QString &conf) {
  if (conf.size() > 0 && valid_new_password == true) {
    int res = QString::compare(pass, conf, Qt::CaseSensitive);
    if (res != 0) {
      addPasswordError("Passwords don't match");
      return false;
    }
  }
  return true;
}

void Modify::on_lineEditNewPass_textChanged(const QString &arg1) {
  QString conf = ui->lineEditConfirmPass->text();
  clearNewPasswordError();
  if (conf.size() == 0) {
    if (arg1.size() > 0) {
      ui->lineEditConfirmPass->setDisabled(false);

    } else {
      ui->lineEditConfirmPass->setDisabled(true);
      clearConfirmPasswordError();
      ui->lineEditConfirmPass->clear();
    }
  } else {
    checkPassword(arg1);
    checkConfirmation(arg1, conf);
  }
}

void Modify::on_lineEditNewPass_editingFinished() {
  QString password = ui->lineEditNewPass->text();
  checkPassword(password);
  if (valid_new_password == true) {
    if (ui->lineEditConfirmPass->text().size() > 0)
      checkConfirmation(password, ui->lineEditConfirmPass->text());
    else
      clearNewPasswordError();
  }
}

void Modify::on_lineEditConfirmPass_textChanged() {
  clearConfirmPasswordError();
  QString password1 = ui->lineEditNewPass->text();
  QString password2 = ui->lineEditConfirmPass->text();

  checkConfirmation(password1, password2);
}

void Modify::on_lineEditConfirmPass_editingFinished() {
  QString password1 = ui->lineEditNewPass->text();
  QString password2 = ui->lineEditConfirmPass->text();

  checkConfirmation(password1, password2);
}

void Modify::on_pushButtonSavePassword_clicked() {

  QString oldpass = ui->lineEditOldPass->text();

  QString newpass = ui->lineEditNewPass->text();
  QString confirm = ui->lineEditConfirmPass->text();
  checkPassword(newpass);
  if (correct_old_password == UNCHECKED)
    client->checkOldPassword(oldpass);

  // Don't send the request only if it's sure that the current old password is
  // wrong; send when it is unchecked (pending request with yet no response),
  // checked only on server, or when is correct;
  if (valid_new_password && correct_old_password == CORRECT &&
      checkConfirmation(newpass, confirm)) {
    QMessageBox msgbox;
    msgbox.setText("Are you sure?");
    msgbox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    msgbox.setDefaultButton(QMessageBox::Save);
    if (msgbox.exec() == QMessageBox::Save) {
      client->updatePassword(oldpass, newpass);
    }
  }
}

void Modify::on_lineEditOldPass_editingFinished() {
  QString old_password = ui->lineEditOldPass->text();

  client->checkOldPassword(old_password);
}

void Modify::on_lineEditOldPass_textChanged(const QString &arg1) {
  correct_old_password = UNCHECKED;
  clearOldPasswordError();

  if (arg1.size() > 0) {
    ui->lineEditNewPass->setDisabled(false);
  } else {
    ui->lineEditNewPass->setDisabled(true);
    ui->lineEditConfirmPass->setDisabled(true);
    clearNewPasswordError();
    ui->lineEditNewPass->clear();
    ui->lineEditConfirmPass->clear();
  }
}

void Modify::on_failedUpdatePassword(const QString &reason) {
  if (reason.contains("password") && !reason.contains("new")) {
    ui->icon_error_pass->setVisible(true);
    ui->icon_old_pass->setVisible(true);
    AppMainWindow::errorLineEdit(ui->lineEditOldPass, false);
  } else {
    if (reason.contains("Wrong password")) {
      correct_old_password = WRONG;
      ui->icon_old_pass->setVisible(true);
      AppMainWindow::errorLineEdit(ui->lineEditOldPass, false);
    } else {
      ui->icon_new_pass->setVisible(true);
      AppMainWindow::errorLineEdit(ui->lineEditNewPass, false);
    }
  }
  addPasswordError(reason);
}

void Modify::on_successUpdatePassword() {
  correct_old_password = CORRECT;
  ui->labelInfoPass->setText("Password correctly updated");
  ui->icon_error_pass->setVisible(false);
  ui->icon_old_pass->setVisible(false);
  ui->icon_new_pass->setVisible(false);
  AppMainWindow::errorLineEdit(ui->lineEditNewPass, false);
  AppMainWindow::errorLineEdit(ui->lineEditOldPass, false);
  AppMainWindow::errorLineEdit(ui->lineEditConfirmPass, false);
}

void Modify::on_wrongOldPasswordEntered(const QString &reason) {
  correct_old_password = WRONG;

  // Print message only if modify window is visible
  if (ui->groupBox->isVisible())
    addPasswordError(reason);
}

void Modify::on_correctOldPasswordEntered() {
  correct_old_password = CORRECT;
  clearOldPasswordError();
}

void Modify::addPasswordError(QString error) {
  ui->icon_error_pass->setVisible(true);

  if (error.contains("Min") || error.contains("at least") ||
      error.contains("Empty")) {
    ui->icon_new_pass->setVisible(true);
    AppMainWindow::errorLineEdit(ui->lineEditNewPass, true);
  }

  if (error.contains("match")) {
    ui->icon_new_pass->setVisible(true);
    AppMainWindow::errorLineEdit(ui->lineEditNewPass, true);
    AppMainWindow::errorLineEdit(ui->lineEditConfirmPass, true);
  }

  if (error.contains("Wrong")) {
    ui->icon_old_pass->setVisible(true);
    AppMainWindow::errorLineEdit(ui->lineEditOldPass, true);
  }

  // Append if error not already signaled
  if (!ui->labelInfoPass->text().contains(error)) {
    if (ui->labelInfoPass->text().isEmpty()) {
      ui->labelInfoPass->setText(error);
    } else {
      ui->labelInfoPass->setText(ui->labelInfoPass->text() + "\n" + error);
    }
  }
}

void Modify::clearOldPasswordError() {
  ui->icon_old_pass->setVisible(false);
  AppMainWindow::errorLineEdit(ui->lineEditOldPass, false);

  if (ui->labelInfoPass->text().contains(QRegExp("\n?Wrong password\n?"))) {
    ui->labelInfoPass->setText(
        ui->labelInfoPass->text().replace(QRegExp("\n?Wrong password\n?"), ""));
  }

  if (ui->labelInfoPass->text().isEmpty())
    ui->icon_error_pass->setVisible(false);
}

void Modify::clearNewPasswordError() {
  if (!ui->labelInfoPass->text().contains("match")) {
    ui->icon_new_pass->setVisible(false);
    AppMainWindow::errorLineEdit(ui->lineEditNewPass, false);
  }
  if (ui->labelInfoPass->text().contains(QRegExp(
          "\n?((Empty password)|(Min. 8 characters, Max 12 characters)|(Use at "
          "least 1 upper case letter)|(Use at least 1 special character)|(Use "
          "at least 1 digit)|(Characters allowed: A-Z, a-z, 0-9, @ \. _ "
          "\-))\n?"))) {
    ui->labelInfoPass->setText(ui->labelInfoPass->text().replace(
        QRegExp("\n?((Empty password)|(Min. 8 characters, Max 12 "
                "characters)|(Use at least 1 upper case letter)|(Use at least "
                "1 special character)|(Use at least 1 digit)|(Characters "
                "allowed: A-Z, a-z, 0-9, @ \. _ \-))\n?"),
        ""));
  }

  if (ui->labelInfoPass->text().isEmpty())
    ui->icon_error_pass->setVisible(false);
}

void Modify::clearConfirmPasswordError() {

  if (!ui->labelInfoPass->text().contains("Min") &&
      !ui->labelInfoPass->text().contains("at least")) {
    ui->icon_new_pass->setVisible(false);
    AppMainWindow::errorLineEdit(ui->lineEditNewPass, false);
  }
  AppMainWindow::errorLineEdit(ui->lineEditConfirmPass, false);

  if (ui->labelInfoPass->text().contains(
          QRegExp("\n?Passwords don't match\n?"))) {
    ui->labelInfoPass->setText(ui->labelInfoPass->text().replace(
        QRegExp("\n?Passwords don't match\n?"), ""));
  }

  if (ui->labelInfoPass->text().isEmpty())
    ui->icon_error_pass->setVisible(false);
}
