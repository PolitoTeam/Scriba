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



    Login* l=new Login(stackedWidget);
    stackedWidget->addWidget(l);
    QObject::connect(l, &Login::access,stackedWidget,&QStackedWidget::setCurrentIndex);

    Signup* s=new Signup(stackedWidget);
    stackedWidget->addWidget(s);
    QObject::connect(s, &Signup::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    Home* h=new Home(stackedWidget);
     stackedWidget->addWidget(h);
    QObject::connect(h, &Home::action,stackedWidget,&QStackedWidget::setCurrentIndex);

    Editor* e=new Editor(stackedWidget);
    stackedWidget->addWidget(e);
    QObject::connect(e, &Editor::action,stackedWidget,&QStackedWidget::setCurrentIndex);

}

Index::~Index()
{
    delete ui;
}
