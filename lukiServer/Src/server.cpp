#include "server.h"
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
	out.setVersion(QDataStream::Qt_5_0);

	Message message;
	message.type = Message::SVRMSG;
	message.data = "-> You have connected to " + socket->localAddress().toString() + ".";
	message.extra = "Dark Green";
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
		case Message::PM:
			sendPM(message, receiveSocket);
	}
}

void Server::sendUserList()
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_0);

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
	out.setVersion(QDataStream::Qt_5_0);

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
	out.setVersion(QDataStream::Qt_5_0);

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
	
	//QTimer::singleShot(0, this, SLOT(assignUsernameToClient(socket, username)));
	assignUsernameToClient(socket, username);
}

void Server::assignUsernameToClient(QTcpSocket* socket, QString username)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_0);

	Message packet;
	packet.type = Message::USR;
	packet.data = username;

	out << packet;

	socket->write(block);
}

void Server::sendPM(Message packet, QTcpSocket* fromSocket) const
{
	QString message = packet.data;
	User* toUser = nullptr;
	User* fromUser = nullptr;

	// get pointer for fromUser
	for (auto i : clients)
	{
		if (i->getSocket() == fromSocket)
			fromUser = i;
	}

	// get pointer for toUser
	for (auto i : clients)
	{
		if (i->getUserName() == packet.extra)
			toUser = i;
	}

	if (toUser == nullptr)
	{
		QString errorMessage = "[!] Could not PM \"" + packet.extra + "\", user not found.";
		sendServerMessageToUser(errorMessage, fromUser, "Red");
	}
	else
	{
		QByteArray block;
		QDataStream out(&block, QIODevice::WriteOnly);
		out.setVersion(QDataStream::Qt_5_0);

		Message pmPacket;
		pmPacket.type = Message::PM;
		pmPacket.data = packet.data;
		pmPacket.extra = toUser->getUserName() + " " + fromUser->getUserName();

		out << pmPacket;

		toUser->getSocket()->write(block);
		fromUser->getSocket()->write(block);

		std::cout << "PM " << fromUser->getUserName().toStdString() << " -> " << toUser->getUserName().toStdString() << ": " << packet.data.toStdString() << std::endl;
	}
}

void Server::sendServerMessageToUser(QString message, User* user, QString color) const
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_0);

	Message packet;
	packet.type = Message::SVRMSG;
	packet.data = message;
	packet.extra = color;

	out << packet;

	user->getSocket()->write(block);
}
