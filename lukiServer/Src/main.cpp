#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <iostream>
#include "server.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	// Create server and listen
	Server *server = new Server(nullptr);
	if (!server->listen(QHostAddress::Any, 42069))
	{
		std::cerr << "Failed to start server!" << std::endl;
		return 1;
	}

	std::cout << "Server is listening on port " << server->serverPort() << ".\n" << std::endl;

	return a.exec();
}
