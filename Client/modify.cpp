#include "modify.h"
#include "signup.h"
#include "ui_modify.h"
#include <QFileDialog>
#include <QRegularExpression>
#include <QMessageBox>

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
}

Modify::~Modify()
{
	delete ui;
}

void Modify::on_pushButtonUpload_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(
				this,
				tr("Open Image"), QDir::homePath(),
				tr("Image Files (*.png *.jpg *.bmp)")
	);

	if (!fileName.isEmpty() && !fileName.isNull()) {
		profile_photo_temp->load(fileName);
		ui->profile_image->setPixmap(profile_photo_temp->scaled(
											IMAGE_WIDTH, IMAGE_HEIGHT,
											Qt::KeepAspectRatioByExpanding)
									 );
	}
}

void Modify::upload() {
	QString nickname = client->getNickname();
	ui->profile_image->setPixmap(client->getProfile()->scaled(
									 IMAGE_WIDTH, IMAGE_HEIGHT,
									 Qt::KeepAspectRatioByExpanding)
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

void Modify::on_lineEditNickname_textChanged(const QString&)
{
	ui->labelInfoNick->clear();
}

void Modify::on_lineEditNewPass_textChanged(const QString &arg1)
{
	ui->labelInfoPass->setText("");
	if (arg1.size()>0) {
		ui->lineEditConfirmPass->setDisabled(false);
	} else {
		ui->lineEditConfirmPass->setDisabled(true);
		ui->lineEditConfirmPass->clear();
	}
}

void Modify::on_lineEditConfirmPass_textChanged(const QString&)
{
	if (valid_new_password == true) {
		ui->labelInfoPass->clear();
	}
}


void Modify::on_lineEditNewPass_editingFinished()
{
	QString password = ui->lineEditNewPass->text();
	checkPassword(password);
}

void Modify::on_lineEditConfirmPass_editingFinished()
{
	QString password1 = ui->lineEditNewPass->text();
	QString password2 = ui->lineEditConfirmPass->text();

	checkConfirmation(password1,password2);
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

void Modify::on_pushButtonSavePassword_clicked()
{
	QString oldpass = ui->lineEditOldPass->text();

    status=1;
    client->checkOldPassword(oldpass);


}

void Modify::continueSaving(){
    status=0;
    QString newpass = ui->lineEditNewPass->text();
    QString confirm = ui->lineEditConfirmPass->text();
    QString oldpass = ui->lineEditOldPass->text();
    checkPassword(newpass);
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

void Modify::on_pushButtonResetNickname_clicked()
{
	QString nickname=client->getNickname();
	ui->lineEditNickname->setText(nickname);
}

void Modify::on_pushButtonResetPhoto_clicked()
{
	profile_photo_temp->load(":/images/anonymous");
	ui->profile_image->setPixmap(profile_photo_temp->scaled(
									 IMAGE_WIDTH, IMAGE_HEIGHT,
									 Qt::KeepAspectRatioByExpanding)
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

void Modify::on_pushButtonFinish_clicked()
{
	on_pushButtonCancel_clicked();
	changeWidget(HOME);
}

void Modify::on_pushButtonCancel_clicked()
{
	ui->lineEditOldPass->clear();
	ui->lineEditNewPass->clear();
	ui->lineEditConfirmPass->clear();
	ui->labelInfoPass->clear();
    ui->labelInfoOldPassword->clear();
}

void Modify::on_lineEditOldPass_editingFinished()
{
	QString old_password = ui->lineEditOldPass->text();

	client->checkOldPassword(old_password);
}

void Modify::on_wrongOldPasswordEntered()
{
    correct_old_password = false;
	// Print message only if modify window is visible


	if (ui->groupBox->isVisible()) {
        ui->labelInfoOldPassword->setText("Wrong old password");
	}
    if (status==1)
        continueSaving();
}

void Modify::on_correctOldPasswordEntered()
{
    correct_old_password= true;
    status=1;
    ui->labelInfoOldPassword->clear();
    if (status==1)
        continueSaving();
}
