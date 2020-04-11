#ifndef MODIFY_PROFILE_H
#define MODIFY_PROFILE_H

#include <QWidget>
#include "appMainWindow.h"
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
	void changeWidget(int);

private slots:
	void on_pushButtonUpload_clicked();
	void on_lineEditNickname_editingFinished();
	void on_lineEditNickname_textChanged(const QString &arg1);
	void on_lineEditNewPass_textChanged(const QString &arg1);
	void on_lineEditConfirmPass_textChanged(const QString &arg1);
	void on_lineEditNewPass_editingFinished();
	void on_lineEditConfirmPass_editingFinished();
	void on_pushSaveNickname_clicked();
	void on_pushButtonSavePassword_clicked();
	void on_pushButtonResetNickname_clicked();
	void on_pushButtonResetPhoto_clicked();
	void on_pushButtonSavephoto_clicked();
	void on_pushButtonFinish_clicked();
	void on_pushButtonCancel_clicked();
	void on_lineEditOldPass_editingFinished();
	void on_wrongOldPasswordEntered();
	void on_correctOldPasswordEntered();

public slots:
	void upload();

private:
	Ui::modify *ui;
	Client* client;
	QPixmap *profile_photo_temp;
	bool valid_new_password = false;
	bool correct_old_password;

	void clearLabels();
	void checkPassword(const QString &password);
	bool checkConfirmation(const QString &pass,const QString &conf);
	bool checkNickname(const QString &nickname);
};

#endif // MODIFY_PROFILE_H
