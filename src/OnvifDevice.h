#pragma once
#include "AbstractDevice.h"


class OnvifDeviceClient;
class OnvifEventClient;
class OnvifMediaClient;

class OnvifDevice : public AbstractDevice {

 public:
	OnvifDevice();
	~OnvifDevice();
	Result initDevice(const QUrl &rEndpoint, const QString &rUser, const QString &rPassword) override;
	QUuid getDeviceId() const override;
	QString getModel() const override;
	QString getManufacturer() const override;
	QString getSerialNumber() const override;
	QUrl getEndpoint() const override;
	// The date and time in local time (of this client)
	QDateTime getDeviceTime() const override;
	DetailedResult<DeviceInfo> getDeviceInfo() override;
	DetailedResult<QList<MediaProfile>> getyMediaProfiles() override;
	DetailedResult<QUrl> getStreamUrl(const QString &rMediaProfile) override;

 private:
	QUrl mDeviceEndpoint;
	QUuid mDeviceId;
	QString mModel;
	QString mManufacturer;
	QString mSerialNumber;
	qint64 mTimeDifference;
	OnvifDeviceClient *mpDeviceClient;
	OnvifEventClient *mpEventClient;
	OnvifMediaClient *mpMediaClient;
};
