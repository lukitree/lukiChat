#include "lukichat.h"
#include "message.h"

lukiChat::lukiChat(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	qRegisterMetaTypeStreamOperators<Message>("Message");

	connectDialog = new ConnectDialog(this);

	serverSocket = new QTcpSocket(this);

	connect(serverSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
	connect(serverSocket, SIGNAL(readyRead()), this, SLOT(receive()));
	connect(ui.actionbarConnect, SIGNAL(triggered()), this, SLOT(on_actionReconnect_triggered()));
	connect(ui.actionbarDisconnect, SIGNAL(triggered()), this, SLOT(on_actionDisconnect_triggered()));
	connect(serverSocket, SIGNAL(connected()), this, SLOT(sendUserName()));
	connect(serverSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
}

lukiChat::~lukiChat()
{

}

void lukiChat::onConnect()
{
}

void lukiChat::onDisconnect()
{
	printMessage("-> You have been disconnected.");
	ui.userList->clear();
}

void lukiChat::receive()
{
	QDataStream in(serverSocket);
	in.setVersion(QDataStream::Qt_5_5);

	Message message;
	in >> message;

	switch (message.type)
	{
		case Message::MSG:
			printMessage(message.data);
			break;
		case Message::MSGOWNR:
			printOwnedMessage(message.data);
			break;
		case Message::USRLST:
			processUserList(message.data);
			break;
	}
}

void lukiChat::sendUserName()
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_5);

	Message message;
	message.type = Message::USR;
	message.data = connectDialog->usernameEdit->text();

	out << message;
	serverSocket->write(block);
}

void lukiChat::on_actionConnect_triggered()
{
	QString port = connectDialog->portEdit->text();
	QString address = connectDialog->serverEdit->text();
	QString username = connectDialog->usernameEdit->text();

	if (connectDialog->exec() && !port.isEmpty() && !address.isEmpty() && !username.isEmpty())
	{
		serverSocket->abort();
		serverSocket->connectToHost(address, port.toInt());
		// connect to server
	}
}

void lukiChat::on_actionReconnect_triggered()
{
	QString port = connectDialog->portEdit->text();
	QString address = connectDialog->serverEdit->text();
	QString username = connectDialog->usernameEdit->text();
	
	if (!port.isEmpty() && !address.isEmpty() && !username.isEmpty())
	{
		serverSocket->abort();
		serverSocket->connectToHost(address, port.toInt());
	}
}

void lukiChat::on_actionDisconnect_triggered()
{
	serverSocket->disconnectFromHost();
}

void lukiChat::on_sendButton_clicked()
{
	QString text = ui.messageEdit->text();
	ui.messageEdit->clear();

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_5);

	Message message;
	message.type = Message::MSG;
	message.data = text;

	out << message;

	serverSocket->write(block);
}

void lukiChat::processUserList(QString users)
{
	ui.userList->clear();
	QStringList userlist = users.split(" ", QString::SkipEmptyParts);

	// populate userlist in ui
	for (auto i : userlist)
	{
		new QListWidgetItem(i, ui.userList);
	}
}

void lukiChat::error()
{
	new QListWidgetItem("-> Failed to connect to host.", ui.messageList);
	ui.messageList->scrollToBottom();
}

void lukiChat::printMessage(QString message)
{
	new QListWidgetItem(message, ui.messageList);
	ui.messageList->scrollToBottom();
}

void lukiChat::printOwnedMessage(QString message)
{
	QListWidgetItem* item = new QListWidgetItem(message);

	item->setForeground(Qt::darkGray);
	ui.messageList->addItem(item);
	ui.messageList->scrollToBottom();
}
