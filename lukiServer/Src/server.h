#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QList>
#include <memory>
#include <QIODevice>
#include <QDataStream>
#include "user.h"

class Server : public QTcpServer
{
	Q_OBJECT

public:
	Server(QObject *parent);
	~Server();

private slots:
	void addNewConnection();
	void onDisconnect();
	void updateUserList();
	void receive();

private:
	QList<User*> clients;

	void sendToAll();
	void sendToUser(User& user);
};

#endif // SERVER_H
