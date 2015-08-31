#include "lukichat.h"

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
	printServerMessage("-> You have been disconnected.", "Red");
	ui.userList->clear();
}

void lukiChat::receive()
{
	QDataStream in(serverSocket);
	in.setVersion(QDataStream::Qt_5_0);

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
		case Message::SVRMSG:
			printServerMessage(message.data, message.extra);
			break;
		case Message::USRLST:
			processUserList(message.data);
			break;
		case Message::USR:
			setupAssignedUsername(message.data);
			break;
		case Message::PM:
			printPM(message);
			break;
	}
}

void lukiChat::sendUserName()
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_0);

	QString username = connectDialog->usernameEdit->text();

	username.replace(" ", "_");

	Message message;
	message.type = Message::USR;
	message.data = username;

	out << message;
	serverSocket->write(block);
}

void lukiChat::on_actionConnect_triggered()
{

	if (connectDialog->exec())
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

	QStringList textTokens = text.split(" ", QString::KeepEmptyParts);

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_0);

	Message message;
	message.type = Message::MSG;
	message.data = text;

	if (textTokens.front() == "/pm")
	{
		textTokens.pop_front();
		message.extra = textTokens.front();
		textTokens.pop_front();
		message.type = Message::PM;
		message.data.clear();

		for (auto i : textTokens)
			message.data += i + " ";
	}

	// Check if user is PM themselves
	if (message.data == "")
		printServerMessage("[!] Error: cannot send empty PM.", "Red");
	else if (message.extra == assignedUsername)
		printServerMessage("[!] Error: cannot PM self.", "Red");
	else
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
		QListWidgetItem* item = new QListWidgetItem(i, ui.userList);
		if (i == assignedUsername)
			item->setBackground(Qt::lightGray);
		ui.userList->addItem(item);
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

void lukiChat::printServerMessage(QString message, QString color)
{
	Qt::GlobalColor setColor = Qt::blue;
	if (color == "Green")
		setColor = Qt::green;
	else if (color == "Dark Green")
		setColor = Qt::darkGreen;
	else if (color == "Red")
		setColor = Qt::red;
	else if (color == "Dark Red")
		setColor = Qt::darkRed;
	else if (color == "Blue")
		setColor = Qt::blue;
	else if (color == "Dark Blue")
		setColor = Qt::darkBlue;

	QListWidgetItem* item = new QListWidgetItem(message);
	item->setForeground(setColor);

	ui.messageList->addItem(item);
	ui.messageList->scrollToBottom();
}

void lukiChat::printOwnedMessage(QString message)
{
	QListWidgetItem* item = new QListWidgetItem(message);

	item->setForeground(Qt::darkGray);
	ui.messageList->addItem(item);
	ui.messageList->scrollToBottom();
}

void lukiChat::setupAssignedUsername(QString username)
{
	assignedUsername = username;

	QString notification = "<> You have been assigned the username: \"" + assignedUsername + "\" by the server.";
	QListWidgetItem* item = new QListWidgetItem(notification);
	item->setForeground(Qt::blue);

	ui.messageList->addItem(item);
	ui.messageList->scrollToBottom();
}

void lukiChat::printPM(Message message)
{
	QStringList users = message.extra.split(" ", QString::SkipEmptyParts);

	QString toUser = users.front();
	QString fromUser = users.back();

	QListWidgetItem* item;

	if (fromUser == assignedUsername)
	{
		item = new QListWidgetItem("To " + toUser + ": " + message.data);
	}
	else
	{
		item = new QListWidgetItem("From " + fromUser + ": " + message.data);
		// Play sound when receiving a message
		QSound::play("resources/pm.wav");
	}

	item->setForeground(Qt::magenta);
	ui.messageList->addItem(item);
	ui.messageList->scrollToBottom();
}
