#include <QApplication>
#include <QDebug>
#include <stdlib.h>
#include "server.h"

#define PORT 1500

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);
	quint16 port = PORT;

	QStringList args = QCoreApplication::arguments();
	if (args.length() != 1 && args.length() != 2) {
		qDebug() << "Usage: ./Server <port_number>";
		qDebug() << "If no argument provided port 1500 is used.";
		exit(-1);
	}

	if (args.length() == 2) {
		bool ok;
		unsigned int tmp = args.at(1).toUInt(&ok);
		if (ok) {
			port = tmp;
		}
	}

	Server *m_Server = new Server();
	if (!m_Server->tryConnectionToMongo()) {
		qDebug() << "Unable to establish a database connection.\n";
		a.exit(-1);
	}

	if (!m_Server->listen(QHostAddress::LocalHost, port)) {
		qDebug() << "Unable to start the server";
		a.exit(-1);
	}

	return a.exec();
}
