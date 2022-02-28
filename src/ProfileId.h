#pragma once
#include "QUuid"
#include <QMetaObject>


class ProfileId {

	Q_GADGET

 public:
	explicit ProfileId(const QUuid &rDeviceId = QUuid(), const QString &rProfileToken = QString());
	Q_INVOKABLE QUuid getDeviceId() const;
	void setDeviceId(const QUuid &rVal);
	Q_INVOKABLE QString getProfileToken() const;
	void setProfileToken(const QString &rVal);
	QString toString() const;
	bool isNull() const;

 private:
	QUuid mDeviceId;
	QString mProfileToken;
};
