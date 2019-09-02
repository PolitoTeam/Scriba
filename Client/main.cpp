#include <QApplication>

#include <QDebug>
#include <index.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Index m;

    m.show();

    return a.exec();
}
