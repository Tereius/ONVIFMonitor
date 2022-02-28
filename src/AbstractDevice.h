#pragma once
#include "DeviceInfo.h"
#include "MediaProfile.h"
#include "Result.h"
#include <QDateTime>
#include <QImage>
#include <QList>
#include <QString>
#include <QUrl>
#include <QUuid>


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
	virtual DetailedResult<QList<MediaProfile>> getMediaProfiles() = 0;
	virtual QFuture<DetailedResult<QImage>> getSnapshot(const MediaProfile &rMediaProfile, const QSize &rSize) = 0;
};
