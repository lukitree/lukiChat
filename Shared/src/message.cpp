#include "message.h"

Message::Message()
{
	type = MessageType::MSG;
	data.clear();
	extra.clear();
}

Message::~Message()
{
}
