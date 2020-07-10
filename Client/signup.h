#ifndef SIGNUP_H
#define SIGNUP_H

#include "appMainWindow.h"
#include "client.h"
#include <QPixmap>
#include <QWidget>
#include <qmessagebox.h>

#define IMAGE_WIDTH 150
#define IMAGE_HEIGHT 150

namespace Ui {
class Signup;
}

class Signup : public QWidget {
  Q_OBJECT

public:
  Signup(QWidget *parent = nullptr, Client *client = nullptr);
  ~Signup();
  void enableAllButtons();
  QPixmap *getProfile();
  static bool isValidPassword(const QString &password, QString &errorMsg,
                              bool &valid);

private slots:
  // void on_pushButtonClear_clicked();
  void on_pushButtonSignup_clicked();
  void on_lineEditUsername_editingFinished();
  void on_lineEditPassword_editingFinished();
  void on_lineEditConfirmPassword_editingFinished();
  void on_lineEditUsername_textChanged(const QString &arg1);
  void on_lineEditPassword_textChanged(const QString &arg1);
  void on_lineEditConfirmPassword_textChanged(const QString &arg1);
  void on_t_pushButtonBackLogin_clicked();
  void signedUp();
  void signupFailed(const QString &reason);
  void on_profile_image_clicked();
  void on_failedUsernameCheck(const QString &username);
  void on_successUsernameCheck(const QString &username);

signals:
  void changeWidget(int i);
  void changeLoginLabel(const QString &label);

private:
  Ui::Signup *ui;
  bool valid;
  Client *client;
  QPixmap *profile;
  bool photo;
  QMessageBox *popUp;
  enum UsernameStatus { FREE, USED, UNCHECKED } inuse_username = UNCHECKED;

  bool checkUsername(const QString &username);
  void checkPassword(const QString &password);
  bool checkConfirmation(const QString &pass, const QString &conf);
  void clearLabel();
  void clearLineEdit();
};

#endif // SIGNUP_H
