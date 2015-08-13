#include "server.h"
#include "message.h"
#include <iostream>

Server::Server(QObject *parent)
	: QTcpServer(parent)
{
	qRegisterMetaTypeStreamOperators<Message>("Message");
	connect(this, SIGNAL(newConnection()), this, SLOT(addNewConnection()));
}

Server::~Server()
{
}

void Server::addNewConnection()
{
	User *newUser = new User;

	newUser->setSocket(this->nextPendingConnection());
	QTcpSocket* socket = newUser->getSocket();
	connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	connect(socket, SIGNAL(disconnected()), this, SLOT(updateUserList()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(receive()));
	clients.append(newUser);

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_5);

	Message message;
	message.type = Message::MSG;
	message.data = "You have connected to " + socket->localAddress().toString() + ".";
	out << message;
	socket->write(block);
}

void Server::onDisconnect()
{
	QTcpSocket* disconnectedSocket = qobject_cast<QTcpSocket*>(sender());
	User* disconnectedUser;
	for (auto i : clients)
	{
		if (i->getSocket() == disconnectedSocket)
		{
			disconnectedUser = i;
		}
	}

	std::cout << "User \"" << disconnectedUser->getFullID().toStdString() << "\" disconnected." << std::endl;
}

void Server::updateUserList()
{
}

void Server::receive()
{
	QTcpSocket* receiveSocket = qobject_cast<QTcpSocket*>(sender());
	QDataStream in(receiveSocket);

	Message message;
	in >> message;

	switch (message.type)
	{
		case Message::USR:
			for (auto i : clients)
			{
				if (i->getSocket() == receiveSocket)
				{
					i->setUserName(message.data);
					std::cout << "Received connection from \"" << i->getFullID().toStdString() << "\"." << std::endl;
				}
			}
	}
}

void Server::sendToAll()
{
}

void Server::sendToUser(User& user)
{
}
