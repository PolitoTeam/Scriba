#include "AppMainWindow.h"
#include "ui_index.h"
#include <QStackedWidget>
#include <QLayout>
#include "login.h"
#include "editor.h"
#include "home.h"
#include "signup.h"

AppMainWindow::AppMainWindow(QWidget *parent,Client* c) :
    QMainWindow(parent),
    ui(new Ui::Index),
    client(c)
{
    ui->setupUi(this);
    client->setParent(this);

    QStackedWidget *stackedWidget=new QStackedWidget(this);
    QWidget* w=ui->widget;
    QLayout* layout=new QGridLayout(w);
    w->setLayout(layout);
    layout->addWidget(stackedWidget);

    login = new Login(stackedWidget,client);
    stackedWidget->addWidget(login);
    QObject::connect(login, &Login::access,stackedWidget,&QStackedWidget::setCurrentIndex);

    signup = new Signup(stackedWidget,client);
    stackedWidget->addWidget(signup);
    QObject::connect(signup, &Signup::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    home = new Home(stackedWidget);
    home->setClient(client);
    stackedWidget->addWidget(home);
    QObject::connect(home, &Home::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    editor = new Editor(stackedWidget);
    editor->setClient(client);
    stackedWidget->addWidget(editor);
    QObject::connect(editor, &Editor::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    connect(home, &Home::logOut, this, &AppMainWindow::on_logOut);
}

AppMainWindow::~AppMainWindow()
{
    delete ui;
}

void AppMainWindow::on_logOut()
{
    login->disconnect();
}
