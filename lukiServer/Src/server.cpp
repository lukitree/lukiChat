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
		}
	}

	std::cout << "<- User \"" << disconnectedUser->getFullID().toStdString() << "\" disconnected." << std::endl;

	clients.removeAll(disconnectedUser);

	QTimer::singleShot(0, this, SLOT(sendUserList()));
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
			setUsername(receiveSocket, message.data);
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
		userlist.data += i->getUserName() + " ";
	}

	out << userlist;

	for (auto i : clients)
	{
		i->getSocket()->write(block);
	}
}

void Server::sendToAll(QString message, User* user) const
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_5);

	message = user->getUserName() + ": " + message;

	std::cout << message.toStdString() << std::endl;

	// send to all clients except message owner
	Message packet;
	packet.type = Message::MSG;
	packet.data = message;

	out << packet;

	for (auto i : clients)
	{
		if (i->getUserName() != user->getUserName())
			i->getSocket()->write(block);
	}

	// send messge to owner
	sendToOwner(message, user);
}

void Server::sendToOwner(QString message, User* user) const
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_5);

	Message packet;
	packet.type = Message::MSGOWNR;
	packet.data = message;

	out << packet;

	user->getSocket()->write(block);
}

void Server::sendToUser(User& user) const
{
}

void Server::setUsername(QTcpSocket* socket, QString username)
{
	bool usernameTaken = true;
	int usernameNumberAppend = 1;
	const QString originalUsername = username;
	while (usernameTaken)
	{
		usernameTaken = false;
		for (auto i : clients)
		{
			if (i->getUserName() == username)
			{
				usernameTaken = true;
			}
		}

		if (usernameTaken)
		{
			username = originalUsername + QString::number(usernameNumberAppend);
			++usernameNumberAppend;
		}
	}

	for (auto i : clients)
	{
		if (i->getSocket() == socket)
		{
			i->setUserName(username);
			std::cout << "-> Received connection from \"" << i->getFullID().toStdString() << "\"." << std::endl;
		}
	}
	
}
