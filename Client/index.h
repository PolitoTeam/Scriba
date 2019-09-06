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

class Index : public QMainWindow
{
    Q_OBJECT

public:
    explicit Index(QWidget *parent = nullptr);
    ~Index();

private slots:
    void on_logOut();

private:
    Ui::Index *ui;
    Login* login;
    Signup* signup;
    Home* home;
    Editor* editor;
};

#endif // INDEX_H
