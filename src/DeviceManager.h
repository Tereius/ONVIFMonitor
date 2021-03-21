#pragma once
#include "Uuid.h"
#include "Result.h"
#include <QObject>
#include <QMap>
#include <QUrl>
#include <QMutex>
#include <QAtomicInteger>
#include <QPointer>
#include <QSharedPointer>


#define DeviceM DeviceManager::getGlobal()

class Device;
class DeviceInfo;

class DeviceManager : public QObject {

	Q_OBJECT

 public:
	DeviceManager(QObject *pParent = nullptr);
	virtual ~DeviceManager();
	Q_INVOKABLE void initialize();
	Q_INVOKABLE void removeDevice(const Uuid &rDeviceId);
	Q_INVOKABLE DeviceInfo getDeviceInfo(const Uuid &rDeviceId);
	Q_INVOKABLE DeviceInfo getDeviceInfoByEndpointRef(const Uuid &rEndpointRef);
	Q_INVOKABLE DeviceInfo getDeviceInfoByHost(const QString &rHost, int port = 8080);
	Q_INVOKABLE Uuid addDevice(const QUrl &rEndpoint, const QString &rUsername, const QString &rPassword, bool save = false,
	                           const QString &rDeviceName = QString(), const QUuid &rDeviceId = QUuid::createUuid());
	Q_INVOKABLE Uuid addDevice(const QUrl &rEndpoint, const QString &rDeviceName = QString(), const QUuid &rDeviceId = QUuid::createUuid());
	Q_INVOKABLE void renameDevice(const Uuid &rDeviceId, const QString &rDeviceName);
	Q_INVOKABLE Result setDeviceHost(const Uuid &rDeviceId, const QUrl &rDeviceHost);
	Q_INVOKABLE void setDeviceCredentials(const Uuid &rDeviceId, const QString &rUsername, const QString &rPassword, bool save = false);
	Q_INVOKABLE void reinitializeDevice(const Uuid &rDeviceId);

	QSharedPointer<Device> getDevice(const Uuid &rDeviceId);

	QList<DeviceInfo> getDeviceInfos();

	static DeviceManager *getGlobal();

 signals:
	void unauthorized(const Uuid &rDeviceId);
	void deviceAdded(const Uuid &rAddedDeviceId);
	void deviceRemoved(const Uuid &rRemovedDeviceId);
	void deviceInitialized(const Uuid &rRemovedDeviceId);
	void deviceChanged(const Uuid &rRemovedDeviceId);

 private:
	Q_DISABLE_COPY(DeviceManager);

	void initDevices();
	void setBusy(bool isBusy);
	QString getUniqueDeviceName(const QString &rProposedName);

	QMap<Uuid, QSharedPointer<Device>> mDevices;
	QMutex mMutex;
};
