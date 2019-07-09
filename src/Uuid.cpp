#include "Uuid.h"


Uuid::Uuid() :
mEntity() {

}

Uuid::Uuid(const QString &text) :
mEntity(QString("{%1}").arg(text)) {

}

Uuid::Uuid(const QUuid &rOther) :
mEntity(rOther) {

}

bool Uuid::isNull() const {

	return mEntity.isNull();
}

QString Uuid::toString() const {

	if(!isNull()) {
		return mEntity.toString().mid(1, 36);
	}
	return QString();
}

QString Uuid::toShortString() const {

	return toString().mid(0, 8);
}

QUuid Uuid::toQuuid() const {

	return mEntity;
}

bool Uuid::operator==(const QUuid &other) const {

	return mEntity == other;
}

bool Uuid::operator<(const Uuid &other) const {

	return mEntity < other.toQuuid();
}

bool Uuid::operator>(const Uuid &other) const {

	return mEntity > other.toQuuid();
}

bool Uuid::operator!=(const Uuid &other) const {

	return mEntity != other.toQuuid();
}

Uuid Uuid::createUuid() {

	Uuid ret;
	ret.mEntity = QUuid::createUuid();
	return ret;
}
