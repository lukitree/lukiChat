#include "server.h"
#include "message.h"

Server::Server(QObject *parent)
	: QTcpServer(parent)
{
	connect(this, SIGNAL(newConnection()), this, SLOT(addNewConnection()));
}

Server::~Server()
{
}

void Server::addNewConnection()
{
	User *newUser = new User;

	QTcpSocket* socket = newUser->getSocket();
	connect(socket, SIGNAL(disconected()), this, SLOT(onDisconnect()));
	connect(socket, SIGNAL(disconnected()), this, SLOT(sendUserList()));
	connect(socket, SIGNAL(readRead()), this, SLOT(receive()));
	clients.append(newUser);

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_5);

	Message message;
	message.data = "You are connected to " + this->serverAddress().toString() + ".";
	out << message;
	socket->write(block);
}

void Server::onDisconnect()
{
}

void Server::updateUserList()
{
}

void Server::receive()
{
}

void Server::sendToAll()
{
}

void Server::sendToUser(User& user)
{
}
