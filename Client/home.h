#ifndef HOME_H
#define HOME_H

#include <QWidget>
#include "editor.h"

namespace Ui {
class Home;
}

class Home : public QWidget
{
    Q_OBJECT

public:
    explicit Home(QWidget *parent = 0);
    ~Home();

private slots:
    void on_pushButtonLogOut_clicked();
    void on_pushButtonNewFile_clicked();
    void move_to_home();

signals:
    void move_to_login_clicked();
    void move_to_editor_clicked();
    void move_to_home_clicked();

private:
    Ui::Home *ui;
    Editor *editor;
};

#endif // HOME_H
