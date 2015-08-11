#ifndef USER_H
#define USER_H

#include <QTcpSocket>
#include <QString>
#include <memory>

class User
{
public:
	User();
	~User();

	void setUserName(QString name);
	QString getUserName();

	void setSocket(QTcpSocket* socket);
	QTcpSocket* getSocket();

private:
	QString username;
	QTcpSocket* socket;
};

#endif // USER_H
