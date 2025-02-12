#pragma once
#include <QHash>
#include <QObject>

class EventMessage {

	Q_GADGET
	Q_PROPERTY(QString topic MEMBER topic)

 public:
	QString topic;
	QHash<QString, QString> source;
	QHash<QString, QString> key;
	QHash<QString, QString> data;
};

QDebug operator<<(QDebug debug, const EventMessage &message);
QDataStream &operator<<(QDataStream &out, const EventMessage &message);
QDataStream &operator>>(QDataStream &in, EventMessage &record);
