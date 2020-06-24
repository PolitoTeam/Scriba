#include <QInputDialog>
#include <QDir>
#include <QMessageBox>
#include "home.h"
#include "ui_home.h"
#include "client.h"
#include "CRDT.h"

Home::Home(QWidget *parent,Client* client) :
	QWidget(parent),
	ui(new Ui::Home),
	client(client)
{
	ui->setupUi(this);
	connect(client, &Client::filesReceived, this, &Home::showActiveFiles);
	connect(client, &Client::openFilesError, this, &Home::on_openFilesError);
	connect(client, &Client::correctNewFile, this, &Home::newFileCompleted);
	connect(client, &Client::wrongNewFile, this, &Home::newFileError);
	connect(client, &Client::correctOpenedFile, this, &Home::openFileCompleted);
	connect(client, &Client::wrongListFiles, this, &Home::on_openFilesError);
	connect(client, &Client::wrongSharedLink, this, &Home::on_openFilesError);
	connect(this, &Home::fileChosen,client,&Client::openFile);
}

Home::~Home()
{
	delete ui;
}

void Home::on_pushButtonLogOut_clicked()
{
	emit logOut();
    emit changeWidget(LOGIN);
}

void Home::on_pushButtonNewFile_clicked()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("New File"),
										 tr("File name:"), QLineEdit::Normal,
										 tr(""), &ok);

	if (ok && !text.isEmpty()) {
		// To update the window title in the editor
		client->setOpenedFile(text + "," + client->getUsername());

		client->createNewFile(text);
	}
}

void Home::newFileCompleted() {
	emit addCRDTterminator();
	emit changeWidget(EDITOR);
}

void Home::openFileCompleted() {
	emit changeWidget(EDITOR);
}

void Home::newFileError(const QString& reason) {
	QMessageBox::critical(this, tr("Error"),
						  reason, QMessageBox::Close);
	on_pushButtonNewFile_clicked();
}

void Home::on_openFilesError(const QString& reason) {
	QMessageBox::critical(this, tr("Error"), reason, QMessageBox::Close);
}

void Home::on_pushButtonModify_clicked()
{
	emit modify();
	emit changeWidget(MODIFY);
}

void Home::on_pushButtonSharedLink_clicked()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Shared Link"),
										 tr("Paste link here:"),
										 QLineEdit::Normal, tr(""), &ok);
	if (ok && !text.isEmpty()) {
		client->getFilenameFromLink(text);
	}
}

void Home::on_pushButtonOpenFile_clicked()
{
	client->getFiles(false);
}

void Home::on_pushButtonOpenShared_clicked()
{
	client->getFiles(true);
}

void Home::showActiveFiles(bool shared){
	QStringList items;
	QList<QPair<QString,QString>> map_files = client->getActiveFiles();

	qDebug() << map_files;
	for (auto i = map_files.begin(); i != map_files.end(); ++i){
		if (!shared) {
			items << QString(i->first);
		} else {
			items << QString(i->first + " (" + i->second + ")");
		}
	}
	// Sort entry by filename
	items.sort();

	bool ok;
	QString item = QInputDialog::getItem(this, tr("Open File"),
										 tr("Files:"), items, 0, false, &ok);
	if (ok && !item.isEmpty()) {
		// Format to obtain choice = "filename,user"
		QString choice;
		if (!shared) {
			choice = item + "," + client->getUsername();
		} else {
			choice = item.replace(" (", ",").replace(")", "");
		}
		emit fileChosen(choice);
	}
}

void Home::setProfile(){
    this->ui->usernameLabel->setText(this->client->getUsername());
    this->ui->nickNameLabel->setText(this->client->getNickname());
    this->ui->profileImage->setCustomPixmap(*this->client->getProfile());
}
