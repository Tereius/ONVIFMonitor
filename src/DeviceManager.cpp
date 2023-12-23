#include "DeviceManager.h"
#include "DeviceInfo.h"
#include "GenericDevice.h"
#include "MicrophoneRtpSource.h"
#include "OnvifDevice.h"
#include "OnvifRtspClient.h"
#include "SecretsManager.h"
#include "Window.h"
#include "asyncfuture.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFuture>
#include <QGlobalStatic>
#include <QJSEngine>
#include <QMutexLocker>
#include <QSettings>
#include <QtConcurrent>

#define SECRET "yoY,V<EN.!0KPMpp" // Don't change. Otherwise saved passwords will be lost
#define GENERIC_DEVICE_NAME "New Device"

class DeviceManagerSingleton : public DeviceManager {};

Q_GLOBAL_STATIC(DeviceManagerSingleton, instance)

DeviceManager *DeviceManager::getInstance() {

	return instance;
}

DeviceManager::DeviceManager(QObject *pParent /*= nullptr*/) : QObject(pParent), mDevices(), mMutex(), mTimer() {

	mTimer.setTimerType(Qt::VeryCoarseTimer);
	mTimer.setInterval(6 * 1000);
	mTimer.setSingleShot(true);
	connect(&mTimer, &QTimer::timeout, this, &DeviceManager::checkDevices);
}

DeviceManager::~DeviceManager() = default;

void DeviceManager::initialize() {

	static bool initialized = false;
	if(!initialized) {
		initDevices();
		initialized = true;
	}
}

void DeviceManager::removeDevice(const QUuid &rDeviceId) {

	auto id = resolveId(rDeviceId);
	setBusy(true);
	mMutex.lock();
	mDevices.remove(id);
	mMutex.unlock();
	QSettings settings;
	settings.beginGroup("devices");
	settings.beginGroup(id.toString());
	settings.remove("");
	setBusy(false);
	emit deviceRemoved(id);
}

DeviceInfo DeviceManager::getDeviceInfo(const QUuid &rDeviceId) {

	QMutexLocker lock(&mMutex);
	if(mDevices.contains(resolveId(rDeviceId))) {
		return mDevices[resolveId(rDeviceId)].mDevice->getDeviceInfo();
	}
	return {};
}

bool DeviceManager::containsDevice(const QUuid &rDeviceId) {

	QMutexLocker lock(&mMutex);
	return mDevices.contains(resolveId(rDeviceId));
}

QUuid DeviceManager::getDeviceByHost(const QString &rHost, int port /*= 8080*/) {

	QUuid deviceId;
	setBusy(true);
	QMutexLocker lock(&mMutex);
	for(const auto &device : mDevices) {
		if(device.mDevice && device.mDevice->getHost() == rHost && device.mDevice->getPort() == port) {
			deviceId = resolveId(device.mDevice->getDeviceId());
			break;
		}
	}
	setBusy(false);
	return deviceId;
}

QUuid DeviceManager::getDeviceByEndpoint(const QUrl &rEndpoint) {

	QUuid deviceId;
	setBusy(true);
	QMutexLocker lock(&mMutex);
	for(const auto &device : mDevices) {
		if(device.mDevice && device.mDevice->getDeviceInfo().mEndpoint == rEndpoint) {
			deviceId = resolveId(device.mDevice->getDeviceId());
			break;
		}
	}
	setBusy(false);
	return deviceId;
}

QList<QUuid> DeviceManager::getDevices() {

	QMutexLocker lock(&mMutex);
	return mDevices.keys();
}

QList<QUuid> DeviceManager::getDevicesByName(const QString &rName) {

	QList<QUuid> deviceIds;
	setBusy(true);
	QMutexLocker lock(&mMutex);
	for(const auto &device : mDevices) {
		if(device.mDevice && device.mDeviceName.compare(rName) == 0) {
			deviceIds.push_back(resolveId(device.mDevice->getDeviceId()));
		}
	}
	setBusy(false);
	return deviceIds;
}

QFuture<DetailedResult<QUuid>> DeviceManager::addDevice(const QUrl &rEndpoint, const QString &rUsername, const QString &rPassword,
                                                        const QString &rDeviceName /*= QString()*/,
                                                        const QUuid &rDeviceId /*= QUuid::createUuid()*/) {

	if(!rDeviceId.isNull()) {
		mMutex.lock();
		auto duplicateDeviceId = getDeviceByEndpoint(rEndpoint);
		auto duplicateDeviceByEndpoint = !duplicateDeviceId.isNull();
		auto duplicateDeviceById = mDevices.contains(rDeviceId);
		if(!duplicateDeviceByEndpoint && duplicateDeviceById) duplicateDeviceId = rDeviceId;
		mMutex.unlock();
		if(!duplicateDeviceByEndpoint && !duplicateDeviceById) {
			QSharedPointer<AbstractDevice> device;
			// if(rEndpoint.url().endsWith("onvif/device_service")) {
			//	device = QSharedPointer<OnvifDevice>::create();
			// } else {
			device = QSharedPointer<OnvifDevice>::create();
			//}
			auto initFuture = initDevice(device, rEndpoint, rUsername, rPassword);
			return AsyncFuture::observe(initFuture)
			 .subscribe([this, initFuture, device, rEndpoint, rUsername, rPassword, rDeviceName, rDeviceId]() {
				 auto initResult = initFuture.result();
				 if(initResult) {
					 auto deviceEntry = Device();
					 deviceEntry.mEndpoint = rEndpoint;
					 deviceEntry.mDeviceName = rDeviceName;
					 deviceEntry.mUsername = rUsername;
					 deviceEntry.mPassword = rPassword;
					 deviceEntry.mDevice = device;
					 deviceEntry.mInitialized = true;
					 QSettings settings;
					 SecretsManager sec;
					 sec.writeSecretSync(rDeviceId.toString(), rPassword);
					 settings.beginGroup("devices");
					 settings.beginGroup(rDeviceId.toString());
					 settings.setValue("id", rDeviceId);
					 settings.setValue("endpoint", rEndpoint);
					 settings.setValue("name", rDeviceName);
					 settings.setValue("user", rUsername);
					 settings.setValue("password", rPassword);
					 settings.endGroup();
					 mMutex.lock();
					 mDevices.insert(rDeviceId, deviceEntry);
					 mAliasIds.insert(device->getDeviceId(), rDeviceId);
					 mMutex.unlock();
					 emit deviceAdded(rDeviceId);
					 return DetailedResult<QUuid>(rDeviceId);
				 } else {
					 auto result = DetailedResult<QUuid>(initResult, initResult.getDetails());
					 return result;
				 }
			 })
			 .future();
		}
		auto deferred = AsyncFuture::deferred<DetailedResult<QUuid>>();
		deferred.complete(DetailedResult<QUuid>(
		 Result::FAULT, tr("Device can't be added because it already exists under the name: %1").arg(getName(duplicateDeviceId))));
		return deferred.future();
	} else {
		auto deferred = AsyncFuture::deferred<DetailedResult<QUuid>>();
		deferred.complete(DetailedResult<QUuid>(Result::FAULT, tr("Device can't be added because the given uuid is invalid")));
		return deferred.future();
	}
}

QFuture<Result> DeviceManager::initDevice(QSharedPointer<AbstractDevice> device, const QUrl &rEndpoint, const QString &rUsername,
                                          const QString &rPassword) {

	if(device) {
		auto initFuture =
		 QtConcurrent::run([device, rEndpoint, rUsername, rPassword]() { return device->initDevice(rEndpoint, rUsername, rPassword); });
		return initFuture;
	}
	auto d = AsyncFuture::deferred<Result>();
	d.complete(Result(Result::FAULT, tr("Can't initialize device. The device doesn't exist.")));
	return d.future();
}

void DeviceManager::renameDevice(const QUuid &rDeviceId, const QString &rDeviceName) {

	mMutex.lock();
	auto id = resolveId(rDeviceId);
	if(mDevices.contains(id)) {
		auto &device = mDevices[id];
		device.mDeviceName = rDeviceName;
		QSettings settings;
		settings.beginGroup("devices");
		settings.beginGroup(id.toString());
		settings.setValue("name", rDeviceName);
		mMutex.unlock();
		emit deviceChanged(id);
	} else {
		mMutex.unlock();
	}
}

QString DeviceManager::getName(const QUuid &rDeviceId) {

	QMutexLocker lock(&mMutex);
	auto id = resolveId(rDeviceId);
	return mDevices.value(id).mDeviceName;
}

QFuture<Result> DeviceManager::setDeviceCredentials(const QUuid &rDeviceId, const QString &rUsername, const QString &rPassword,
                                                    bool save /*= false*/) {

	mMutex.lock();
	auto id = resolveId(rDeviceId);
	if(mDevices.contains(id)) {
		auto &device = mDevices[id];
		device.mUsername = rUsername;
		device.mPassword = rPassword;
		device.mInitialized = false;
		mMutex.unlock();
		emit deviceChanged(id);
		if(save) {
			QSettings settings;
			settings.beginGroup("devices");
			settings.beginGroup(id.toString());
			settings.setValue("user", rUsername);
			settings.setValue("password", rPassword);
		}
		auto initFuture = initDevice(device.mDevice, device.mEndpoint, device.mUsername, device.mPassword);
		AsyncFuture::observe(initFuture).subscribe([this, initFuture, id]() {
			mMutex.lock();
			if(initFuture.result() && mDevices.contains(id)) {
				auto &device = mDevices[id];
				device.mInitialized = true;
			}
			mMutex.unlock();
			emit deviceChanged(id);
		});
		return initFuture;
	} else {
		mMutex.unlock();
	}
	auto d = AsyncFuture::deferred<Result>();
	d.complete(Result(Result::FAULT, tr("Can't change credentials. The device doesn't exist.")));
	return d.future();
}

bool DeviceManager::isDeviceInitialized(const QUuid &rDeviceId) {

	mMutex.lock();
	auto deviceEntry = mDevices.value(resolveId(rDeviceId));
	mMutex.unlock();
	return deviceEntry.mInitialized;
}

QFuture<DetailedResult<QList<MediaProfile>>> DeviceManager::getMediaProfiles(const QUuid &rDeviceId) {

	mMutex.lock();
	auto deviceEntry = mDevices.value(resolveId(rDeviceId));
	mMutex.unlock();
	if(deviceEntry.mDevice) {
		return QtConcurrent::run([deviceEntry]() { return deviceEntry.mDevice->getMediaProfiles(); });
	}
	auto d = AsyncFuture::deferred<DetailedResult<QList<MediaProfile>>>();
	d.complete(DetailedResult<QList<MediaProfile>>(Result::FAULT, tr("Can't get media profiles. The device doesn't exist.")));
	return d.future();
}

QUrl DeviceManager::getStreamUrl(const QUuid &rDeviceId, const QString &rMediaProfileToken) {

	mMutex.lock();
	auto deviceEntry = mDevices.value(resolveId(rDeviceId));
	mMutex.unlock();
	if(deviceEntry.mDevice) {
		auto deviceInfo = deviceEntry.mDevice->getDeviceInfo();
		MediaProfile profile;
		for(const auto &rProfile : deviceInfo.mMediaProfiles) {
			if(rProfile.getToken().compare(rMediaProfileToken) == 0) {
				if(!rProfile.mStreamUrls.isEmpty()) {
					return rProfile.mStreamUrls.first().mUrlWithCredentials;
				}
			}
		}
	}
	return {};
}

QFuture<DetailedResult<QImage>> DeviceManager::getSnapshot(const QUuid &rDeviceId, const QString &rMediaProfileToken,
                                                           const QSize &rSize /*= QSize()*/) {

	mMutex.lock();
	auto deviceEntry = mDevices.value(resolveId(rDeviceId));
	mMutex.unlock();
	if(deviceEntry.mDevice) {
		auto deviceInfo = deviceEntry.mDevice->getDeviceInfo();
		MediaProfile profile;
		for(const auto &rProfile : deviceInfo.mMediaProfiles) {
			if(rProfile.getToken().compare(rMediaProfileToken) == 0) {
				profile = rProfile;
				break;
			}
		}
		return deviceEntry.mDevice->getSnapshot(profile, rSize);
	}
	auto d = AsyncFuture::deferred<DetailedResult<QImage>>();
	d.complete(DetailedResult<QImage>(Result::FAULT, tr("Can't get snapshot. The device doesn't exist.")));
	return d.future();
}

QSharedPointer<AbstractDevice> DeviceManager::getDevice(const QUuid &rDeviceId) {

	mMutex.lock();
	auto deviceEntry = mDevices.value(resolveId(rDeviceId));
	mMutex.unlock();
	return deviceEntry.mDevice;
}

DeviceManager *DeviceManager::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {

	Q_UNUSED(qmlEngine)
	Q_UNUSED(jsEngine)
	auto instance = getInstance();
	QJSEngine::setObjectOwnership(instance, QJSEngine::CppOwnership);
	return instance;
}

#include <QMediaDevices>

void DeviceManager::checkDevices() {
	
	for(const auto &deviceId : getDevices()) {
		mMutex.lock();
		auto deviceEntry = mDevices.value(resolveId(deviceId));
		mMutex.unlock();
		if(deviceEntry.mDevice) {
			if(!deviceEntry.mDevice->getDeviceInfo().mInitialized) {
				// If device is not initialized try initialize it again
				auto initFuture = initDevice(deviceEntry.mDevice, deviceEntry.mEndpoint, deviceEntry.mUsername, deviceEntry.mPassword);
				AsyncFuture::observe(initFuture).subscribe([this, initFuture, deviceId]() {
					auto changed = false;
					mMutex.lock();
					if(mDevices.contains(deviceId)) {
						auto &device = mDevices[deviceId];
						if(device.mInitialized != initFuture.result().isSuccess()) {
							device.mInitialized = initFuture.result().isSuccess();
							changed = true;
						}
					}
					mMutex.unlock();
					if(changed) {
						emit deviceChanged(deviceId);
					}
				});
			} else {
				// Check if the device is still available
				auto pingFurure = QtConcurrent::run([deviceEntry]() { return deviceEntry.mDevice->pingDevice(); });
				AsyncFuture::observe(pingFurure).subscribe([this, pingFurure, deviceId]() {
					auto changed = false;
					mMutex.lock();
					if(mDevices.contains(deviceId)) {
						auto &device = mDevices[deviceId];
						if(device.mInitialized != pingFurure.result().isSuccess()) {
							device.mInitialized = pingFurure.result().isSuccess();
							changed = true;
							if(device.mInitialized)
								qInfo() << "Device" << device.mDeviceName << "went online";
							else
								qInfo() << "Device" << device.mDeviceName << "went offline";
						}
					}
					mMutex.unlock();
					if(changed) {
						emit deviceChanged(deviceId);
					}
				});
			}
		}
	}
}

void DeviceManager::initDevices() {

	setBusy(true);
	QSettings settings;
	settings.beginGroup("devices");
	auto deviceGroup = settings.childGroups();
	for(int i = 0; i < deviceGroup.size(); ++i) {
		settings.beginGroup(deviceGroup.at(i));
		QUuid deviceId = QUuid(settings.value("id").toUuid());
		QUrl deviceEndpoint = settings.value("endpoint").toUrl();
		QString deviceName = settings.value("name").toString();
		QString username = settings.value("user").toString();
		QString password = settings.value("password").toString();
		settings.endGroup();
		auto deviceEntry = Device();
		deviceEntry.mEndpoint = deviceEndpoint;
		deviceEntry.mDeviceName = deviceName;
		deviceEntry.mUsername = username;
		deviceEntry.mPassword = password;
		deviceEntry.mDevice = QSharedPointer<OnvifDevice>::create();
		deviceEntry.mInitialized = false;
		if(!deviceId.isNull() && deviceEndpoint.isValid()) {
			mMutex.lock();
			mDevices.insert(deviceId, deviceEntry);
			mMutex.unlock();
			emit deviceAdded(deviceId);
			auto initFuture = initDevice(deviceEntry.mDevice, deviceEntry.mEndpoint, deviceEntry.mUsername, deviceEntry.mPassword);
			AsyncFuture::observe(initFuture).subscribe([this, initFuture, deviceId]() {
				mMutex.lock();
				if(initFuture.result() && mDevices.contains(deviceId)) {
					auto &device = mDevices[deviceId];
					device.mInitialized = true;
					mAliasIds.insert(device.mDevice->getDeviceId(), deviceId);
				}
				mMutex.unlock();
				emit deviceChanged(deviceId);
			});
		} else {
			qWarning() << "Found invalid device from settings";
		}
	}
	mTimer.start();
}

void DeviceManager::setBusy(bool isBusy) {

	Window::getGlobal()->setModalBusy(isBusy);
}

QString DeviceManager::getUniqueDeviceName(const QString &rProposedName) {

	auto suffix = 1;
	bool isUnique = false;
	auto probeName = rProposedName;
	while(!isUnique) {
		isUnique = true;
		mMutex.lock();
		for(const auto &device : mDevices) {
			if(probeName == device.mDeviceName) {
				isUnique = false;
				probeName = QString("%1 %2").arg(rProposedName).arg(suffix++);
				break;
			}
		}
		mMutex.unlock();
	}
	return probeName;
}

QUuid DeviceManager::resolveId(const QUuid &id) {

	QMutexLocker lock(&mMutex);
	if(mDevices.contains(id)) return id;
	if(mAliasIds.contains(id)) return mAliasIds.value(id);
	return {};
}
