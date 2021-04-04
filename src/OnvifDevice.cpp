#include "OnvifDevice.h"
#include "OnvifDeviceClient.h"
#include "OnvifEventClient.h"
#include "OnvifMediaClient.h"
#include <QTime>
#include <cmath>


OnvifDevice::OnvifDevice() :
 mDeviceEndpoint(),
 mDeviceId(),
 mModel(),
 mManufacturer(),
 mSerialNumber(),
 mTimeDifference(0),
 mpDeviceClient(new OnvifDeviceClient(QUrl(), QSharedPointer<SoapCtx>::create())),
 mpEventClient(new OnvifEventClient(QUrl(), mpDeviceClient->GetCtx())),
 mpMediaClient(new OnvifMediaClient(QUrl(), mpDeviceClient->GetCtx())) {}

OnvifDevice::~OnvifDevice() {

	mpDeviceClient->deleteLater();
	mpEventClient->deleteLater();
	mpMediaClient->deleteLater();
}

Result OnvifDevice::initDevice(const QUrl &rEndpoint, const QString &rUser, const QString &rPassword) {

	Result result;
	mpDeviceClient->SetEndpoint(rEndpoint);
	mpDeviceClient->GetCtx()->SetAuth(rUser, rPassword);
	if(mpDeviceClient->GetEndpoint().isValid()) {
		Request<_tds__GetSystemDateAndTime> dateTimeRequest;
		auto timestamp = QDateTime::currentMSecsSinceEpoch();
		auto dateTimeResponse = mpDeviceClient->GetSystemDateAndTime(dateTimeRequest);
		if(dateTimeResponse) {
			if(auto sysDateTime = dateTimeResponse.GetResultObject()->SystemDateAndTime) {
				bool daylightSaving = sysDateTime->DaylightSavings;
				if(sysDateTime->UTCDateTime && sysDateTime->UTCDateTime->Time && sysDateTime->UTCDateTime->Date) {
					auto deviceTime =
					 QTime(sysDateTime->UTCDateTime->Time->Hour, sysDateTime->UTCDateTime->Time->Minute, sysDateTime->UTCDateTime->Time->Second);
					auto deviceDate =
					 QDate(sysDateTime->UTCDateTime->Date->Year, sysDateTime->UTCDateTime->Date->Month, sysDateTime->UTCDateTime->Date->Day);
					auto currentDateTime = QDateTime::currentDateTimeUtc();
					auto rtt = currentDateTime.toMSecsSinceEpoch() - timestamp;
					mTimeDifference = currentDateTime.addMSecs(-std::llround(rtt / 2)).msecsTo(QDateTime(deviceDate, deviceTime, Qt::UTC));
				} else {
					qWarning() << "Couldn't extract device UTC date time";
				}
			} else {
				qWarning() << "Couldn't extract device date time";
			}
		} else {
			qWarning() << "Couldn't get device date and time:" << dateTimeResponse.GetCompleteFault();
			result = Result::fromResponse(dateTimeResponse, QObject::tr("Couldn't initialize device"));
		}

		Request<_tds__GetDeviceInformation> deviceInfoRequest;
		auto infoResponse = mpDeviceClient->GetDeviceInformation(deviceInfoRequest);
		if(infoResponse) {
			if(auto info = infoResponse.GetResultObject()) {
				mModel = info->Model;
				mManufacturer = info->Manufacturer;
				mSerialNumber = info->SerialNumber;
			}
		}

		Request<_tds__GetServices> servicesRequest;
		servicesRequest.IncludeCapability = false;
		auto response = mpDeviceClient->GetServices(servicesRequest);
		if(response) {
			auto services = response.GetResultObject();
			for(auto service : services->Service) {
				if(result.isFault()) {
					break;
				}
				if(service->Namespace == OnvifDeviceClient::GetServiceNamespace()) {
					qInfo() << "Found device service:" << service->XAddr;
					// Device Service
				} else if(service->Namespace == OnvifEventClient::GetServiceNamespace()) {
					// Event Service
					qInfo() << "Found event service:" << service->XAddr;
					mpEventClient->SetEndpoint(QUrl(service->XAddr));
				} else if(service->Namespace == OnvifMediaClient::GetServiceNamespace()) {
					// Media Service
					qInfo() << "Found media service:" << service->XAddr;
					mpMediaClient->SetEndpoint(QUrl(service->XAddr));
				} else {
					qInfo() << "Skipping service:" << service->Namespace;
				}
			}
		} else {
			qWarning() << "Couldn't get device services:" << response.GetCompleteFault();
			result = Result::fromResponse(response);
		}
	} else {
		qWarning() << "Couldn't init device - invalid endpoint: " << mpDeviceClient->GetEndpoint();
		result = Result(Result::FAULT, QObject::tr("Invalid Endpoint url: %1").arg(mpDeviceClient->GetEndpoint().toString()));
	}
	return result;
}

DetailedResult<DeviceInfo> OnvifDevice::getDeviceInfo() {
	return DetailedResult<DeviceInfo>();
}

DetailedResult<QList<MediaProfile>> OnvifDevice::getyMediaProfiles() {
	return DetailedResult<QList<MediaProfile>>();
}

DetailedResult<QUrl> OnvifDevice::getStreamUrl(const QString &rMediaProfile) {
	return DetailedResult<QUrl>();
}
QUuid OnvifDevice::getDeviceId() const {

	return mDeviceId;
}
QString OnvifDevice::getModel() const {

	return mModel;
}
QString OnvifDevice::getManufacturer() const {

	return mManufacturer;
}

QString OnvifDevice::getSerialNumber() const {

	return mSerialNumber;
}

QUrl OnvifDevice::getEndpoint() const {

	if(mpDeviceClient) return mpDeviceClient->GetEndpoint();
	return {};
}

QDateTime OnvifDevice::getDeviceTime() const {

	return QDateTime::currentDateTime().addMSecs(mTimeDifference);
}
