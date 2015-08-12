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

	std::cout << "Received connection from " << socket->peerAddress().toString().toStdString() << std::endl;

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
