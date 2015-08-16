#ifndef LUKICHAT_H
#define LUKICHAT_H

#include <QtWidgets/QMainWindow>
#include <QTcpSocket>
#include "ui_lukichat.h"
#include "connectdialog.h"

class lukiChat : public QMainWindow
{
	Q_OBJECT

public:
	lukiChat(QWidget *parent = 0);
	~lukiChat();

private slots:
	void onConnect();
	void onDisconnect();
	void receive();
	void sendUserName();
	void on_actionConnect_triggered();
	void on_actionReconnect_triggered();
	void on_actionDisconnect_triggered();
	void on_sendButton_clicked();
	void error();

private:
	Ui::lukiChatClass ui;
	QTcpSocket* serverSocket;

	ConnectDialog* connectDialog;
	QString assignedUsername;

	void printMessage(QString message);
	void printOwnedMessage(QString message);
	void processUserList(QString users);
	void setupAssignedUsername(QString username);
};

#endif // LUKICHAT_H
