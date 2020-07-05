#ifndef INDEX_H
#define INDEX_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QStackedWidget>
#include <QLineEdit>

typedef enum {LOGIN, SIGNUP, HOME, MODIFY, EDITOR } Widget;

namespace Ui {
class Index;
}

class Login;
class Signup;
class Home;
class Editor;
class Client;
class Modify;

class AppMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	AppMainWindow(QWidget *parent = nullptr,Client* c=nullptr);
	~AppMainWindow();
    static void errorLineEdit(QLineEdit* lineEdit,bool f);

private slots:
	void on_logOut();
	void error(QAbstractSocket::SocketError socketError);
	void on_changeWidget(int widget);
	void on_changeLoginLabel(const QString& label);

private:
	Ui::Index *ui;
	QStackedWidget *stackedWidget;
	Login* login;
	Signup* signup;
	Home* home;
	Editor* editor;
	Client* client;
	Modify* modify;
};

#endif // INDEX_H
