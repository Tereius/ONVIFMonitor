#pragma once
#include "Result.h"
#include "Uuid.h"
#include "MediaProfile.h"
#include "DeviceInfo.h"
#include "Topics.h"
#include <QObject>
#include <QUrl>
#include <QMutex>
#include <QList>

class OnvifDeviceClient;
class OnvifEventClient;
class OnvifMediaClient;

class Device : public QObject {

	Q_OBJECT

public:
	Device(const QUrl &rDeviceEndpoint, const Uuid &rDeviceId, const QString &rDeviceName = QString(), QObject *pParent = nullptr);
	virtual ~Device();
	Result initialize();
	DeviceInfo getDeviceInfo();
	QString getDeviceName();
	Uuid getDeviceId();
	void setDeviceName(const QString &rDeviceName);
	void setCredentials(const QString &rUser, const QString &rPassword);
	//! You are responsible to call initialize afterwards!
	Result setHost(const QUrl &rHost);
	DetailedResult<QList<MediaProfile>> getMediaProfiles();
	DetailedResult<MediaProfile> getMediaProfile(const QString &rProfileToken) const;
	DetailedResult<QList<Topic>> getTopics() const;
	QUrl getStreamUrl(const QString &rProfileToken) const;
	//! Register an event
	void watchEvent();

private:
	Result initDeviceDateTime();
	Result initServices();
	Result initDeviceCapab();
	Result initEventCapab();
	Result initMediaCapab();
	Result initDeviceInfo();

	QUrl mDeviceEndpoint;
	OnvifDeviceClient *mpDeviceClient;
	OnvifEventClient *mpEventClient;
	OnvifMediaClient *mpMediaClient;
	DeviceInfo mDeviceInfo;
	QMutex mMutex;
};