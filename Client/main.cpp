#include <QApplication>
#include "appMainWindow.h"
#include "client.h"

#define ADDR "127.0.0.1"
#define PORT 1500

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QString addr = ADDR;
	quint16 port = PORT;

	QStringList args = QCoreApplication::arguments();
	if (args.length() != 1 && args.length() != 3) {
		qDebug() << "Usage: ./Client <ip_addr> <port_number>";
		qDebug() << "If no argument provided localhost:1500 is used.";
		exit(-1);
	}

	if (args.length() == 3) {
		addr = args.at(1);

		bool ok;
		unsigned int tmp = args.at(2).toInt(&ok);
		if (ok) {
			port = tmp;
		}

		qDebug() << "addr:" << addr << "port:" << port;
	}

	AppMainWindow m(nullptr,new Client(nullptr, addr, port));
	m.show();
	return a.exec();
}
