#include "DeviceManager.h"
#include "DeviceInfo.h"
#include "Window.h"
#include "Device.h"
#include <QGlobalStatic>
#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent>
#include <QMutexLocker>


Q_GLOBAL_STATIC(DeviceManager, globalManager)
#define SECRET "yoY,V<EN.!0KPMpp" // Don't change. Otherwise saved passwords will be lost
#define GENERIC_DEVICE_NAME "New Device"

DeviceManager::DeviceManager(QObject *pParent /*= nullptr*/) : QObject(pParent), mDevices(), mMutex(QMutex::Recursive) {}

DeviceManager::~DeviceManager() {}

void DeviceManager::initialize() {

	static bool initialized = false;
	if(!initialized) {
		initDevices();
		initialized = true;
	}
}

DeviceManager *DeviceM {

	return globalManager;
}

void DeviceManager::removeDevice(const Uuid &rDeviceId) {

	setBusy(true);
	mMutex.lock();
	mDevices.remove(rDeviceId);
	mMutex.unlock();
	QSettings settings;
	settings.beginGroup("devices");
	settings.beginGroup(rDeviceId.toString());
	settings.remove("");
	setBusy(false);
	emit deviceRemoved(rDeviceId);
}

DeviceInfo DeviceManager::getDeviceInfo(const Uuid &rDeviceId) {

	DeviceInfo pDeviceInfo;
	setBusy(true);
	mMutex.lock();
	auto device = mDevices.value(rDeviceId);
	mMutex.unlock();
	if(device) {
		pDeviceInfo = device->getDeviceInfo();
	}
	setBusy(false);
	return pDeviceInfo;
}

DeviceInfo DeviceManager::getDeviceInfoByEndpointRef(const Uuid &rEndpointRef) {

	DeviceInfo pDeviceInfo;
	setBusy(true);
	QMutexLocker lock(&mMutex);
	for(const auto &device : mDevices) {
		if(!device->getDeviceInfo().getEndpointReference().isNull() && device->getDeviceInfo().getEndpointReference() == rEndpointRef) {
			pDeviceInfo = device->getDeviceInfo();
			break;
		}
	}
	setBusy(false);
	return pDeviceInfo;
}

DeviceInfo DeviceManager::getDeviceInfoByHost(const QString &rHost, int port /*= 8080*/) {

	DeviceInfo pDeviceInfo;
	setBusy(true);
	QMutexLocker lock(&mMutex);
	for(const auto &device : mDevices) {
		if(device->getDeviceInfo().getHost() == rHost && device->getDeviceInfo().getPort() == port) {
			pDeviceInfo = device->getDeviceInfo();
			break;
		}
	}
	setBusy(false);
	return pDeviceInfo;
}

Uuid DeviceManager::addDevice(const QUrl &rEndpoint, const QString &rDeviceName /*= QString()*/,
                              const QUuid &rDeviceId /*= QUuid::createUuid()*/) {

	return addDevice(rEndpoint, QString(), QString(), false, rDeviceName, rDeviceId);
}

Uuid DeviceManager::addDevice(const QUrl &rEndpoint, const QString &rUsername, const QString &rPassword, bool save /*= false*/,
                              const QString &rDeviceName /*= QString()*/, const QUuid &rDeviceId /*= QUuid::createUuid()*/) {

	auto deviceId = !rDeviceId.isNull() ? rDeviceId : QUuid::createUuid();
	if(rEndpoint.isValid()) {
		setBusy(true);
		auto device = QSharedPointer<Device>::create(rEndpoint, deviceId, rDeviceName, this);
		connect(
		 device.data(), &Device::unauthorized, this, [this, deviceId]() { emit unauthorized(deviceId); }, Qt::DirectConnection);
		if(!rUsername.isNull() && !rPassword.isNull()) {
			device->setCredentials(rUsername, rPassword);
		}
		QtConcurrent::run([=]() {
			auto result = device->initialize();
			if(device->getDeviceName().isNull()) {
				// We set a generic device name
				device->setDeviceName(getUniqueDeviceName(tr(GENERIC_DEVICE_NAME)));
			}
			mMutex.lock();
			mDevices.insert(deviceId, device);
			mMutex.unlock();
			QSettings settings;
			settings.beginGroup("devices");
			settings.beginGroup(deviceId.toString());
			settings.setValue("id", deviceId);
			settings.setValue("endpoint", rEndpoint);
			settings.setValue("endpointReference", device->getDeviceInfo().getEndpointReference().toQuuid());
			settings.setValue("name", device->getDeviceInfo().getDeviceName());
			if(!rUsername.isNull() && !rPassword.isNull() && save) {
				settings.setValue("user", rUsername);
				settings.setValue("password", rPassword);
			}
			setBusy(false);
			emit deviceAdded(deviceId);
			if(result) {
				emit deviceInitialized(deviceId);
			}
		});
	}
	return deviceId;
}

void DeviceManager::renameDevice(const Uuid &rDeviceId, const QString &rDeviceName) {

	mMutex.lock();
	auto device = mDevices.value(rDeviceId);
	mMutex.unlock();
	if(device) {
		device->setDeviceName(rDeviceName);
		QSettings settings;
		settings.beginGroup("devices");
		settings.beginGroup(rDeviceId.toString());
		settings.setValue("name", rDeviceName);
	}
	emit deviceChanged(rDeviceId);
}

Result DeviceManager::setDeviceHost(const Uuid &rDeviceId, const QUrl &rDeviceHost) {

	setBusy(true);
	Result result;
	mMutex.lock();
	auto device = mDevices.value(rDeviceId);
	mMutex.unlock();
	if(device) {
		result = device->setHost(rDeviceHost);
		if(result) {
			QSettings settings;
			settings.beginGroup("devices");
			settings.beginGroup(rDeviceId.toString());
			settings.setValue("endpoint", device->getDeviceInfo().getEndpoint());
			QtConcurrent::run([=]() {
				auto result = device->initialize();
				emit deviceChanged(rDeviceId);
				if(result) {
					emit deviceInitialized(rDeviceId);
				}
				setBusy(false);
			});
		} else {
			setBusy(false);
		}
	} else {
		setBusy(false);
	}
	return result;
}

void DeviceManager::setDeviceCredentials(const Uuid &rDeviceId, const QString &rUsername, const QString &rPassword, bool save /*= false*/) {

	setBusy(true);
	mMutex.lock();
	auto device = mDevices.value(rDeviceId);
	mMutex.unlock();
	if(device) {
		device->setCredentials(rUsername, rPassword);
		if(save) {
			QSettings settings;
			settings.beginGroup("devices");
			settings.beginGroup(rDeviceId.toString());
			settings.setValue("user", rUsername);
			settings.setValue("password", rPassword);
		}
	}
	setBusy(false);
	emit deviceChanged(rDeviceId);
}

void DeviceManager::reinitializeDevice(const Uuid &rDeviceId) {

	mMutex.lock();
	auto device = mDevices.value(rDeviceId);
	mMutex.unlock();
	if(device) {
		setBusy(true);
		QtConcurrent::run([=]() {
			auto result = device->initialize();
			emit deviceChanged(rDeviceId);
			if(result) {
				emit deviceInitialized(rDeviceId);
			}
			setBusy(false);
		});
	}
}

QSharedPointer<Device> DeviceManager::getDevice(const Uuid &rDeviceId) {

	QMutexLocker lock(&mMutex);
	return mDevices.value(rDeviceId);
}

QList<DeviceInfo> DeviceManager::getDeviceInfos() {

	QMutexLocker lock(&mMutex);
	auto ret = QList<DeviceInfo>();
	for(auto device : mDevices) {
		if(device) {
			ret.append(device->getDeviceInfo());
		}
	}
	return ret;
}

void DeviceManager::initDevices() {

	setBusy(true);
	QSettings settings;
	settings.beginGroup("devices");
	auto deviceGroup = settings.childGroups();
	for(int i = 0; i < deviceGroup.size(); ++i) {
		settings.beginGroup(deviceGroup.at(i));
		Uuid deviceId = Uuid(settings.value("id").toUuid());
		QUrl deviceEndpoint = settings.value("endpoint").toUrl();
		QString deviceName = settings.value("name").toString();
		QString username = settings.value("user").toString();
		QString password = settings.value("password").toString();
		if(!deviceId.isNull() && deviceEndpoint.isValid()) {
			auto device = QSharedPointer<Device>::create(deviceEndpoint, deviceId, deviceName, this);
			connect(
			 device.data(), &Device::unauthorized, this, [this, deviceId]() { emit unauthorized(deviceId); }, Qt::DirectConnection);
			if(!username.isNull() && !password.isNull()) {
				device->setCredentials(username, password);
			}
			mMutex.lock();
			mDevices.insert(deviceId, device);
			mMutex.unlock();
			emit deviceAdded(deviceId);
		} else {
			qWarning() << "Found invalid device from settings";
		}
		settings.endGroup();
	}

	QtConcurrent::run([this]() {
		QMutexLocker lock(&mMutex);
		for(auto device : mDevices) {
			auto result = device->initialize();
			if(device->getDeviceName().isNull()) {
				// We set a generic device name
				device->setDeviceName(getUniqueDeviceName(tr(GENERIC_DEVICE_NAME)));
			}
			if(result) {
				emit deviceInitialized(device->getDeviceId());
			}
			emit deviceChanged(device->getDeviceId());
		}
		setBusy(false);
	});
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
		for(auto device : mDevices) {
			if(probeName == device->getDeviceName()) {
				isUnique = false;
				probeName = QString("%1 %2").arg(rProposedName).arg(suffix++);
				break;
			}
		}
		mMutex.unlock();
	}
	return probeName;
}
