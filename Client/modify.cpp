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
    int w=170;
    int h=170;
    ui->profile_image->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
}

Modify::~Modify()
{
    delete ui;
}

void Modify::on_pushButtonUpload_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/Users/giuseppe.pastore/Desktop", tr("Image Files (*.png *.jpg *.bmp)")); //specificare path
    qDebug()<<"Selected image: "<<fileName;
}
