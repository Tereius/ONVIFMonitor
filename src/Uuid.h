#pragma once
#include <QString>
#include <QMetaType>
#include <QUuid>


class Uuid {

public:
	Uuid();
	Uuid(const QString &text);
	Uuid(const QUuid &rOther);
	bool isNull() const;
	QString toString() const;
	QString toShortString() const;
	QUuid toQuuid() const;
	bool operator==(const QUuid &other) const;
	bool operator!=(const Uuid &other) const;
	bool operator<(const Uuid &other) const;
	bool operator>(const Uuid &other) const;

	static Uuid createUuid();

private:

	QUuid mEntity;
};

inline bool operator==(const Uuid &e1, const Uuid &e2) {

	return e1.toQuuid() == e2.toQuuid();
}

inline uint qHash(const Uuid &key, uint seed) {

	return qHash(key.toQuuid(), seed);
}

Q_DECLARE_METATYPE(Uuid)
