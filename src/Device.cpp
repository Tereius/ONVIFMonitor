#include "Device.h"
#include "DeviceInfo.h"
#include "OnvifDeviceClient.h"
#include "OnvifEventClient.h"
#include "OnvifMediaClient.h"
#include <QDateTime>
#include <QMutexLocker>


#define shortId mDeviceInfo.getDeviceId().toShortString().toLocal8Bit().constData()

Device::Device(const QUrl &rDeviceEndpoint, const Uuid &rDeviceId, const QString &rDeviceName /*= QString()*/, QObject *pParent /*= nullptr*/) :
	QObject(pParent),
	mDeviceEndpoint(rDeviceEndpoint),
	mpDeviceClient(new OnvifDeviceClient(rDeviceEndpoint, QSharedPointer<SoapCtx>::create(), this)),
	mpEventClient(new OnvifEventClient(rDeviceEndpoint, mpDeviceClient->GetCtx(), this)), // Initialize with wrong url. Use a shared soap ctx
	mpMediaClient(new OnvifMediaClient(rDeviceEndpoint, mpDeviceClient->GetCtx(), this)), // Initialize with wrong url. Use a shared soap ctx
	mDeviceInfo(),
	mMutex(QMutex::Recursive) {

	mDeviceInfo.setDeviceId(rDeviceId);
	mDeviceInfo.setEndpoint(rDeviceEndpoint);
	mDeviceInfo.setDeviceName(rDeviceName);
	mDeviceInfo.setInitialized(false);
}

Device::~Device() {

}

Result Device::initialize() {

	qInfo() << shortId << "device initialization";
	auto result = Result();
	mMutex.lock();
	mDeviceInfo.setInitialized(false);
	mDeviceInfo.setError(QString());
	mDeviceInfo.setEventService(EventService());
	mDeviceInfo.setMediaService(MediaService());
	mMutex.unlock();
	if(mpDeviceClient->GetEndpoint().isValid()) {
		Request<_tds__GetSystemDateAndTime> dateTimeRequest;
		auto timestamp = QDateTime::currentMSecsSinceEpoch();
		auto dateTimeResponse = mpDeviceClient->GetSystemDateAndTime(dateTimeRequest);
		if(dateTimeResponse) {
			if(auto sysDateTime = dateTimeResponse.GetResultObject()->SystemDateAndTime) {
				bool daylightSaving = sysDateTime->DaylightSavings;
				if(sysDateTime->UTCDateTime && sysDateTime->UTCDateTime->Time && sysDateTime->UTCDateTime->Date) {
					auto deviceTime = QTime(sysDateTime->UTCDateTime->Time->Hour, sysDateTime->UTCDateTime->Time->Minute, sysDateTime->UTCDateTime->Time->Second);
					auto deviceDate = QDate(sysDateTime->UTCDateTime->Date->Year, sysDateTime->UTCDateTime->Date->Month, sysDateTime->UTCDateTime->Date->Day);
					auto rtt = QDateTime::currentMSecsSinceEpoch() - timestamp;
					mMutex.lock();
					mDeviceInfo.setDateTimeOffset(QDateTime::currentMSecsSinceEpoch() - timestamp - QDateTime(deviceDate, deviceTime, Qt::UTC).toMSecsSinceEpoch() - rtt / 2);
					mMutex.unlock();
				}
				else {
					qWarning() << shortId << "Couldn't extract device UTC date time";
				}
			}
			else {
				qWarning() << shortId << "Couldn't extract device date time";
			}

			Request<_tds__GetEndpointReference> endpointRefRequest;
			auto endpointRefResponse = mpDeviceClient->GetEndpointReference(endpointRefRequest);
			if(endpointRefResponse) {
				if(auto endpointRef = endpointRefResponse.GetResultObject()) {
					mMutex.lock();
					mDeviceInfo.setEndpointReference(endpointRef->GUID);
					mMutex.unlock();
				}
				else {
					qWarning() << shortId << "Couldn't extract the device endpoint reference";
				}
			}
			else {
				qWarning() << shortId << "Couldn't get the device endpoint reference";
			}
			result = initServices();
			if(result) {
				result = initDeviceInfo();
			}
		}
		else {
			qWarning() << shortId << "Couldn't get device date and time:" << dateTimeResponse.GetCompleteFault();
			result = Result::fromResponse(dateTimeResponse, tr("Couldn't initialize device"));
		}
	}
	else {
		qWarning() << shortId << "Couldn't init device - invalid endpoint: " << mpDeviceClient->GetEndpoint();
		result = Result(Result::FAULT, tr("Invalid Endpoint: %1").arg(mpDeviceClient->GetEndpoint().toString()));
	}
	if(!result) {
		mMutex.lock();
		mDeviceInfo.setError(result.getLabel());
		mMutex.unlock();
	}
	return result;
}

DeviceInfo Device::getDeviceInfo() {

	QMutexLocker lock(&mMutex);
	return mDeviceInfo;
}

QString Device::getDeviceName() {

	QMutexLocker lock(&mMutex);
	return mDeviceInfo.getDeviceName();
}

Uuid Device::getDeviceId() {

	QMutexLocker lock(&mMutex);
	return mDeviceInfo.getDeviceId();
}

void Device::setDeviceName(const QString &rDeviceName) {

	QMutexLocker lock(&mMutex);
	mDeviceInfo.setDeviceName(rDeviceName);
}

void Device::setCredentials(const QString &rUser, const QString &rPassword) {

	mpDeviceClient->SetAuth(rUser, rPassword);
	mpMediaClient->SetAuth(rUser, rPassword);
	mpEventClient->SetAuth(rUser, rPassword);
}

Result Device::setHost(const QUrl &rHost) {

	Result result;
	auto scheme = rHost.scheme();
	auto user = rHost.userName();
	auto pwd = rHost.password();
	auto host = rHost.host();
	auto port = rHost.port();
	auto query = rHost.query();
	auto path = rHost.path();

	auto oldEndpoint = mpDeviceClient->GetEndpoint();
	if(!scheme.isNull()) {
		oldEndpoint.setScheme(scheme);
	}
	if(!user.isNull() || !pwd.isNull()) {
		qWarning() << shortId << "Ignoring the credentials provided via url";
	}
	if(!host.isNull()) {
		oldEndpoint.setHost(host);
	}
	if(port > -1) {
		oldEndpoint.setPort(port);
	}
	if(!query.isNull()) {
		oldEndpoint.setQuery(query);
	}
	if(!path.isNull()) {
		oldEndpoint.setPath(path);
	}
	if(oldEndpoint.isValid()) {
		qInfo() << shortId << "Changed endpoint from:" << mpDeviceClient->GetEndpoint().toString() << "To:" << oldEndpoint.toString();
		mMutex.lock();
		mDeviceInfo.setEndpoint(oldEndpoint);
		mMutex.unlock();
		mpDeviceClient->SetEndpoint(oldEndpoint);
	}
	else {
		qWarning() << shortId << "The url is invalid. Couldn't change the endpoint: " << oldEndpoint.errorString();
		result = Result(Result::FAULT, tr("Url is invalid"));
		result.setDetails(oldEndpoint.errorString());
	}
	return result;
}

DetailedResult<QList<MediaProfile>> Device::getMediaProfiles() {

	DetailedResult<QList<MediaProfile>> result;
	Request<_trt__GetProfiles> profilesRequest;
	auto profilesResponse = mpMediaClient->GetProfiles(profilesRequest);
	if(profilesResponse) {
		QList<MediaProfile> profiles;
		for(auto profile : profilesResponse.GetResultObject()->Profiles) {
			mMutex.lock();
			MediaProfile retProfile(mDeviceInfo.getDeviceId());
			mMutex.unlock();
			retProfile.setName(profile->Name);
			retProfile.setToken(profile->token);
			retProfile.setFixed(profile->fixed);
			profiles << retProfile;
		}
		result.setResultObject(profiles);
	}
	else {
		qWarning() << shortId << "Couldn't get media profiles:" << profilesResponse.GetCompleteFault();
		result = DetailedResult<QList<MediaProfile>>::fromResponse(profilesResponse, tr("Couldn't load profiles"));
	}
	return result;
}

DetailedResult<MediaProfile> Device::getMediaProfile(const QString &rProfileToken) const {

	DetailedResult<MediaProfile> result;
	Request<_trt__GetProfile> profilesRequest;
	profilesRequest.ProfileToken = rProfileToken;
	auto profileResponse = mpMediaClient->GetProfile(profilesRequest);
	if(profileResponse) {
		auto profile = profileResponse.GetResultObject()->Profile;
		MediaProfile retProfile;
		retProfile.setName(profile->Name);
		retProfile.setToken(profile->token);
		retProfile.setFixed(profile->fixed);
		result.setResultObject(retProfile);
	}
	else {
		qWarning() << shortId << "Couldn't get media profile:" << profileResponse.GetCompleteFault();
		result = DetailedResult<MediaProfile>::fromResponse(profileResponse, tr("Couldn't load profile"));
	}
	return result;
}

DetailedResult<QList<Topic>> Device::getTopics() const {

	DetailedResult<QList<Topic>> result;
	Request<_tev__GetEventProperties> request;
	auto response = mpEventClient->GetParsedEventProperties(request);
	if(response) {
		result.setResultObject(response.GetResultObject().GetTopics());
	}
	else {
		qWarning() << shortId << "Couldn't get topics:" << response.GetCompleteFault();
		result = DetailedResult<QList<Topic>>::fromResponse(response, tr("Couldn't load topics"));
	}
	return result;
}

QUrl Device::getStreamUrl(const QString &rProfileToken) const {

	QUrl ret;
	Request<_trt__GetStreamUri> streamUriRequest;
	streamUriRequest.ProfileToken = rProfileToken;
	streamUriRequest.StreamSetup = new tt__StreamSetup();
	streamUriRequest.StreamSetup->Stream = tt__StreamType::RTP_Unicast;
	streamUriRequest.StreamSetup->Transport = new tt__Transport();
	streamUriRequest.StreamSetup->Transport->Protocol = tt__TransportProtocol::UDP;
	auto streamUriResponse = mpMediaClient->GetStreamUri(streamUriRequest);
	if(streamUriResponse) {
		if(auto streamUri = streamUriResponse.GetResultObject()->MediaUri) {
			ret = QUrl(streamUri->Uri);
		}
		else {
			qWarning() << shortId << "Couldn't extract streaming Uri";
		}
	}
	else {
		qWarning() << shortId << "Couldn't get media profiles:" << streamUriResponse.GetCompleteFault();
	}
	return ret;
}

Result Device::initDeviceDateTime() {

	Result result;
	Request<_tds__GetSystemDateAndTime> dateTimeRequest;
	auto timestamp = QDateTime::currentMSecsSinceEpoch();
	auto dateTimeResponse = mpDeviceClient->GetSystemDateAndTime(dateTimeRequest);
	if(dateTimeResponse) {
		if(auto sysDateTime = dateTimeResponse.GetResultObject()->SystemDateAndTime) {
			bool daylightSaving = sysDateTime->DaylightSavings;
			if(sysDateTime->UTCDateTime && sysDateTime->UTCDateTime->Time && sysDateTime->UTCDateTime->Date) {
				auto deviceTime = QTime(sysDateTime->UTCDateTime->Time->Hour, sysDateTime->UTCDateTime->Time->Minute, sysDateTime->UTCDateTime->Time->Second);
				auto deviceDate = QDate(sysDateTime->UTCDateTime->Date->Year, sysDateTime->UTCDateTime->Date->Month, sysDateTime->UTCDateTime->Date->Day);
				auto rtt = QDateTime::currentMSecsSinceEpoch() - timestamp;
				mMutex.lock();
				mDeviceInfo.setDateTimeOffset(QDateTime::currentMSecsSinceEpoch() - timestamp - QDateTime(deviceDate, deviceTime, Qt::UTC).toMSecsSinceEpoch() - rtt / 2);
				mMutex.unlock();
			}
			else {
				qWarning() << shortId << "Couldn't extract device UTC date time";
			}
		}
		else {
			qWarning() << shortId << "Couldn't extract device date time";
		}
	}
	else {
		qWarning() << shortId << "Couldn't get device date and time:" << dateTimeResponse.GetCompleteFault();
		result = Result::fromResponse(dateTimeResponse, tr("Couldn't initialize device"));
	}
	return result;
}

Result Device::initServices() {

	Result result;
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
				qInfo() << shortId << "Found device service:" << service->XAddr;
				// Device Service
			}
			else if(service->Namespace == OnvifEventClient::GetServiceNamespace()) {
				// Event Service
				qInfo() << shortId << "Found event service:" << service->XAddr;
				auto eventService = EventService();
				eventService.setServiceEndpoint(QUrl(service->XAddr));
				mMutex.lock();
				mDeviceInfo.setEventService(eventService);
				mMutex.unlock();
				result = initEventCapab();
			}
			else if(service->Namespace == OnvifMediaClient::GetServiceNamespace()) {
				// Media Service
				qInfo() << shortId << "Found media service:" << service->XAddr;
				auto mediaService = MediaService();
				mediaService.setServiceEndpoint(QUrl(service->XAddr));
				mMutex.lock();
				mDeviceInfo.setMediaService(mediaService);
				mMutex.unlock();
				result = initMediaCapab();
			}
			else {
				qInfo() << shortId << "Skipping service:" << service->Namespace;
			}
		}
	}
	else {
		qWarning() << shortId << "Couldn't get device services:" << response.GetCompleteFault();
		result = Result::fromResponse(response);
	}
	return result;
}

Result Device::initDeviceCapab() {

	Result result;
	if(mpDeviceClient) {

	}
	else {
		result = Result(Result::FAULT, "Device service not initialized");
	}
	return result;
}

Result Device::initEventCapab() {

	Result result;
	if(mpEventClient) {
		Request<_tev__GetServiceCapabilities> request;
		auto response = mpEventClient->GetServiceCapabilities(request);
		if(response) {
			auto capabilities = response.GetResultObject();
			if(auto capa = capabilities->Capabilities) {
				mMutex.lock();
				auto service = mDeviceInfo.getEventService();
				if(capa->MaxNotificationProducers) service.setMaxNotificationProducers(*capa->MaxNotificationProducers);
				if(capa->MaxPullPoints) service.setMaxPullPoints(*capa->MaxPullPoints);
				if(capa->PersistentNotificationStorage) service.setPersistentNotificationStorage(*capa->PersistentNotificationStorage);
				if(capa->WSPausableSubscriptionManagerInterfaceSupport) service.setWSPausableSubscriptionManagerInterfaceSupport(*capa->WSPausableSubscriptionManagerInterfaceSupport);
				if(capa->WSPullPointSupport) service.setWSPullPointSupport(*capa->WSPullPointSupport);
				if(capa->WSSubscriptionPolicySupport) service.setWSSubscriptionPolicySupport(*capa->WSSubscriptionPolicySupport);
				service.setInitialized(true);
				mDeviceInfo.setEventService(service);
				mMutex.unlock();
			}
			else {
				qWarning() << shortId << "Couldn't get event service capabilities";
			}
		}
		else {
			qWarning() << shortId << "Couldn't get event service capabilities:" << response.GetCompleteFault();
			result = Result::fromResponse(response);
		}
	}
	else {
		result = Result(Result::FAULT, "Event service not initialized");
	}
	return result;
}

Result Device::initMediaCapab() {

	Result result;
	if(mpMediaClient) {
		Request<_trt__GetServiceCapabilities> request;
		auto response = mpMediaClient->GetServiceCapabilities(request);
		if(response) {
			auto capabilities = response.GetResultObject();
			if(auto capa = capabilities->Capabilities) {
				mMutex.lock();
				auto service = mDeviceInfo.getMediaService();
				if(capa->EXICompression) service.setEXICompression(*capa->EXICompression);
				if(capa->OSD) service.setOSD(*capa->OSD);
				if(capa->Rotation) service.setRotation(*capa->Rotation);
				if(capa->SnapshotUri) service.setSnapshotUri(*capa->SnapshotUri);
				if(capa->TemporaryOSDText) service.setTemporaryOSDText(capa->TemporaryOSDText);
				if(capa->VideoSourceMode) service.setVideoSourceMode(*capa->VideoSourceMode);
				if(auto profileCapa = capa->ProfileCapabilities) {
					service.setMaximumNumberOfProfiles(*profileCapa->MaximumNumberOfProfiles);
				}
				if(auto streamingCapa = capa->StreamingCapabilities) {
					if(streamingCapa->NonAggregateControl) service.setNonAggregateControl(*streamingCapa->NonAggregateControl);
					if(streamingCapa->NoRTSPStreaming) service.setNoRTSPStreaming(*streamingCapa->NoRTSPStreaming);
					if(streamingCapa->RTP_USCORERTSP_USCORETCP) service.setRTPRtspTcp(*streamingCapa->RTP_USCORERTSP_USCORETCP);
					if(streamingCapa->RTP_USCORETCP) service.setRTPTcp(*streamingCapa->RTP_USCORETCP);
					if(streamingCapa->RTPMulticast) service.setRTPMulticast(*streamingCapa->RTPMulticast);
				}
				service.setInitialized(true);
				mDeviceInfo.setMediaService(service);
				mMutex.unlock();
			}
			else {
				qWarning() << shortId << "Couldn't get media service capabilities";
			}
		}
		else {
			qWarning() << shortId << "Couldn't get media service capabilities:" << response.GetCompleteFault();
			result = Result::fromResponse(response);
		}
	}
	else {
		result = Result(Result::FAULT, "Media service not initialized");
	}
	return result;
}

Result Device::initDeviceInfo() {

	Result result;
	Request<_tds__GetDeviceInformation> infoRequest;
	auto infoResponse = mpDeviceClient->GetDeviceInformation(infoRequest);
	if(infoResponse) {
		mMutex.lock();
		mDeviceInfo.setFirmwareVersion(infoResponse.GetResultObject()->FirmwareVersion);
		mDeviceInfo.setManufacturer(infoResponse.GetResultObject()->Manufacturer);
		mDeviceInfo.setHardwareId(infoResponse.GetResultObject()->HardwareId);
		mDeviceInfo.setModel(infoResponse.GetResultObject()->Model);
		mDeviceInfo.setSerialNumber(infoResponse.GetResultObject()->SerialNumber);
		mDeviceInfo.setInitialized(true);
		if(mDeviceInfo.getDeviceName().isNull()) {
			mDeviceInfo.setDeviceName(mDeviceInfo.getModel());
		}
		mMutex.unlock();
	}
	else {
		qWarning() << shortId << "Couldn't get device info:" << infoResponse.GetCompleteFault();
		result = Result::fromResponse(infoResponse);
	}
	return result;
}

void Device::watchEvent() {

}
