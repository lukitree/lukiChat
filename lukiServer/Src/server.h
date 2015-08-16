#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QList>
#include <memory>
#include <QIODevice>
#include <QDataStream>
#include <QTimer>
#include "user.h"
#include "message.h"

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
	void assignUsernameToClient(QTcpSocket* socket, QString username);

private:
	QList<User*> clients;

	void sendToAll(QString message, User* user) const;
	void sendToOwner(QString message, User* user) const;
	void sendToUser(User& user) const;
	void setUsername(QTcpSocket* socket, QString username);
	void sendPM(Message packet, QTcpSocket* fromSocket) const;
	void sendServerMessageToUser(QString message, User* user, QString color) const;
};

#endif // SERVER_H
