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
    ui->lineEditPassword_3->setDisabled(true);
    valid=false;


}

void Signup::setClient(Client *client){
    this->client=client;
}

Signup::~Signup()
{
    delete ui;
}

void Signup::on_pushButtonLogin_2_clicked()
{
    ui->lineEditUsername->clear();
    ui->lineEditPassword->clear();
    ui->lineEditPassword_3->clear();
    ui->label_info_pass->clear();
    ui->label_info_user->clear();
    ui->lineEditPassword_3->setDisabled(true);
    valid=false;
}

void Signup::on_pushButtonLogin_clicked()
{
    QString username=ui->lineEditUsername->text();
    QString password=ui->lineEditPassword->text();
    QString confirm=ui->lineEditPassword_3->text();

   // ui->pushButtonLogin->setEnabled(false); //pulsante disabilitato in attesa della risosta dal server
    if (checkUsername(username) && valid && checkConfirmation(password,confirm)){
        client->signup(username,password);
        emit action(0);
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

void Signup::on_lineEditPassword_3_editingFinished()
{
     QString password1=ui->lineEditPassword->text();
     QString password2=ui->lineEditPassword_3->text();

     checkConfirmation(password1,password2);

}

void Signup::on_lineEditUsername_textChanged(const QString &arg1)
{
    ui->label_info_user->clear();
}

void Signup::on_lineEditPassword_textChanged(const QString &arg1)
{
    ui->label_info_pass->setText("");
    if (arg1.size()>0)
        ui->lineEditPassword_3->setDisabled(false);
    else {
        ui->lineEditPassword_3->setDisabled(true);
        ui->lineEditPassword_3->clear();
    }
}

void Signup::on_lineEditPassword_3_textChanged(const QString &arg1)
{
    if(valid==true)
         ui->label_info_pass->setText("");
}

bool Signup::checkUsername(const QString &username){
    QRegularExpression re("^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$");
    QRegularExpressionMatch match = re.match(username);
    bool hasMatch = match.hasMatch();

    if (username.size()>0 && !hasMatch){
        ui->label_info_user->setText("Username non valido");
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
            ui->label_info_pass->setText("Min. 8 caratteri, Max 12 caratteri");
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
            ui->label_info_pass->setText(info);
         }
        valid=t;
    }
}

bool Signup::checkConfirmation(const QString &pass,const QString &conf){
    if (conf.size()>0 && valid==true){
        int x = QString::compare(pass, conf, Qt::CaseSensitive);
        if (x!=0){
            ui->label_info_pass->setText("Le password non corrispondono");
        }
    }
}

