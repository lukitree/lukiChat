#ifndef LUKICHAT_H
#define LUKICHAT_H

#include <QtWidgets/QMainWindow>
#include "ui_lukichat.h"

class lukiChat : public QMainWindow
{
	Q_OBJECT

public:
	lukiChat(QWidget *parent = 0);
	~lukiChat();

private:
	Ui::lukiChatClass ui;
};

#endif // LUKICHAT_H
