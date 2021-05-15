#include "MediaProfile.h"


MediaProfile::MediaProfile(const QUuid &rOwnerDeviceId /*= Uuid()*/) : mDeviceId(rOwnerDeviceId), mName(), mToken(), mFixed() {}

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

QUuid MediaProfile::getDeviceId() const {

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

bool MediaProfile::isValid() const {

	return !mDeviceId.isNull() && !mToken.isEmpty();
}

bool MediaProfile::operator==(const MediaProfile &rOther) const {

	return mDeviceId == rOther.mDeviceId && mToken == rOther.mToken;
}

bool MediaProfile::operator!=(const MediaProfile &rOther) const {

	return !operator==(rOther);
}
