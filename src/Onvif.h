#pragma once
#include "MonitorGridModel.h"
#include "ProfileId.h"
#include <QObject>
#include <QtQmlIntegration>

class Onvif : public QObject {

	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

 public:
	Q_INVOKABLE QUuid createUuid() const;
	Q_INVOKABLE QString createUuidString() const;
	Q_INVOKABLE ProfileId createProfileId(const QUuid &rDeviceId, const QString &rProfileToken) const;
};
