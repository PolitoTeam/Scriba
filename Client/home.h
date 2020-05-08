#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include "appMainWindow.h"
#include "editor.h"

namespace Ui {
class Home;
}

class Home : public QWidget
{
	Q_OBJECT

public:
	Home(QWidget *parent = nullptr,Client* client=nullptr);
	~Home();

private slots:
	void on_pushButtonLogOut_clicked();
	void on_pushButtonNewFile_clicked();
	void on_pushButtonModify_clicked();
	void on_pushButtonSharedLink_clicked();
	void showActiveFiles(bool shared);
	void on_pushButtonOpenFile_clicked();
	void newFileCompleted();
	void openFileCompleted();
	void newFileError(const QString& reason);
	void on_openFilesError(const QString& reason);
	void on_pushButtonOpenShared_clicked();

signals:
	void changeWidget(int);
	void logOut();
	void modify();
	void fileChosen(const QString& filename);
	void addCRDTterminator();

private:
	Ui::Home *ui;
	Client* client;
};

#endif // HOME_H
