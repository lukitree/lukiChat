#include "user.h"

User::User()
{
	this->username = "None";
	socket = nullptr;
}

User::~User()
{

}

void User::setUserName(QString name)
{
	this->username = name;
}

QString User::getUserName()
{
	return username;
}

void User::setSocket(QTcpSocket* socket)
{
	if (socket == nullptr)
		delete socket;
	this->socket = socket;
}

QTcpSocket* User::getSocket()
{
	return socket;
}
