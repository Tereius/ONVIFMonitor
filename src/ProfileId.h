#pragma once
#include "Uuid.h"
#include <QMetaObject>


class ProfileId {

	Q_GADGET

public:

	ProfileId(const Uuid &rDeviceId = Uuid(), const QString &rProfileToken = QString());
	Q_INVOKABLE Uuid getDeviceId() const;
	void setDeviceId(const Uuid &rVal);
	Q_INVOKABLE QString getProfileToken() const;
	void setProfileToken(const QString &rVal);
	QString toString() const;
	bool isNull() const;

private:
	Uuid mDeviceId;
	QString mProfileToken;
};
