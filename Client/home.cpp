#include "home.h"
#include "ui_home.h"

Home::Home(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Home)
{
    ui->setupUi(this);
    editor = new Editor(this);
}

Home::~Home()
{
    delete ui;
}

void Home::on_pushButtonLogOut_clicked()
{
    this->hide();
    parentWidget()->show();
}

void Home::on_pushButtonNewFile_clicked()
{
    this->hide();
    editor->show();
}
