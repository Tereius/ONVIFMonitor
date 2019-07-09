#include "DeviceInfo.h"


DeviceInfo::DeviceInfo() :
	mDeviceId(),
	mDeviceName(),
	mInitialized(),
	mError(),
	mFirmwareVersion(),
	mHardwareId(),
	mManufacturer(),
	mModel(),
	mSerialNumber(),
	mDateTimeOffset(),
	mEndpoint(),
	mEndpointReference(),
	mEventService(),
	mMediaService() {

}

bool DeviceInfo::isNull() const {

	return mDeviceId.isNull();
}

Uuid DeviceInfo::getDeviceId() const {

	return mDeviceId;
}

void DeviceInfo::setDeviceId(const Uuid &rid) {

	mDeviceId = rid;
}

QString DeviceInfo::getFirmwareVersion() const {

	return mFirmwareVersion;
}

void DeviceInfo::setFirmwareVersion(const QString &val) {

	mFirmwareVersion = val;
}

QString DeviceInfo::getHardwareId() const {

	return mHardwareId;
}

void DeviceInfo::setHardwareId(const QString &val) {

	mHardwareId = val;
}

QString DeviceInfo::getManufacturer() const {

	return mManufacturer;
}

void DeviceInfo::setManufacturer(const QString &val) {

	mManufacturer = val;
}

QString DeviceInfo::getModel() const {

	return mModel;
}

void DeviceInfo::setModel(const QString &val) {

	mModel = val;
}

QString DeviceInfo::getSerialNumber() const {

	return mSerialNumber;
}

void DeviceInfo::setSerialNumber(const QString &val) {

	mSerialNumber = val;
}

qint64 DeviceInfo::getDateTimeOffset() const {

	return mDateTimeOffset;
}

void DeviceInfo::setDateTimeOffset(qint64 val) {

	mDateTimeOffset = val;
}

QUrl DeviceInfo::getEndpoint() const {

	return mEndpoint;
}

void DeviceInfo::setEndpoint(const QUrl &val) {

	mEndpoint = val;
}

QString DeviceInfo::getDeviceName() const {

	return mDeviceName;
}

void DeviceInfo::setDeviceName(const QString &val) {

	mDeviceName = val;
}

bool DeviceInfo::isInitialized() const {

	return mInitialized;
}

void DeviceInfo::setInitialized(bool initialized) {

	mInitialized = initialized;
}

QString DeviceInfo::getHost() const {

	return mEndpoint.host();
}

int DeviceInfo::getPort() const {

	return mEndpoint.port();
}

Uuid DeviceInfo::getEndpointReference() const {

	return mEndpointReference;
}

void DeviceInfo::setEndpointReference(const Uuid &val) {

	mEndpointReference = val;
}

QString DeviceInfo::getError() const {

	return mError;
}

void DeviceInfo::setError(const QString &rError) {

	mError = rError;
}

EventService DeviceInfo::getEventService() const {

	return mEventService;
}

void DeviceInfo::setEventService(const EventService &rService) {

	mEventService = rService;
}

MediaService DeviceInfo::getMediaService() const {

	return mMediaService;
}

void DeviceInfo::setMediaService(const MediaService &rService) {

	mMediaService = rService;
}
