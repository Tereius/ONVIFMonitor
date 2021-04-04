#pragma once
#include "Uuid.h"
#include "Result.h"
#include <QFuture>
#include <QObject>
#include <QMap>
#include <QUrl>
#include <QMutex>
#include <QAtomicInteger>
#include <QPointer>
#include <QSharedPointer>


#define DeviceM DeviceManager::getGlobal()

class AbstractDevice;
class DeviceInfo;

class DeviceManager : public QObject {

	Q_OBJECT

 public:
	explicit DeviceManager(QObject *pParent = nullptr);
	~DeviceManager() override;
	Q_INVOKABLE void initialize();
	Q_INVOKABLE void removeDevice(const Uuid &rDeviceId);
	Q_INVOKABLE Uuid getDeviceByHost(const QString &rHost, int port = 8080);
	Q_INVOKABLE QList<Uuid> getDevicesByName(const QString &rName);
	Q_INVOKABLE Uuid addDevice(const QUrl &rEndpoint, const QString &rUsername, const QString &rPassword, bool save = false,
	                           const QString &rDeviceName = QString(), const QUuid &rDeviceId = QUuid::createUuid());
	Q_INVOKABLE Uuid addDevice(const QUrl &rEndpoint, const QString &rDeviceName = QString(), const QUuid &rDeviceId = QUuid::createUuid());
	Q_INVOKABLE void renameDevice(const Uuid &rDeviceId, const QString &rDeviceName);
	Q_INVOKABLE Result setDeviceHost(const Uuid &rDeviceId, const QUrl &rDeviceHost);
	Q_INVOKABLE void setDeviceCredentials(const Uuid &rDeviceId, const QString &rUsername, const QString &rPassword, bool save = false);
	Q_INVOKABLE void reinitializeDevice(const Uuid &rDeviceId);

	Q_INVOKABLE QFuture<DetailedResult<DeviceInfo>> getDeviceInfoF(const Uuid &rDeviceId);

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

	struct Device {
		QString mDeviceName;
		QSharedPointer<AbstractDevice> mDevice;
	};

	void initDevices();
	void setBusy(bool isBusy);
	QString getUniqueDeviceName(const QString &rProposedName);

	QMap<Uuid, Device> mDevices;
	QMutex mMutex;
};
