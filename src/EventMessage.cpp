#include "EventMessage.h"
#include <QDebug>


QDebug operator<<(QDebug debug, const EventMessage &message) {

	QDebugStateSaver const saver(debug);
	debug.nospace() << message.topic;
	debug.nospace() << message.source;
	debug.nospace() << message.key;
	debug.nospace() << message.data;
	return debug;
}

QDataStream &operator<<(QDataStream &out, const EventMessage &message) {

	out << message.topic;
	out << message.source;
	out << message.key;
	out << message.data;
	return out;
}
QDataStream &operator>>(QDataStream &in, EventMessage &record) {

	in >> record.topic;
	in >> record.source;
	in >> record.key;
	in >> record.data;
	return in;
}