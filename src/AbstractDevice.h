#pragma once
#include "Result.h"
#include "MediaProfile.h"
#include "DeviceInfo.h"
#include <QString>
#include <QUuid>
#include <QDateTime>
#include <QUrl>
#include <QList>


class AbstractDevice {

 public:
	virtual Result initDevice(const QUrl &rEndpoint, const QString &rUser = {}, const QString &rPassword = {}) = 0;
	// The implementation should return a globally unique uuid that will never change.
	// The default implementation generates an id from AbstractDevice::getSerialNumber() and AbstractDevice::getManufacturer()
	virtual QUuid getDeviceId() const;
	virtual QString getModel() const = 0;
	virtual QString getManufacturer() const = 0;
	virtual QString getSerialNumber() const = 0;
	virtual QUrl getEndpoint() const = 0;
	virtual QDateTime getDeviceTime() const = 0;
	QString getHost() const;
	int getPort() const;
	virtual DetailedResult<DeviceInfo> getDeviceInfo() = 0;
	virtual DetailedResult<QList<MediaProfile>> getyMediaProfiles() = 0;
	virtual DetailedResult<QUrl> getStreamUrl(const QString &rMediaProfile) = 0;
};
