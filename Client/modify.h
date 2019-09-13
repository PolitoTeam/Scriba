#ifndef MODIFY_PROFILE_H
#define MODIFY_PROFILE_H

#include <QWidget>
#include "client.h"

namespace Ui {
class modify;
}

class Modify : public QWidget
{
    Q_OBJECT

public:
    Modify(QWidget *parent = nullptr,Client* client=nullptr);
    ~Modify();

signals:
    void action(int);

private:
    Ui::modify *ui;
    Client* client;
};

#endif // MODIFY_PROFILE_H
