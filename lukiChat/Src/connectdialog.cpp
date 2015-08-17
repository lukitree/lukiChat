#include "connectdialog.h"

ConnectDialog::ConnectDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);

	//TESTING
	usernameEdit->setText("NoName");
	portEdit->setText("42069");	// default server port
	serverEdit->setText("127.0.0.1");
}

ConnectDialog::~ConnectDialog()
{

}
