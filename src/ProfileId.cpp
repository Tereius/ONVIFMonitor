#include "ProfileId.h"


ProfileId::ProfileId(const QUuid &rDeviceId /*= Uuid()*/, const QString &rProfileToken /*= QString()*/) :
 mDeviceId(rDeviceId), mProfileToken(rProfileToken) {}

QUuid ProfileId::getDeviceId() const {
	return mDeviceId;
}

void ProfileId::setDeviceId(const QUuid &rVal) {
	mDeviceId = rVal;
}

QString ProfileId::getProfileToken() const {
	return mProfileToken;
}

void ProfileId::setProfileToken(const QString &rVal) {
	mProfileToken = rVal;
}

QString ProfileId::toString() const {

	return QString("DeviceId: %1, ProfileToken: %2").arg(mDeviceId.toString()).arg(mProfileToken);
}

bool ProfileId::isNull() const {

	return mDeviceId.isNull();
}
