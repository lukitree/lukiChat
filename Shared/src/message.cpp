#include "message.h"

Message::Message()
{
	type = MessageType::MSG;
	data.clear();
}

Message::~Message()
{
}
