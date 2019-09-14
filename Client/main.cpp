#include <QApplication>
#include "appMainWindow.h"
#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppMainWindow m(nullptr,new Client()); //instanziazione finestra grafica

    m.show();

    return a.exec();
}
