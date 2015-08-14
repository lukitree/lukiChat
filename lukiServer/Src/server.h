#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QList>
#include <memory>
#include <QIODevice>
#include <QDataStream>
#include <QTimer>
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
	void receive();
	void sendUserList();

private:
	QList<User*> clients;

	void sendToAll(QString message, User* user);
	void sendToUser(User& user);
};

#endif // SERVER_H
