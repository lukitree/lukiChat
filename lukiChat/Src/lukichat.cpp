#include "lukichat.h"
#include "message.h"

lukiChat::lukiChat(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	qRegisterMetaTypeStreamOperators<Message>("Message");

	connectDialog = new ConnectDialog(this);

	serverSocket = new QTcpSocket(this);
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
	QDataStream in(serverSocket);
}

void lukiChat::on_actionConnect_triggered()
{
	QString port = connectDialog->portEdit->text();
	QString address = connectDialog->serverEdit->text();
	QString username = connectDialog->usernameEdit->text();

	if (connectDialog->exec() && !port.isEmpty() && !address.isEmpty() && !username.isEmpty())
	{
		serverSocket->connectToHost(address, port.toInt());
		// connect to server
	}
}
