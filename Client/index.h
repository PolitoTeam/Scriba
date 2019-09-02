#ifndef INDEX_H
#define INDEX_H

#include <QMainWindow>

namespace Ui {
class Index;
}

class Index : public QMainWindow
{
    Q_OBJECT

public:
    explicit Index(QWidget *parent = nullptr);
    ~Index();

private:
    Ui::Index *ui;
};

#endif // INDEX_H
