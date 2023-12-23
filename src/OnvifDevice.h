#pragma once
#include "AbstractDevice.h"
#include "DeviceInfo.h"
#include <QAtomicInt>

class OnvifDeviceClient;
class OnvifEventClient;
class OnvifMediaClient;
class OnvifMedia2Client;

class OnvifDevice : public AbstractDevice {

 public:
	OnvifDevice();
	~OnvifDevice();
	Result initDevice(const QUrl &rEndpoint, const QString &rUser, const QString &rPassword) override;
	QUuid getDeviceId() const override;
	Result pingDevice() const override;
	DeviceInfo getDeviceInfo() const override;
	DetailedResult<QList<MediaProfile>> getMediaProfiles() override;
	DetailedResult<QList<MediaProfile>> getMedia2Profiles();
	QUrl getStreamUrl(const QUuid &rDeviceId, const QString &rMediaProfileToken);
	QFuture<DetailedResult<QImage>> getSnapshot(const MediaProfile &rMediaProfile, const QSize &rSize = QSize()) override;

 private:
	QUuid mDeviceId;
	DeviceInfo mDeviceInfo;
	OnvifDeviceClient *mpDeviceClient;
	OnvifEventClient *mpEventClient;
	OnvifMediaClient *mpMediaClient;
	OnvifMedia2Client *mpMedia2Client;
};
