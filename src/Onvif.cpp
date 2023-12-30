#include "Onvif.h"

QUuid Onvif::createUuid() const {

	return QUuid::createUuid();
}

QString Onvif::createUuidString() const {

	return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

ProfileId Onvif::createProfileId(const QUuid &rDeviceId, const QString &rProfileToken) const {

	return ProfileId(rDeviceId, rProfileToken);
}
