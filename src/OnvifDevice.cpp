#include "OnvifDevice.h"
#include "OnvifDeviceClient.h"
#include "OnvifEventClient.h"
#include "OnvifMediaClient.h"
#include "SoapHelper.h"
#include "HttpClient.h"
#include <QTime>
#include <QScopedPointer>
#include <cmath>


OnvifDevice::OnvifDevice() : mDeviceInfo(), mpDeviceClient(nullptr), mpEventClient(nullptr), mpMediaClient(nullptr) {

	mpDeviceClient = new OnvifDeviceClient(QUrl(), SoapCtx::Builder()
#ifdef WITH_OPENSSL
	                                                .EnableSsl()
#endif
	                                                .Build());
	mpEventClient = new OnvifEventClient(QUrl(), SoapCtx::Builder()
#ifdef WITH_OPENSSL
	                                              .EnableSsl()
#endif
	                                              .Build());
	mpMediaClient = new OnvifMediaClient(QUrl(), SoapCtx::Builder()
#ifdef WITH_OPENSSL
	                                              .EnableSsl()
#endif
	                                              .Build());
}

OnvifDevice::~OnvifDevice() {

	mpDeviceClient->deleteLater();
	mpEventClient->deleteLater();
	mpMediaClient->deleteLater();
}

Result OnvifDevice::initDevice(const QUrl &rEndpoint, const QString &rUser, const QString &rPassword) {

	Result result;
	mpDeviceClient->SetEndpoint(rEndpoint);
	mpDeviceClient->GetCtx()->SetAuth(rUser, rPassword);
	mpEventClient->GetCtx()->SetAuth(rUser, rPassword);
	mpMediaClient->GetCtx()->SetAuth(rUser, rPassword);
	mDeviceInfo = {};
	mDeviceInfo.mError = "";
	mDeviceInfo.mEndpoint = rEndpoint;
	mDeviceInfo.mUser = rUser;
	mDeviceInfo.mPassword = rPassword;
	mDeviceInfo.mModel = QObject::tr("Unnamed ONVIF device");
	mDeviceInfo.mManufacturer = QString();
	mDeviceInfo.mSerialNumber = QString();
	mDeviceInfo.mDateTimeOffset = 0;

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
					mDeviceInfo.mDateTimeOffset =
					 currentDateTime.addMSecs(-std::llround(rtt / 2)).msecsTo(QDateTime(deviceDate, deviceTime, Qt::UTC));
				} else {
					qWarning() << "Couldn't extract device UTC date time";
				}
			} else {
				qWarning() << "Couldn't extract device date time";
			}

			Request<_tds__GetDeviceInformation> deviceInfoRequest;
			auto infoResponse = mpDeviceClient->GetDeviceInformation(deviceInfoRequest);
			if(infoResponse) {
				if(auto info = infoResponse.GetResultObject()) {
					mDeviceInfo.mModel = info->Model;
					mDeviceInfo.mManufacturer = info->Manufacturer;
					mDeviceInfo.mSerialNumber = info->SerialNumber;
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
							mDeviceInfo.mHasMediaService = true;
						} else {
							qInfo() << "Skipping service:" << service->Namespace;
						}
					}

					Request<_tds__GetEndpointReference> endpointReferenceRequest;
					auto endpointReferenceResponse = mpDeviceClient->GetEndpointReference(endpointReferenceRequest);
					if(endpointReferenceResponse) {
						auto endpointReference = endpointReferenceResponse.GetResultObject();
						if(endpointReference) mDeviceId = SoapHelper::QuuidFromString(endpointReference->GUID);
					} else {
						qWarning() << "Couldn't get device endpoint reference:" << endpointReferenceResponse;
					}

					if(mDeviceId.isNull()) {
						qInfo() << "The device didn't provide a GUID device endpoint reference. Fallback to default implementation";
						mDeviceId = AbstractDevice::getDeviceId();
						if(mDeviceId.isNull()) {
							qWarning() << "Couldn't get/generate a unique and persistent device id. A random one will be used";
							mDeviceId = QUuid::createUuid();
						}
					}
				} else {
					qWarning() << "Couldn't get device services:" << response;
					result = Result::fromResponse(response);
				}
			} else {
				qWarning() << "Couldn't get device info" << infoResponse;
				result = Result::fromResponse(infoResponse);
			}
		} else {
			qWarning() << "Couldn't get device date and time:" << dateTimeResponse;
			result = Result::fromResponse(dateTimeResponse, QObject::tr("Couldn't initialize device"));
		}
	} else {
		qWarning() << "Couldn't init device - invalid endpoint: " << mpDeviceClient->GetEndpoint();
		result = Result(Result::FAULT, QObject::tr("Invalid Endpoint url: %1").arg(mpDeviceClient->GetEndpoint().toString()));
	}

	mDeviceInfo.mError = result.getDetails();
	mDeviceInfo.mInitialized = result.isSuccess();

	if(mDeviceInfo.mInitialized && mDeviceInfo.mHasMediaService) {
		auto mediaProfilesResult = getyMediaProfiles();
		if(mediaProfilesResult) {
			mDeviceInfo.mMediaProfiles = mediaProfilesResult.GetResultObject();
		} else {
			qWarning() << "Couldn't get media profiles";
		}
	}

	return result;
}

QUuid OnvifDevice::getDeviceId() const {

	return mDeviceId;
}

DeviceInfo OnvifDevice::getDeviceInfo() const {

	return mDeviceInfo;
}

DetailedResult<QList<MediaProfile>> OnvifDevice::getyMediaProfiles() {

	if(!mDeviceInfo.mInitialized) {
		return DetailedResult<QList<MediaProfile>>(Result::FAULT, QObject::tr("The device is not initialized"));
	}

	if(!mDeviceInfo.mHasMediaService) {
		return DetailedResult<QList<MediaProfile>>(Result::FAULT, QObject::tr("The device has no media service"));
	}

	QList<MediaProfile> profiles;
	bool hasSnapshotCapability = false;
	bool hasStreaming = true;
	bool hasRtpOverTcp = false;
	bool hasRtpOverRtspOverTcp = false;
	bool hasRtpOverRtspOverHttpOverTcp = true; // TODO: No capability check

	Request<_trt__GetServiceCapabilities> capRequest;
	auto capResponse = mpMediaClient->GetServiceCapabilities(capRequest);
	if(capResponse && capResponse.GetResultObject()) {
		auto capability = capResponse.GetResultObject();
		if(capability->Capabilities) {
			if(capability->Capabilities->SnapshotUri) hasSnapshotCapability = *capability->Capabilities->SnapshotUri;
			if(capability->Capabilities->StreamingCapabilities) {
				if(capability->Capabilities->StreamingCapabilities->NoRTSPStreaming)
					hasStreaming = !*capability->Capabilities->StreamingCapabilities->NoRTSPStreaming;
				if(capability->Capabilities->StreamingCapabilities->RTP_USCORETCP)
					hasRtpOverTcp = *capability->Capabilities->StreamingCapabilities->RTP_USCORETCP;
				if(capability->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP)
					hasRtpOverRtspOverTcp = *capability->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP;
			}
		}
	} else {
		qInfo() << "Couldn't get media capabilities:" << capResponse;
	}

	DetailedResult<QList<MediaProfile>> result;
	Request<_trt__GetProfiles> profilesRequest;
	auto profilesResponse = mpMediaClient->GetProfiles(profilesRequest);
	if(profilesResponse) {
		for(auto profile : profilesResponse.GetResultObject()->Profiles) {
			if(profile) {
				MediaProfile retProfile(getDeviceId());
				retProfile.setName(profile->Name);
				retProfile.setToken(profile->token);
				retProfile.setFixed(profile->fixed);
				retProfile.mSnapshotCapability = hasSnapshotCapability;

				if(hasSnapshotCapability) {
					Request<_trt__GetSnapshotUri> snapshotRequest;
					snapshotRequest.ProfileToken = profile->token;
					auto snapshotRespoinse = mpMediaClient->GetSnapshotUri(snapshotRequest);
					if(snapshotRespoinse) {
						if(auto snapshotResult = snapshotRespoinse.GetResultObject()) {
							if(snapshotResult->MediaUri) retProfile.mSnapshotUrl = QUrl::fromUserInput(snapshotResult->MediaUri->Uri);
						}
					}
				}

				if(hasStreaming) {
					Request<_trt__GetStreamUri> streamRequest;
					streamRequest.ProfileToken = profile->token;
					streamRequest.StreamSetup = new tt__StreamSetup();
					streamRequest.StreamSetup->Stream = tt__StreamType::RTP_Unicast;
					streamRequest.StreamSetup->Transport = new tt__Transport();
					streamRequest.StreamSetup->Transport->Protocol = tt__TransportProtocol::UDP;
					auto streamResponse = mpMediaClient->GetStreamUri(streamRequest);
					if(streamResponse) {
						if(auto streamResult = streamResponse.GetResultObject()) {
							if(streamResult->MediaUri) {
								StreamUrl streamUrl;
								streamUrl.mProtocol = StreamUrl::SP_UDP;
								streamUrl.mUrl = QUrl::fromUserInput(streamResult->MediaUri->Uri);
								retProfile.mStreamUrls.push_back(streamUrl);
							}
						}
					}

					if(hasRtpOverTcp) {
						streamRequest.StreamSetup->Transport->Protocol = tt__TransportProtocol::TCP;
						streamResponse = mpMediaClient->GetStreamUri(streamRequest);
						if(streamResponse) {
							if(auto streamResult = streamResponse.GetResultObject()) {
								if(streamResult->MediaUri) {
									StreamUrl streamUrl;
									streamUrl.mProtocol = StreamUrl::SP_TCP;
									streamUrl.mUrl = QUrl::fromUserInput(streamResult->MediaUri->Uri);
									retProfile.mStreamUrls.push_back(streamUrl);
								}
							}
						}
					}

					if(hasRtpOverRtspOverTcp) {
						streamRequest.StreamSetup->Transport->Protocol = tt__TransportProtocol::RTSP;
						streamResponse = mpMediaClient->GetStreamUri(streamRequest);
						if(streamResponse) {
							if(auto streamResult = streamResponse.GetResultObject()) {
								if(streamResult->MediaUri) {
									StreamUrl streamUrl;
									streamUrl.mProtocol = StreamUrl::SP_RTSP;
									streamUrl.mUrl = QUrl::fromUserInput(streamResult->MediaUri->Uri);
									retProfile.mStreamUrls.push_back(streamUrl);
								}
							}
						}
					}

					if(hasRtpOverRtspOverHttpOverTcp) {
						streamRequest.StreamSetup->Transport->Protocol = tt__TransportProtocol::HTTP;
						streamResponse = mpMediaClient->GetStreamUri(streamRequest);
						if(streamResponse) {
							if(auto streamResult = streamResponse.GetResultObject()) {
								if(streamResult->MediaUri) {
									StreamUrl streamUrl;
									streamUrl.mProtocol = StreamUrl::SP_HTTP;
									streamUrl.mUrl = QUrl::fromUserInput(streamResult->MediaUri->Uri);
									retProfile.mStreamUrls.push_back(streamUrl);
								}
							}
						}
					}
				}
				profiles << retProfile;
			}
		}
		result.setResultObject(profiles);
	} else {
		qWarning() << "Couldn't get media profiles:" << profilesResponse;
		return DetailedResult<QList<MediaProfile>>::fromResponse(profilesResponse, QObject::tr("Couldn't load profiles"));
	}
	return result;
}

DetailedResult<QImage> OnvifDevice::getSnapshot(const QString &rMediaProfile) {

	if(!mDeviceInfo.mInitialized) {
		return DetailedResult<QImage>(Result::FAULT, QObject::tr("The device is not initialized"));
	}

	if(!mDeviceInfo.mHasMediaService) {
		return DetailedResult<QImage>(Result::FAULT, QObject::tr("The device has no media service"));
	}

	MediaProfile profile;
	for(const auto &rProfile : mDeviceInfo.mMediaProfiles) {
		if(rProfile.getToken().compare(rMediaProfile) == 0) {
			profile = rProfile;
			break;
		}
	}
	if(profile.isValid()) {
		if(profile.mSnapshotCapability && profile.mSnapshotUrl.isValid()) {
			if(auto ctx = mpDeviceClient->GetCtx()) {
				QScopedPointer<HttpClient> httpClient(new HttpClient(ctx));
				auto response = httpClient->get(profile.mSnapshotUrl);
				if(response && !response.GetResultObject().isEmpty()) {
					auto image = QImage::fromData(response.GetResultObject());
					return DetailedResult<QImage>(image);
				}
				qWarning() << "Snapshot failed:" << response;
				return DetailedResult<QImage>(Result::FAULT, QObject::tr("Snapshot failed. %1").arg(response.GetCompleteFault()));
			} else {
				return DetailedResult<QImage>(Result::FAULT, QObject::tr("Snapshot failed. Couldn't get soap context"));
			}
		} else {
			return DetailedResult<QImage>(
			 Result::FAULT, QObject::tr("The device doesn't provide a snapshot url for the media profile \"%1\"").arg(rMediaProfile));
		}
	} else {
		return DetailedResult<QImage>(Result::FAULT,
		                              QObject::tr("The device doesn't provide a media profile with name \"%1\"").arg(rMediaProfile));
	}
}
