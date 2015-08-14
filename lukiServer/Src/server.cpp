#include "server.h"
#include "message.h"
#include <iostream>

Server::Server(QObject *parent)
	: QTcpServer(parent)
{
	qRegisterMetaTypeStreamOperators<Message>("Message");

	QTimer *userListTimer = new QTimer(this);
	connect(userListTimer, SIGNAL(timeout()), this, SLOT(sendUserList()));
	userListTimer->start(60000);

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
	connect(socket, SIGNAL(disconnected()), this, SLOT(sendUserList()));
	connect(socket, SIGNAL(readyRead()), this, SLOT(receive()));
	clients.append(newUser);

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_5);

	Message message;
	message.type = Message::MSG;
	message.data = "-> You have connected to " + socket->localAddress().toString() + ".";
	out << message;
	socket->write(block);

	QTimer::singleShot(50, this, SLOT(sendUserList()));
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
			i->setUserName("");
		}
	}

	std::cout << "-> User \"" << disconnectedUser->getFullID().toStdString() << "\" disconnected." << std::endl;
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
					std::cout << "-> Received connection from \"" << i->getFullID().toStdString() << "\"." << std::endl;
				}
			}
			break;
		case Message::MSG:
			for (auto i : clients)
				if (i->getSocket() == receiveSocket)
					sendToAll(message.data, i);
			break;
	}
}

void Server::sendUserList()
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_5);

	Message userlist;
	userlist.type = Message::USRLST;
	userlist.data.clear();

	for (auto i : clients)
	{
		if (i->getUserName() == "")
			clients.removeAll(i);
		else
			userlist.data += i->getUserName() + " ";
	}

	out << userlist;

	for (auto i : clients)
	{
		i->getSocket()->write(block);
	}
}

void Server::sendToAll(QString message, User* user)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_5);

	message = user->getUserName() + ": " + message;

	std::cout << message.toStdString() << std::endl;

	Message packet;
	packet.type = Message::MSG;
	packet.data = message;

	out << packet;

	for (auto i : clients)
	{
		i->getSocket()->write(block);
	}
}

void Server::sendToUser(User& user)
{
}
