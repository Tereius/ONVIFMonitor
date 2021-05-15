#pragma once
#include "AbstractDevice.h"
#include "DeviceInfo.h"


class OnvifDeviceClient;
class OnvifEventClient;
class OnvifMediaClient;

class OnvifDevice : public AbstractDevice {

 public:
	OnvifDevice();
	~OnvifDevice();
	Result initDevice(const QUrl &rEndpoint, const QString &rUser, const QString &rPassword) override;
	QUuid getDeviceId() const override;
	DeviceInfo getDeviceInfo() const override;
	DetailedResult<QList<MediaProfile>> getyMediaProfiles() override;
	DetailedResult<QImage> getSnapshot(const QString &rMediaProfile) override;

 private:
	QUuid mDeviceId;
	DeviceInfo mDeviceInfo;
	OnvifDeviceClient *mpDeviceClient;
	OnvifEventClient *mpEventClient;
	OnvifMediaClient *mpMediaClient;
};
