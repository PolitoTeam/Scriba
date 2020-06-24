#include <QFileDialog>
#include <QRegularExpression>
#include <QMessageBox>
#include "modify.h"
#include "signup.h"
#include "ui_modify.h"

Modify::Modify(QWidget *parent,Client* client) :
	QWidget(parent),
	ui(new Ui::modify),
	client(client)
{
	ui->setupUi(this);

	ui->lineEditConfirmPass->setDisabled(true);
	profile_photo_temp = new QPixmap();

	connect(client, &Client::wrongOldPassword,
			this, &Modify::on_wrongOldPasswordEntered);
	connect(client, &Client::correctOldPassword,
			this, &Modify::on_correctOldPasswordEntered);
    connect(client, &Client::failedUpdatePassword,
            this, &Modify::on_failedUpdatePassword);
    connect(client, &Client::successUpdatePassword,
            this, &Modify::on_successUpdatePassword);
}

Modify::~Modify()
{
	delete ui;
}

void Modify::on_profile_image_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(
				this,
				tr("Open Image"), QDir::homePath(),
				tr("Image Files (*.png *.jpg *.bmp)")
	);

	if (!fileName.isEmpty() && !fileName.isNull()) {
		profile_photo_temp->load(fileName);
        ui->profile_image->setCustomPixmap(*profile_photo_temp);
	}
}

void Modify::upload() {
	QString nickname = client->getNickname();
    ui->profile_image->setCustomPixmap(*client->getProfile()
								 );
	ui->lineEditNickname->setText(nickname);
	this->clearLabels();
}

void Modify::clearLabels() {
	ui->lineEditNewPass->clear();
	ui->lineEditOldPass->clear();
	ui->lineEditConfirmPass->clear();
}

void Modify::on_lineEditNickname_editingFinished()
{
	QString nickname = ui->lineEditNickname->text();
	checkNickname(nickname);
}


void Modify::on_lineEditNickname_textChanged(const QString&)
{
	ui->labelInfoNick->clear();
}


bool Modify::checkNickname(const QString &nickname){

	if (nickname.isEmpty() || nickname.isNull()) {
		ui->labelInfoNick->setText("Empty nickname");
		return false;
	}
	return true;

}

void Modify::on_pushSaveNickname_clicked()
{
	QString nickname = ui->lineEditNickname->text();
	QString original = client->getNickname();

	if (nickname.compare(original) == 0){
		ui->labelInfoNick->setText("Nickname not modified");
		return;
	}

	if (checkNickname(nickname)) {
		QMessageBox msgbox;
		msgbox.setText("Are you sure?");
		msgbox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
		msgbox.setDefaultButton(QMessageBox::Save);
		if(msgbox.exec() == QMessageBox::Save){
			client->updateNickname(nickname);
		}
	}
}


void Modify::on_t_pushButtonResetNickname_clicked()
{
	QString nickname=client->getNickname();
	ui->lineEditNickname->setText(nickname);
}

void Modify::on_t_pushButtonResetPhoto_clicked()
{
	profile_photo_temp->load(":/images/anonymous");
    ui->profile_image->setCustomPixmap(*profile_photo_temp
								 );
}


void Modify::on_pushButtonSavephoto_clicked()
{
	QMessageBox msgbox;
	msgbox.setText("Are you sure?");
	msgbox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
	msgbox.setDefaultButton(QMessageBox::Save);
	if (msgbox.exec() == QMessageBox::Save) {
		client->overrideProfileImage(*this->profile_photo_temp);
		client->sendProfileImage();
	}
}

void Modify::on_t_pushButtonFinish_clicked()
{
    on_t_pushButtonCancel_clicked();
	changeWidget(HOME);
}

void Modify::on_t_pushButtonCancel_clicked()
{
	ui->lineEditOldPass->clear();
	ui->lineEditNewPass->clear();
	ui->lineEditConfirmPass->clear();
	ui->labelInfoPass->clear();
    ui->labelInfoOldPassword->clear();
}

// PASSWORD CHECKING

void Modify::checkPassword(const QString& password) {
    if (password.size() > 0) {
        QString msg;
        bool success = Signup::isValidPassword(password, msg,
                                               valid_new_password);
        if (!success) {
            ui->labelInfoPass->setText(msg);
        }

    }
}

bool Modify::checkConfirmation(const QString &pass,const QString &conf){
    if (conf.size() > 0 && valid_new_password == true) {
        int res = QString::compare(pass, conf, Qt::CaseSensitive);
        if (res != 0){
            ui->labelInfoPass->setText("Passwords don't match");
            return false;
        }
    }
    return true;
}


void Modify::on_lineEditNewPass_textChanged(const QString &arg1)
{
    QString conf=ui->lineEditConfirmPass->text();
    ui->labelInfoPass->setText("");
    if (conf.size()==0){
        if (arg1.size()>0) {
            ui->lineEditConfirmPass->setDisabled(false);

        } else {
            ui->lineEditConfirmPass->setDisabled(true);
            ui->lineEditConfirmPass->clear();
        }
    }else{
        checkPassword(arg1);
        checkConfirmation(arg1,conf);
    }
}

void Modify::on_lineEditNewPass_editingFinished()
{
    QString password = ui->lineEditNewPass->text();
    checkPassword(password);
    if (valid_new_password == true) {
        if (ui->lineEditConfirmPass->text().size()>0)
            checkConfirmation(password,ui->lineEditConfirmPass->text());
        else
             ui->labelInfoPass->clear();
    }
}

void Modify::on_lineEditConfirmPass_textChanged()
{
    if(ui->labelInfoPass->text().contains("match"))
        ui->labelInfoPass->clear();
    QString password1 = ui->lineEditNewPass->text();
    QString password2 = ui->lineEditConfirmPass->text();

    checkConfirmation(password1,password2);

}


void Modify::on_lineEditConfirmPass_editingFinished()
{
    QString password1 = ui->lineEditNewPass->text();
    QString password2 = ui->lineEditConfirmPass->text();

    checkConfirmation(password1,password2);

}


void Modify::on_pushButtonSavePassword_clicked()
{
    QString oldpass = ui->lineEditOldPass->text();

    QString newpass = ui->lineEditNewPass->text();
    QString confirm = ui->lineEditConfirmPass->text();
    checkPassword(newpass);
    if (correct_old_password==UNCHECKED)
        client->checkOldPassword(oldpass);

    // not send the request only if it's sure that the current onld password is wrong; send when is unchekced(pending request with yet no response, check only on server, or when is correct;
    if (valid_new_password && correct_old_password
            && checkConfirmation(newpass,confirm)){
        QMessageBox msgbox;
        msgbox.setText("Are you sure?");
        msgbox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgbox.setDefaultButton(QMessageBox::Save);
        if (msgbox.exec()==QMessageBox::Save) {
            client->updatePassword(oldpass,newpass);
        }
    }
}

void Modify::on_lineEditOldPass_editingFinished()
{
	QString old_password = ui->lineEditOldPass->text();

	client->checkOldPassword(old_password);
}

void Modify::on_lineEditOldPass_textChanged()
{
    correct_old_password=UNCHECKED;
}

void Modify::on_failedUpdatePassword(const QString& reason)
{
    if (reason.contains("password") && !reason.contains("new"))
        ui->labelInfoOldPassword->setText(reason);
    else {
        if (reason.contains("Wrong password"))
             correct_old_password=WRONG;
        ui->labelInfoPass->setText(reason);
    }
}

void Modify::on_successUpdatePassword()
{
    correct_old_password=CORRECT;
    ui->labelInfoPass->setText("Password correctly updated");
}

void Modify::on_wrongOldPasswordEntered(const QString &reason)
{
    correct_old_password=WRONG;

	// Print message only if modify window is visible
	if (ui->groupBox->isVisible()) {
        ui->labelInfoOldPassword->setText(reason);
	}
}

void Modify::on_correctOldPasswordEntered()
{
    correct_old_password=CORRECT;
    ui->labelInfoOldPassword->clear();

}
