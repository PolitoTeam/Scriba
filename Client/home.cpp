#include "home.h"
#include "ui_home.h"
#include <QDebug>

Home::Home(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home)
{
    ui->setupUi(this);
    editor = new Editor(this);

    connect(editor, &Editor::home_clicked, this, &Home::move_to_home_clicked);
}

Home::~Home()
{
    delete ui;
}

void Home::on_pushButtonLogOut_clicked()
{
//    this->hide();
//    parentWidget()->show();
    emit move_to_login_clicked();
}

void Home::on_pushButtonNewFile_clicked()
{
//    this->hide();
    editor->show();
    emit move_to_editor_clicked();
}

void Home::move_to_home()
{
//    this->show();
    emit move_to_home_clicked();
}


