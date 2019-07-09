#include "MediaProfile.h"


MediaProfile::MediaProfile(const Uuid &rOwnerDeviceId /*= Uuid()*/) :
mDeviceId(rOwnerDeviceId),
mName(),
mToken(),
mFixed() {

}

QString MediaProfile::getName() const {
	return mName;
}

void MediaProfile::setName(const QString &rName) {
	mName = rName;
}

QString MediaProfile::getToken() const {

	return mToken;
}

void MediaProfile::setToken(const QString &rToken) {

	mToken = rToken;
}

Uuid MediaProfile::getDeviceId() const {

	return mDeviceId;
}

ProfileId MediaProfile::getProfileId() const {

	return ProfileId(getDeviceId(), getToken());
}

bool MediaProfile::getFixed() const {

	return mFixed;
}

void MediaProfile::setFixed(bool fixed) {

	mFixed = fixed;
}
