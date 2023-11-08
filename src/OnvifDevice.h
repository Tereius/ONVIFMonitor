#pragma once
#include "AbstractDevice.h"
#include "DeviceInfo.h"
#include <QAtomicInt>

class QTimer;
class OnvifDeviceClient;
class OnvifEventClient;
class OnvifMediaClient;

class OnvifDevice : public AbstractDevice, public QObject {

 public:
	OnvifDevice();
	~OnvifDevice();
	Result initDevice(const QUrl &rEndpoint, const QString &rUser, const QString &rPassword) override;
	QUuid getDeviceId() const override;
	Result pingDevice() const override;
	DeviceInfo getDeviceInfo() const override;
	DetailedResult<QList<MediaProfile>> getMediaProfiles() override;
	QUrl getStreamUrl(const QUuid &rDeviceId, const QString &rMediaProfileToken);
	QFuture<DetailedResult<QImage>> getSnapshot(const MediaProfile &rMediaProfile, const QSize &rSize = QSize()) override;

 private slots:
	void checkAvailable();

 private:
	QUuid mDeviceId;
	DeviceInfo mDeviceInfo;
	OnvifDeviceClient *mpDeviceClient;
	OnvifEventClient *mpEventClient;
	OnvifMediaClient *mpMediaClient;
	QTimer *mpTimer;
};
