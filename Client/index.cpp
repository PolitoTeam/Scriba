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
    ui(new Ui::Index),
    client(new Client(this))
{
    ui->setupUi(this);

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
