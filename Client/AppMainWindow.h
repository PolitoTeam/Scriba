#ifndef INDEX_H
#define INDEX_H

#include <QMainWindow>

namespace Ui {
class Index;
}

class Login;
class Signup;
class Home;
class Editor;
class Client;

class AppMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    AppMainWindow(QWidget *parent = nullptr,Client* c=nullptr);
    ~AppMainWindow();

private slots:
    void on_logOut();

private:
    Ui::Index *ui;
    Login* login;
    Signup* signup;
    Home* home;
    Editor* editor;
    Client* client;
};

#endif // INDEX_H
