#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class ServerWindow;
}
class Server;
class ServerWindow : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWindow)
public:
    explicit ServerWindow(QWidget *parent = nullptr, quint16 port = 0);
    ~ServerWindow();

private:
    Ui::ServerWindow *ui;
    Server *m_Server;
    quint16 port;

    void closeEvent(QCloseEvent *event);

private slots:
    void toggleStartServer();
};

#endif // SERVERWINDOW_H
