#include "lukichat.h"
#include "message.h"

lukiChat::lukiChat(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	qRegisterMetaTypeStreamOperators<Message>("Message");

	connectDialog = new ConnectDialog(this);

	serverSocket = new QTcpSocket(this);

	connect(serverSocket, SIGNAL(error()), this, SLOT(error()));
}

lukiChat::~lukiChat()
{

}

void lukiChat::onConnect()
{
}

void lukiChat::receive()
{
}

void lukiChat::sendUserName()
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);

	Message msg;
	msg.type = Message::USR;
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

void lukiChat::error()
{
	new QListWidgetItem("Failed to connect to host.", ui.messageList);
}
