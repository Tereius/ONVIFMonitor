#pragma once
#include "DeviceInfo.h"
#include "MediaProfile.h"
#include "Result.h"
#include <QAtomicInteger>
#include <QFuture>
#include <QMap>
#include <QRecursiveMutex>
#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QSize>
#include <QUrl>
#include <QtQmlIntegration>
#include <QTimer>


class AbstractDevice;
class DeviceInfo;
class QQmlEngine;
class QJSEngine;

class DeviceManager : public QObject {

	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

 public:
  ~DeviceManager() override;
	static DeviceManager* getInstance();
	Q_INVOKABLE void initialize();
	Q_INVOKABLE bool containsDevice(const QUuid &rDeviceId);
	Q_INVOKABLE QUuid getDeviceByHost(const QString &rHost, int port = 8080);
	Q_INVOKABLE QUuid getDeviceByEndpoint(const QUrl &rEndpoint);
	Q_INVOKABLE QList<QUuid> getDevices();
	Q_INVOKABLE QList<QUuid> getDevicesByName(const QString &rName);
	Q_INVOKABLE void renameDevice(const QUuid &rDeviceId, const QString &rDeviceName);
	Q_INVOKABLE QFuture<DetailedResult<QUuid>> addDevice(const QUrl &rEndpoint, const QString &rUsername, const QString &rPassword,
	                                                     const QString &rDeviceName = QString(),
	                                                     const QUuid &rDeviceId = QUuid::createUuid());
	Q_INVOKABLE void removeDevice(const QUuid &rDeviceId);
	Q_INVOKABLE DeviceInfo getDeviceInfo(const QUuid &rDeviceId);
	Q_INVOKABLE QString getName(const QUuid &rDeviceId);
	Q_INVOKABLE QFuture<Result> setDeviceCredentials(const QUuid &rDeviceId, const QString &rUsername, const QString &rPassword,
	                                                 bool save = false);
	Q_INVOKABLE bool isDeviceInitialized(const QUuid &rDeviceId);
	Q_INVOKABLE QFuture<DetailedResult<QList<MediaProfile>>> getMediaProfiles(const QUuid &rDeviceId);
	Q_INVOKABLE QUrl getStreamUrl(const QUuid &rDeviceId, const QString &rMediaProfileToken);
	Q_INVOKABLE QFuture<DetailedResult<QImage>> getSnapshot(const QUuid &rDeviceId, const QString &rMediaProfileToken,
	                                                        const QSize &rSize = QSize());

	QSharedPointer<AbstractDevice> getDevice(const QUuid &rDeviceId);

	static DeviceManager *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

 signals:
	void unauthorized(const QUuid &rDeviceId);
	void deviceAdded(const QUuid &rAddedDeviceId);
	void deviceRemoved(const QUuid &rRemovedDeviceId);
	void deviceInitialized(const QUuid &rRemovedDeviceId);
	void deviceChanged(const QUuid &rRemovedDeviceId);

 protected:
	explicit DeviceManager(QObject *pParent = nullptr);

 private slots:
	void checkDevices();

 private:
	Q_DISABLE_COPY(DeviceManager);

	struct Device {
		QAtomicInt mInitialized = 0;
		QUrl mEndpoint;
		QString mDeviceName;
		QString mUsername;
		QString mPassword;
		QSharedPointer<AbstractDevice> mDevice;
	};

	void initDevices();
	QFuture<Result> initDevice(QSharedPointer<AbstractDevice> device, const QUrl &rEndpoint, const QString &rUsername,
	                           const QString &rPassword);
	void setBusy(bool isBusy);
	QString getUniqueDeviceName(const QString &rProposedName);
	QUuid resolveId(const QUuid &id);

	QMap<QUuid, Device> mDevices;
	QMap<QUuid, QUuid> mAliasIds;
	QRecursiveMutex mMutex;
	QTimer mTimer;
};
