#include "modify.h"
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

    connect(client, &Client::wrongOldPassword, this, &Modify::on_wrongOldPasswordEntered);
    connect(client, &Client::correctOldPassword, this, &Modify::on_correctOldPasswordEntered);
}

Modify::~Modify()
{
    delete ui;
}

void Modify::on_pushButtonUpload_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp)")); //specificare path
    //qDebug()<<"Selected image: "<<fileName;
    if (!fileName.isEmpty() && !fileName.isNull()){
        profile_photo_temp->load(fileName);
        ui->profile_image->setPixmap(profile_photo_temp->scaled(IMAGE_WIDTH, IMAGE_HEIGHT, Qt::KeepAspectRatioByExpanding));
    }
}

void Modify::upload(){
    QString nickname=client->getNickname();
    ui->profile_image->setPixmap(client->getProfile()->scaled(IMAGE_WIDTH, IMAGE_HEIGHT, Qt::KeepAspectRatioByExpanding));
    ui->lineEditNickname->setText(nickname);
    this->clearLabels();
}

void Modify::clearLabels(){
    ui->lineEditNewPass->clear();
    ui->lineEditOldPass->clear();
    ui->lineEditConfirmPass->clear();
}

void Modify::on_lineEditNickname_editingFinished()
{
    QString nickname = ui->lineEditNickname->text();
    checkNickname(nickname);
}


void Modify::checkPassword(const QString &password){
    if (password.size()>0){
        bool t=true;
        //lunghezza
        if (password.size()<8 || password.size()>12)
        {
            t=false;
            ui->labelInfoPass->setText("Min. 8 caratteri, Max 12 caratteri");
        }
        else
        {
            QString info("");
            //caratteri non consentiti
            QRegularExpression r("[^A-Za-z0-9@\\.\\-_]");
            QRegularExpressionMatchIterator m=r.globalMatch(password);
            if (m.hasNext()){
                t=false;
                info.append("Caratteri ammessi: A-Z, a-z, 0-9, @ . _ -\n");
            }

            //almeno una lettera maiuscola
            r.setPattern("[A-Z]");
            m=r.globalMatch(password);
            if (!m.hasNext()){
                t=false;
                info.append("Almeno 1 lettera maiuscola\n");
            }

            //almeno una lettera minuscola
            r.setPattern("[a-z]");
            m=r.globalMatch(password);
            if (!m.hasNext()){
                t=false;
                info.append("Almeno 1 lettera minuscola\n");
            }

            //almeno una cifra
            r.setPattern("[0-9]");
            m=r.globalMatch(password);
            if (!m.hasNext()){
                t=false;
                info.append("Almeno 1 cifra\n");
            }

            //almeno un carattere speciale
            r.setPattern("[@\\.\\-_]");
            m=r.globalMatch(password);
            if (!m.hasNext()){
                t=false;
                info.append("Almeno 1 carattere speciale");
            }
            ui->labelInfoPass->setText(info);
         }
        valid_new_password=t;
    }
}

bool Modify::checkConfirmation(const QString &pass,const QString &conf){
    if (conf.size()>0 && valid_new_password==true){
        int x = QString::compare(pass, conf, Qt::CaseSensitive);
        if (x!=0){
            ui->labelInfoPass->setText("Le password non corrispondono");
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
    if (arg1.size()>0)
        ui->lineEditConfirmPass->setDisabled(false);
    else {
        ui->lineEditConfirmPass->setDisabled(true);
        ui->lineEditConfirmPass->clear();
    }
}

void Modify::on_lineEditConfirmPass_textChanged(const QString&)
{
    if(valid_new_password==true)
         ui->labelInfoPass->clear();
}


void Modify::on_lineEditNewPass_editingFinished()
{
    QString password=ui->lineEditNewPass->text();
    checkPassword(password);
}

void Modify::on_lineEditConfirmPass_editingFinished()
{
    QString password1=ui->lineEditNewPass->text();
    QString password2=ui->lineEditConfirmPass->text();

    checkConfirmation(password1,password2);
}

bool Modify::checkNickname(const QString &nickname){

    if (nickname.isEmpty() || nickname.isNull()){
        ui->labelInfoNick->setText("Nickname empty");
        return false;
    }
    return true;

}

void Modify::on_pushSaveNickname_clicked()
{
    QString nickname=ui->lineEditNickname->text();
    QString original=client->getNickname();

    if (nickname.compare(original)==0){
        ui->labelInfoNick->setText("Nickname not modified");
        return;
    }

    if (checkNickname(nickname)){
        QMessageBox msgbox;
        msgbox.setText("Are you sure?");
        msgbox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgbox.setDefaultButton(QMessageBox::Save);
        if(msgbox.exec()==QMessageBox::Save){
            //qDebug()<<"client->updateNickname";
           client->updateNickname(nickname);
        }
    }
}

void Modify::on_pushButtonSavePassword_clicked()
{
    QString oldpass=ui->lineEditOldPass->text();
    QString newpass=ui->lineEditNewPass->text();
    QString confirm=ui->lineEditConfirmPass->text();


    if (valid_new_password && correct_old_password && checkConfirmation(newpass,confirm)){
        QMessageBox msgbox;
        msgbox.setText("Are you sure?");
        msgbox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgbox.setDefaultButton(QMessageBox::Save);
        if(msgbox.exec()==QMessageBox::Save)
           client->updatePassword(oldpass,newpass);
        //disabilita pulsanti;
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
    ui->profile_image->setPixmap(profile_photo_temp->scaled(IMAGE_WIDTH, IMAGE_HEIGHT, Qt::KeepAspectRatioByExpanding));
}


void Modify::on_pushButtonSavephoto_clicked()
{
    QMessageBox msgbox;
    msgbox.setText("Are you sure?");
    msgbox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    msgbox.setDefaultButton(QMessageBox::Save);
    if(msgbox.exec()==QMessageBox::Save) {
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
}

void Modify::on_lineEditOldPass_editingFinished()
{
    QString old_password = ui->lineEditOldPass->text();
    client->checkOldPassword(old_password);
}

void Modify::on_wrongOldPasswordEntered()
{
    correct_old_password = false;
    if (ui->groupBox->isVisible()) // print message only if modify window is visible
        ui->labelInfoPass->setText("Wrong old password");
}

void Modify::on_correctOldPasswordEntered()
{
    correct_old_password = true;
    ui->labelInfoPass->clear();
}
