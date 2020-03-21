#include "serverwindow.h"
#include "ui_serverwindow.h"
#include "server.h"
#include <QMessageBox>

ServerWindow::ServerWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerWindow)
    , m_Server(new Server(this,new Database()))
{
    ui->setupUi(this);
    connect(ui->pushButton_startStop, &QPushButton::clicked, this, &ServerWindow::toggleStartServer);
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::toggleStartServer()
{
    if (m_Server->isListening()) {
        m_Server->stopServer();
        ui->pushButton_startStop->setText(tr("START"));
        ui->label_status->setText("Stopped");
    } else {
        if (!m_Server->tryConnectionToDatabase()) {
            QMessageBox::critical(this, tr("Cannot open database"),
                        tr("Unable to establish a database connection.\n"), QMessageBox::Close);
            return;
        }

        if (!m_Server->listen(QHostAddress::LocalHost, 1500)) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to start the server"), QMessageBox::Close);
            return;
        }

        ui->pushButton_startStop->setText(tr("STOP"));
        ui->label_status->setText("Running");
    }
}

void ServerWindow::closeEvent (QCloseEvent *)
{
    this->m_Server->stopServer();
}
