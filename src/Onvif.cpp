#include "Onvif.h"

QUuid Onvif::createUuid() const {

	return QUuid::createUuid();
}

ProfileId Onvif::createProfileId(const QUuid &rDeviceId, const QString &rProfileToken) const {

	return ProfileId(rDeviceId, rProfileToken);
}
