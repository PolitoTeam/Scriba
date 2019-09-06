#include "index.h"
#include "ui_index.h"
#include <QStackedWidget>
#include <QLayout>
#include "login.h"
#include "editor.h"
#include "home.h"
#include "signup.h"
#include "database.h"

Index::Index(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Index)
{
    ui->setupUi(this);

    Database b;

    QStackedWidget *stackedWidget=new QStackedWidget(this);
    QWidget* w=ui->widget;
    QLayout* layout=new QGridLayout(w);
    w->setLayout(layout);
    layout->addWidget(stackedWidget);



    login = new Login(stackedWidget);
    stackedWidget->addWidget(login);
    QObject::connect(login, &Login::access,stackedWidget,&QStackedWidget::setCurrentIndex);

    signup = new Signup(stackedWidget);
    stackedWidget->addWidget(signup);
    QObject::connect(signup, &Signup::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    home = new Home(stackedWidget);
    stackedWidget->addWidget(home);
    QObject::connect(home, &Home::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    editor = new Editor(stackedWidget);
    stackedWidget->addWidget(editor);
    QObject::connect(editor, &Editor::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    connect(home, &Home::logOut, this, &Index::on_logOut);
}

Index::~Index()
{
    delete ui;
}

void Index::on_logOut()
{
    login->disconnect();
}
