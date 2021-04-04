#include "AbstractDevice.h"
#include <QCryptographicHash>
#include <cstring>


QUuid AbstractDevice::getDeviceId() const {

	QUuid id{};
	auto serial = getSerialNumber();
	auto manufacturer = getManufacturer();
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

QString AbstractDevice::getHost() const {

	return getEndpoint().host();
}

int AbstractDevice::getPort() const {

	if(getEndpoint().scheme().compare("http") == 0)
		return getEndpoint().port(80);
	else if(getEndpoint().scheme().compare("https") == 0)
		return getEndpoint().port(443);
	return getEndpoint().port();
}