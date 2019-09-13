#ifndef MODIFY_PROFILE_H
#define MODIFY_PROFILE_H

#include <QWidget>
#include "client.h"

#define IMAGE_WIDTH 170
#define IMAGE_HEIGHT 170

namespace Ui {
class modify;
}

class Modify : public QWidget
{
    Q_OBJECT

public:
    Modify(QWidget *parent = nullptr,Client* client=nullptr);
    ~Modify();

signals:
    void action(int);

private slots:
    void on_pushButtonUpload_clicked();

    void on_pushButtonCancel_clicked();

    void on_lineEditNickname_editingFinished();

    void on_lineEditNickname_textChanged(const QString &arg1);

    void on_lineEditNewPass_textChanged(const QString &arg1);

    void on_lineEditConfirmPass_textChanged(const QString &arg1);

    void on_pushButtonLogin_clicked();

    void on_lineEditNewPass_editingFinished();

    void on_lineEditConfirmPass_editingFinished();

public slots:
    void upload();

private:
    Ui::modify *ui;
    Client* client;
    void clearLabels();
    bool valid;
    void checkPassword(const QString &password);
    bool checkConfirmation(const QString &pass,const QString &conf);
    bool checkUsername(const QString &username);
};

#endif // MODIFY_PROFILE_H
