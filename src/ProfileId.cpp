#include "ProfileId.h"


ProfileId::ProfileId(const Uuid &rDeviceId /*= Uuid()*/, const QString &rProfileToken /*= QString()*/) :
	mDeviceId(rDeviceId),
	mProfileToken(rProfileToken) {

}

Uuid ProfileId::getDeviceId() const {
	return mDeviceId;
}

void ProfileId::setDeviceId(const Uuid &rVal) {
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
