#include "signup.h"
#include "appMainWindow.h"
#include "ui_signup.h"
#include <QDebug>
#include <QFileDialog>
#include <QPainter>
#include <QRegularExpression>
#include <QStyle>
#include <QTimer>

Signup::Signup(QWidget *parent, Client *client)
    : QWidget(parent), ui(new Ui::Signup), client(client) {
  ui->setupUi(this);
  profile = new QPixmap(":/images/anonymous");
  photo = false;
  ui->profile_image->setCustomPixmap(*profile);
  ui->iconInfoUser->setVisible(false);
  ui->iconInfoPass->setVisible(false);
  AppMainWindow::errorLineEdit(ui->lineEditPassword, false);
  AppMainWindow::errorLineEdit(ui->lineEditConfirmPassword, false);
  AppMainWindow::errorLineEdit(ui->lineEditUsername, false);
  ui->lineEditConfirmPassword->setDisabled(true);
  this->popUp = new QMessageBox(this);
  valid = false;

  // connect(ui->profile_image, &ClickableLabel::clicked, this,
  // &Signup::on_pushButtonUpload_clicked);
  connect(client, &Client::signedUp, this, &Signup::signedUp);
  connect(client, &Client::signupError, this, &Signup::signupFailed);
  // Enable signup but pressing 'enter' on confirm password textfield
  connect(ui->lineEditConfirmPassword, &QLineEdit::returnPressed, this,
          &Signup::on_pushButtonSignup_clicked);

  connect(client, &Client::existingUsername, this,
          &Signup::on_failedUsernameCheck);
  connect(client, &Client::successUsernameCheck, this,
          &Signup::on_successUsernameCheck);
}

Signup::~Signup() { delete ui; }

/*
void Signup::on_pushButtonClear_clicked()
{
        this->clearLineEdit();
        this->clearLabel();
        QPixmap pix(":/images/anonymous");
    photo=false;
        ui->profile_image->setPixmap(pix.scaled(IMAGE_WIDTH, IMAGE_HEIGHT,
                                                                                        Qt::KeepAspectRatioByExpanding));
        ui->lineEditConfirmPassword->setDisabled(true);
        valid = false;
}*/

void Signup::signedUp() {
  // Enable buttons after receiving server reply
  ui->pushButtonSignup->setEnabled(true);
  // ui->pushButtonClear->setEnabled(true);
  ui->t_pushButtonBackLogin->setEnabled(true);

  ui->lineEditUsername->clear();
  ui->lineEditPassword->clear();
  ui->lineEditConfirmPassword->clear();
  AppMainWindow::errorLineEdit(ui->lineEditPassword, false);
  AppMainWindow::errorLineEdit(ui->lineEditConfirmPassword, false);
  AppMainWindow::errorLineEdit(ui->lineEditUsername, false);

  ui->labelInfoPass->clear();
  ui->iconInfoUser->setVisible(false);
  ui->iconInfoPass->setVisible(false);

  QPixmap pix(":/images/anonymous"); // cercare .png
  ui->profile_image->setCustomPixmap(pix);
  photo = false;
  client->disconnectFromHost();

  emit changeLoginLabel("Correctly signed up");
  emit changeWidget(LOGIN);
}

void Signup::signupFailed(const QString &reason) {
  client->disconnectFromHost();
  if (reason.contains("username")) {
    ui->labelInfoUser->setText(reason);
    ui->iconInfoUser->setVisible(true);
    AppMainWindow::errorLineEdit(ui->lineEditUsername, true);
    inuse_username = USED;
  } else {
    ui->labelInfoPass->setText(reason);
    AppMainWindow::errorLineEdit(ui->lineEditPassword, true);
    AppMainWindow::errorLineEdit(ui->lineEditConfirmPassword, true);
    ui->iconInfoPass->setVisible(true);
  }
  // Show popup for 1 second
  this->popUp->setText("Failed sign up");
  this->popUp->setWindowTitle("Sign up");
  this->popUp->setStandardButtons(this->popUp->NoButton);
  this->popUp->setModal(false);
  QTimer::singleShot(1500, this->popUp, &QMessageBox::hide); // 1000 ms
  this->popUp->show();

  clearLabel();
  clearLineEdit();
  ui->pushButtonSignup->setEnabled(true);
  // ui->pushButtonClear->setEnabled(true);
  ui->t_pushButtonBackLogin->setEnabled(true);
}

void Signup::on_pushButtonSignup_clicked() {
  QString username = ui->lineEditUsername->text();
  QString password = ui->lineEditPassword->text();
  QString confirm = ui->lineEditConfirmPassword->text();

  if (inuse_username == UNCHECKED)
    client->checkExistingOrNotUsername(username);

  if (checkUsername(username) && valid && inuse_username != USED &&
      checkConfirmation(password, confirm)) {
    // Disable buttons before receiving server reply
    ui->pushButtonSignup->setEnabled(false);
    // ui->pushButtonClear->setEnabled(false);
    ui->t_pushButtonBackLogin->setEnabled(false);
    if (photo == false)
      client->signup(username, password, nullptr);
    else
      client->signup(username, password, profile);
  }
}

void Signup::on_lineEditUsername_editingFinished() {
  QString username = ui->lineEditUsername->text();
  checkUsername(username);
  if (username.size() > 0 && inuse_username == UNCHECKED) {
    client->checkExistingOrNotUsername(username);
  }
}

void Signup::on_lineEditPassword_editingFinished() {
  QString password = ui->lineEditPassword->text();
  checkPassword(password);
  if (valid == true) {
    if (ui->lineEditConfirmPassword->text().size() > 0)
      checkConfirmation(password, ui->lineEditConfirmPassword->text());
    else {
      ui->labelInfoPass->clear();
      AppMainWindow::errorLineEdit(ui->lineEditPassword, false);
      AppMainWindow::errorLineEdit(ui->lineEditConfirmPassword, false);
      ui->iconInfoPass->setVisible(false);
    }
  }
}

void Signup::on_lineEditConfirmPassword_editingFinished() {

  QString password1 = ui->lineEditPassword->text();
  QString password2 = ui->lineEditConfirmPassword->text();

  checkConfirmation(password1, password2);
}

void Signup::on_lineEditUsername_textChanged(const QString &) {
  inuse_username = UNCHECKED;
  ui->labelInfoUser->clear();
  AppMainWindow::errorLineEdit(ui->lineEditUsername, false);

  ui->iconInfoUser->setVisible(false);
}

void Signup::on_lineEditPassword_textChanged(const QString &arg) {
  QString conf = ui->lineEditConfirmPassword->text();
  ui->labelInfoPass->clear();
  AppMainWindow::errorLineEdit(ui->lineEditPassword, false);
  AppMainWindow::errorLineEdit(ui->lineEditConfirmPassword, false);
  ui->iconInfoPass->setVisible(false);
  if (conf.size() == 0) {
    if (arg.size() > 0) {
      ui->lineEditConfirmPassword->setDisabled(false);

    } else {
      ui->lineEditConfirmPassword->setDisabled(true);
      ui->lineEditConfirmPassword->clear();
    }
  } else {
    checkPassword(arg);
    checkConfirmation(arg, conf);
  }
}

void Signup::on_lineEditConfirmPassword_textChanged(const QString &) {
  if (ui->labelInfoPass->text().contains("match")) {
    ui->labelInfoPass->clear();
    AppMainWindow::errorLineEdit(ui->lineEditPassword, false);
    AppMainWindow::errorLineEdit(ui->lineEditConfirmPassword, false);
    ui->iconInfoPass->setVisible(false);
  }
  QString password1 = ui->lineEditPassword->text();
  QString password2 = ui->lineEditConfirmPassword->text();

  checkConfirmation(password1, password2);
}

bool Signup::checkUsername(const QString &username) {
  QRegularExpression re("^[A-Za-z0-9\\-_]+(\\.[A-Za-z0-9\\-_]+)*"
                        "@[A-Za-z0-9\\-]+(\\.[A-Za-z0-9\\-]+)*"
                        "(\\.[a-z]{2,6})$");

  QRegularExpressionMatch match = re.match(username);
  bool hasMatch = match.hasMatch();

  if (username.size() > 0 && !hasMatch) {
    ui->labelInfoUser->setText("Invalid email");
    ui->iconInfoUser->setVisible(true);
    AppMainWindow::errorLineEdit(ui->lineEditUsername, true);

    return false;
  }
  return true;
}

bool Signup::isValidPassword(const QString &password, QString &errorMsg,
                             bool &valid) {
  bool success = true;
  // Check password length
  if (password.size() < 8 || password.size() > 12) {
    success = false;
    errorMsg = "Min. 8 characters, Max 12 characters";
  } else {
    errorMsg = "";
    // Set characters not allowed
    QRegularExpression regex("[^A-Za-z0-9@\\.\\-_]");
    QRegularExpressionMatchIterator m = regex.globalMatch(password);
    if (m.hasNext()) {
      success = false;
      errorMsg.append("Characters allowed: A-Z, a-z, 0-9, @ . _ -\n");
    }

    // At least an upper case letter
    regex.setPattern("[A-Z]");
    m = regex.globalMatch(password);
    if (!m.hasNext()) {
      success = false;
      errorMsg.append("Use at least 1 upper case letter\n");
    }

    // At least a lower case letter
    regex.setPattern("[a-z]");
    m = regex.globalMatch(password);
    if (!m.hasNext()) {
      success = false;
      errorMsg.append("Use at least 1 lower case letter\n");
    }

    // At least a digit
    regex.setPattern("[0-9]");
    m = regex.globalMatch(password);
    if (!m.hasNext()) {
      success = false;
      errorMsg.append("Use at least 1 digit\n");
    }

    // At least a special character
    regex.setPattern("[@\\.\\-_]");
    m = regex.globalMatch(password);
    if (!m.hasNext()) {
      success = false;
      errorMsg.append("Use at least 1 special character");
    }
  }
  valid = success;
  return success;
}

void Signup::checkPassword(const QString &password) {
  if (password.size() > 0) {
    QString msg;
    bool success = isValidPassword(password, msg, valid);
    if (!success) {
      ui->labelInfoPass->setText(msg);
      ui->iconInfoPass->setVisible(true);
      AppMainWindow::errorLineEdit(ui->lineEditPassword, true);
    }
  }
}

bool Signup::checkConfirmation(const QString &pass, const QString &conf) {
  if (conf.size() > 0 && valid == true) {
    int res = QString::compare(pass, conf, Qt::CaseSensitive);
    if (res != 0) {
      ui->labelInfoPass->setText("Passwords don't match");
      ui->iconInfoPass->setVisible(true);
      AppMainWindow::errorLineEdit(ui->lineEditPassword, true);
      AppMainWindow::errorLineEdit(ui->lineEditConfirmPassword, true);
      return false;
    }
  }
  return true;
}

void Signup::on_t_pushButtonBackLogin_clicked() {
  this->clearLabel();
  this->clearLineEdit();
  QPixmap image(":/images/anonymous");
  ui->profile_image->setCustomPixmap(image);
  photo = false;
  emit changeWidget(LOGIN);
}

void Signup::clearLabel() {
  ui->labelInfoPass->clear();
  ui->labelInfoUser->clear();
  ui->iconInfoUser->setVisible(false);
  ui->iconInfoPass->setVisible(false);
  AppMainWindow::errorLineEdit(ui->lineEditUsername, false);
  AppMainWindow::errorLineEdit(ui->lineEditPassword, false);
  AppMainWindow::errorLineEdit(ui->lineEditConfirmPassword, false);
}

void Signup::clearLineEdit() {
  ui->lineEditUsername->clear();
  ui->lineEditPassword->clear();
  ui->lineEditConfirmPassword->clear();
}

void Signup::enableAllButtons() {
  ui->pushButtonSignup->setEnabled(true);
  // ui->pushButtonClear->setEnabled(true);
  ui->t_pushButtonBackLogin->setEnabled(true);
}

void Signup::on_profile_image_clicked() {
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::homePath(),
                                   tr("Image Files (*.png *.jpg *.bmp)"));

  if (!fileName.isEmpty() && !fileName.isNull()) {
    profile->load(fileName);
    ui->profile_image->setCustomPixmap(*profile);
    photo = true;
  }
}

QPixmap *Signup::getProfile() { return profile; }

void Signup::on_failedUsernameCheck(const QString &username) {
  if (!ui->lineEditUsername->text().compare(username)) {
    inuse_username = USED;
    ui->labelInfoUser->setText("Username already in use");
    ui->iconInfoUser->setVisible(true);
    AppMainWindow::errorLineEdit(ui->lineEditUsername, true);
  }
}

void Signup::on_successUsernameCheck(const QString &username) {
  if (!ui->lineEditUsername->text().compare(username)) {
    inuse_username = FREE;
  }
}
