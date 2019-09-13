#include "signup.h"
#include "ui_signup.h"
#include <QRegularExpression>
#include <QDebug>

Signup::Signup(QWidget *parent,Client* client) :
    QWidget(parent),
    ui(new Ui::Signup),
    client(client)
{
    ui->setupUi(this);
    QPixmap pix(":/images/anonymous"); //cercare .png
    int w=170;
    int h=170;
    ui->profile_image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
    ui->lineEditConfirmPassword->setDisabled(true);
    valid=false;

    connect(client, &Client::signedUp, this, &Signup::signedUp);
    connect(client, &Client::signupError, this, &Signup::signupFailed);
    //connect(client, &Client::error, this, &Signup::error);


}


Signup::~Signup()
{
    delete ui;
}

void Signup::on_pushButtonClear_clicked()
{
    this->clearLineEdit();
    this->clearLabel();
    ui->lineEditConfirmPassword->setDisabled(true);
    valid=false;
}

void Signup::signedUp()
{
    qDebug() << "Singup succeeded.";
    ui->pushButtonSignup->setEnabled(true); //pulsante disabilitato in attesa della risosta dal server
    ui->pushButtonClear->setEnabled(true);
    ui->pushButtonBackLogin->setEnabled(true);
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
    ui->labelInfoPass->setText("Correclty signed up");

    client->disconnectFromHost();
}

void Signup::signupFailed(const QString &reason){
    qDebug() << "Signup failed.";
    client->disconnectFromHost();
    ui->labelInfoPass->setText(reason);
    ui->pushButtonSignup->setEnabled(true); //pulsante disabilitato in attesa della risosta dal server
    ui->pushButtonClear->setEnabled(true);
    ui->pushButtonBackLogin->setEnabled(true);

    client->disconnectFromHost();
}

void Signup::on_pushButtonSignup_clicked()
{
    QString username=ui->lineEditUsername->text();
    QString password=ui->lineEditPassword->text();
    QString confirm=ui->lineEditConfirmPassword->text();


    if (checkUsername(username) && valid && checkConfirmation(password,confirm)){
        ui->pushButtonSignup->setEnabled(false); //pulsante disabilitato in attesa della risosta dal server
        ui->pushButtonClear->setEnabled(false);
        ui->pushButtonBackLogin->setEnabled(false);
        client->signup(username,password);
    }
}

void Signup::on_lineEditUsername_editingFinished()
{
    QString username = ui->lineEditUsername->text();
    checkUsername(username);

}


void Signup::on_lineEditPassword_editingFinished()
{
    QString password=ui->lineEditPassword->text();
    checkPassword(password);
}

void Signup::on_lineEditConfirmPassword_editingFinished()
{
     QString password1=ui->lineEditPassword->text();
     QString password2=ui->lineEditConfirmPassword->text();

     checkConfirmation(password1,password2);

}

void Signup::on_lineEditUsername_textChanged(const QString&)
{
    ui->labelInfoUser->clear();
}

void Signup::on_lineEditPassword_textChanged(const QString& arg)
{
    ui->labelInfoPass->setText("");
    if (arg.size()>0)
        ui->lineEditConfirmPassword->setDisabled(false);
    else {
        ui->lineEditConfirmPassword->setDisabled(true);
        ui->lineEditConfirmPassword->clear();
    }
}

void Signup::on_lineEditConfirmPassword_textChanged(const QString&)
{
    if(valid==true)
         ui->labelInfoPass->setText("");
}

bool Signup::checkUsername(const QString &username){
    QRegularExpression re("^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$");
    QRegularExpressionMatch match = re.match(username);
    bool hasMatch = match.hasMatch();

    if (username.size()>0 && !hasMatch){
        ui->labelInfoUser->setText("Username non valido");
        return false;
    }
    return true;

}

void Signup::checkPassword(const QString &password){
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
        valid=t;
    }
}

bool Signup::checkConfirmation(const QString &pass,const QString &conf){
    if (conf.size()>0 && valid==true){
        int x = QString::compare(pass, conf, Qt::CaseSensitive);
        if (x!=0){
            ui->labelInfoPass->setText("Le password non corrispondono");
            return false;
        }
    }
    return true;
}


void Signup::on_pushButtonBackLogin_clicked()
{
    this->clearLabel();
    this->clearLineEdit();
    emit action(0);
}

void Signup::clearLabel(){
    ui->labelInfoPass->clear();
    ui->labelInfoUser->clear();
}

void Signup::clearLineEdit(){
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
    ui->lineEditConfirmPassword->clear();
}

void Signup::enableAllButtons()
{
    ui->pushButtonSignup->setEnabled(true);
    ui->pushButtonClear->setEnabled(true);
    ui->pushButtonBackLogin->setEnabled(true);
}
