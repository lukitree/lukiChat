#include <QString>
#include <QDataStream>

class Message
{
public:
	Message();
	~Message();

	enum MessageType
	{
		MSG,
		MSGOWNR,
		USR,
		CMD,
		KICK,
		USRLST,
	};

	MessageType type;
	QString data;
};

inline QDataStream& operator<<(QDataStream &out, const Message& t)
{
	out << static_cast<quint16>(t.type);
	out << t.data;

	return out;
}

inline QDataStream& operator>>(QDataStream &in, Message& t)
{
	quint16 i;
	in >> i;
	t.type = static_cast<Message::MessageType>(i);
	in >> t.data;
	
	return in;
}