#include "modify.h"
#include "ui_modify.h"
#include "QFileDialog"

Modify::Modify(QWidget *parent,Client* client) :
    QWidget(parent),
    ui(new Ui::modify),
    client(client)
{
    ui->setupUi(this);
    QPixmap pix(":/images/anonymous"); //cercare .png
    ui->profile_image->setPixmap(pix.scaled(IMAGE_WIDTH, IMAGE_HEIGHT, Qt::KeepAspectRatio));
}

Modify::~Modify()
{
    delete ui;
}

void Modify::on_pushButtonUpload_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), QDir::homePath()); //specificare path
    qDebug()<<"Selected image: "<<fileName;
    QPixmap image(fileName);
    ui->profile_image->setPixmap(image.scaled(IMAGE_WIDTH, IMAGE_HEIGHT, Qt::KeepAspectRatio));
}
