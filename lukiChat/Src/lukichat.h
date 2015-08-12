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
	void receive();
	void sendUserName();
	void on_actionConnect_triggered();
	void error();

private:
	Ui::lukiChatClass ui;
	QTcpSocket* serverSocket;

	ConnectDialog* connectDialog;
};

#endif // LUKICHAT_H
