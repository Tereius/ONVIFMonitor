#pragma once
#include "Result.h"
#include "MediaProfile.h"
#include "DeviceInfo.h"
#include <QString>
#include <QUuid>
#include <QDateTime>
#include <QUrl>
#include <QList>
#include <QImage>


class AbstractDevice {

 public:
	// The implementation may perform custom initialization steps. This method may be called multiple times.
	virtual Result initDevice(const QUrl &rEndpoint, const QString &rUser, const QString &rPassword) = 0;
	// The implementation should return a globally unique uuid that will never change.
	// The default implementation generates an id from its serial number and manufacturer
	virtual QUuid getDeviceId() const;
	QDateTime getDeviceTime() const;
	QString getHost() const;
	int getPort() const;
	virtual DeviceInfo getDeviceInfo() const = 0;
	virtual DetailedResult<QList<MediaProfile>> getyMediaProfiles() = 0;
	virtual DetailedResult<QImage> getSnapshot(const QString &rMediaProfile) = 0;
};
