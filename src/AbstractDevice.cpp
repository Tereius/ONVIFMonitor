#include "AbstractDevice.h"
#include "HttpClient.h"
#include "Util.h"
#include "asyncfuture.h"
#include "mdk/Player.h"
#include <QCryptographicHash>
#include <QSharedPointer>
#include <QtConcurrent>
#include <cstring>


QUuid AbstractDevice::getDeviceId() const {

	QUuid id{};
	auto deviceInfo = getDeviceInfo();
	auto serial = deviceInfo.mSerialNumber;
	auto manufacturer = deviceInfo.mManufacturer;
	Q_ASSERT(!serial.isEmpty());
	if(!serial.isEmpty()) {
		auto hash = QCryptographicHash::hash(serial.toUtf8() + manufacturer.toUtf8(), QCryptographicHash::Md5);
		Q_ASSERT(hash.size() == 16);
		if(hash.size() >= 16) {
			std::memcpy(&id.data1, hash.data(), 4);
			std::memcpy(&id.data2, hash.data() + 4, 2);
			std::memcpy(&id.data3, hash.data() + 6, 2);
			std::memcpy(&id.data4, hash.data() + 8, 8);
		} else {
			qWarning() << "Couldn't derive device id from device serial number: The has shorter than 16 Bytes";
		}
	} else {
		qWarning() << "Couldn't derive device id from device serial number: The serial number is empty";
	}
	return id;
}

QDateTime AbstractDevice::getDeviceTime() const {

	return QDateTime::currentDateTimeUtc().addMSecs(getDeviceInfo().mDateTimeOffset);
}

QString AbstractDevice::getHost() const {

	return getDeviceInfo().mEndpoint.host();
}

int AbstractDevice::getPort() const {

	return Util::getDefaultPort(getDeviceInfo().mEndpoint);
}
