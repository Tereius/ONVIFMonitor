#pragma once
#include "AbstractDevice.h"
#include "DeviceInfo.h"
#include <QSize>
#include <QString>
#include <QUrl>

class GenericDevice : public AbstractDevice {

 public:
	GenericDevice();
	~GenericDevice();
	Result initDevice(const QUrl &rEndpoint, const QString &rUser, const QString &rPassword) override;
	QUuid getDeviceId() const override;
	DeviceInfo getDeviceInfo() const override;
	DetailedResult<QList<MediaProfile>> getMediaProfiles() override;
	QFuture<DetailedResult<QImage>> getSnapshot(const MediaProfile &rMediaProfile, const QSize &rSize = QSize()) override;

 private:
	DeviceInfo mDeviceInfo;
};
