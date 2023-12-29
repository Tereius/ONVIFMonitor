#include "OnvifDevice.h"
#include "App.h"
#include "HttpClient.h"
#include "MediaPlayer.h"
#include "OnvifDeviceClient.h"
#include "OnvifEventClient.h"
#include "OnvifMedia2Client.h"
#include "OnvifMediaClient.h"
#include "OnvifRtspClient.h"
#include "SoapHelper.h"
#include "asyncfuture.h"
#include "mdk/Player.h"
#include <QScopedPointer>
#include <QtConcurrent>
#include <cmath>


DetailedResult<QImage> getSnapshotInternal(const MediaProfile &profile, const QSize &rSize, const QSharedPointer<HttpClient> &httpClient,
                                           QSharedPointer<QAtomicInt> isCanceled, const QString &rUser, const QString &rPassword,
                                           bool forceSnapshotFromStream) {

	DetailedResult<QImage> result(Result::FAULT, "Generic snapshot fault");
	if(profile.mSnapshotCapability && profile.mSnapshotUrl.isValid() && !forceSnapshotFromStream && isCanceled->loadRelaxed() == false) {
		auto response = httpClient->get(profile.mSnapshotUrl);
		if(response && !response.GetResultObject().isEmpty() && isCanceled->loadRelaxed() == false) {
			auto image = QImage::fromData(response.GetResultObject());
			if(rSize.isValid()) {
				image = image.scaled(rSize, Qt::KeepAspectRatio);
			}
			result = DetailedResult<QImage>(image);
		} else {
			result = DetailedResult<QImage>::fromResponse(response);
		}
	}
	if(!result && isCanceled->loadRelaxed() == false) {
		qInfo() << "Fallback to getSnapshotFromStream()";
		if(!profile.mStreamUrls.isEmpty()) {
			for(auto streamUrl : profile.mStreamUrls) {
				result = MediaPlayer::getSnapshot(streamUrl);
				if(result) {
					break;
				}
			}
		} else {
			result =
			 (DetailedResult<QImage>(Result::FAULT, QObject::tr("The device doesn't provide any stream urls \"%1\"").arg(profile.getToken())));
		}
	}
	if(isCanceled->loadRelaxed() == true) {
		result = DetailedResult<QImage>(Result::FAULT, QObject::tr("The snapshot request was canceled"));
	}
	return result;
}

OnvifDevice::OnvifDevice() :
 AbstractDevice(), mDeviceInfo(), mpDeviceClient(nullptr), mpEventClient(nullptr), mpMediaClient(nullptr), mpMedia2Client(nullptr) {

	mpDeviceClient = new OnvifDeviceClient(QUrl(), SoapCtx::Builder()
	                                                .SetUserAgent(App::getDefaultUserAgent())
#ifdef WITH_OPENSSL
	                                                .EnableSsl()
#endif
	                                                .Build());
	mpEventClient = new OnvifEventClient(QUrl(), SoapCtx::Builder()
	                                              .SetUserAgent(App::getDefaultUserAgent())
#ifdef WITH_OPENSSL
	                                              .EnableSsl()
#endif
	                                              .Build());
	mpMediaClient = new OnvifMediaClient(QUrl(), SoapCtx::Builder()
	                                              .SetUserAgent(App::getDefaultUserAgent())
#ifdef WITH_OPENSSL
	                                              .EnableSsl()
#endif
	                                              .Build());
	mpMedia2Client = new OnvifMedia2Client(QUrl(), SoapCtx::Builder()
	                                                .SetUserAgent(App::getDefaultUserAgent())
#ifdef WITH_OPENSSL
	                                                .EnableSsl()
#endif
	                                                .Build());
}

OnvifDevice::~OnvifDevice() {

	mpDeviceClient->deleteLater();
	mpEventClient->deleteLater();
	mpMediaClient->deleteLater();
	mpMedia2Client->deleteLater();
}

Result OnvifDevice::initDevice(const QUrl &rEndpoint, const QString &rUser, const QString &rPassword) {

	Result result;
	mpDeviceClient->SetEndpoint(rEndpoint);
	mpDeviceClient->GetCtx()->SetAuth(rUser, rPassword);
	mpEventClient->GetCtx()->SetAuth(rUser, rPassword);
	mpMediaClient->GetCtx()->SetAuth(rUser, rPassword);
	mpMedia2Client->GetCtx()->SetAuth(rUser, rPassword);
	mDeviceInfo = {};
	mDeviceInfo.mError = "";
	mDeviceInfo.mEndpoint = rEndpoint;
	mDeviceInfo.mUser = rUser;
	mDeviceInfo.mPassword = rPassword;
	mDeviceInfo.mScopes = QStringList();
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

			Request<_tds__GetScopes> deviceScopeRequest;
			auto scopeResponse = mpDeviceClient->GetScopes(deviceScopeRequest);
			if(scopeResponse) {
				if(auto scopeResult = scopeResponse.GetResultObject()) {
					for(auto scope : scopeResult->Scopes) {
						if(scope) {
							mDeviceInfo.mScopes.push_back(scope->ScopeItem);
						}
					}
				}
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
						} else if(service->Namespace == OnvifMedia2Client::GetServiceNamespace()) {
							// Media2 Service
							qInfo() << "Found media2 service:" << service->XAddr;
							mpMedia2Client->SetEndpoint(QUrl(service->XAddr));
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
		// auto media2ProfilesResult = getMedia2Profiles();
		auto mediaProfilesResult = getMediaProfiles();
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

Result OnvifDevice::pingDevice() const {

	if(mpDeviceClient->GetEndpoint().isValid()) {
		Request<_tds__GetSystemDateAndTime> dateTimeRequest;
		auto dateTimeResponse = mpDeviceClient->GetSystemDateAndTime(dateTimeRequest);
		return Result::fromResponse(dateTimeResponse);
	}
	return Result(Result::FAULT, QObject::tr("Invalid Endpoint url: %1").arg(mpDeviceClient->GetEndpoint().toString()));
}

DeviceInfo OnvifDevice::getDeviceInfo() const {

	return mDeviceInfo;
}

DetailedResult<QList<MediaProfile>> OnvifDevice::getMediaProfiles() {

	if(!mDeviceInfo.mInitialized) {
		return DetailedResult<QList<MediaProfile>>(Result::FAULT, QObject::tr("The device is not initialized"));
	}

	if(!mDeviceInfo.mHasMediaService) {
		return DetailedResult<QList<MediaProfile>>(Result::FAULT, QObject::tr("The device has no media service"));
	}

	QList<MediaProfile> profiles;
	bool hasSnapshotCapability = false;
	bool hasStreaming = true;
	bool hasRtpOverTcp = true;
	bool hasRtpOverRtspOverTcp = true;
	bool hasRtpOverRtspOverHttpOverTcp = false; // TODO: No capability check - disable for now doesn't work with test devices
	bool hasMulticast = true;

	Request<_trt__GetServiceCapabilities> capRequest;
	auto capResponse = mpMediaClient->GetServiceCapabilities(capRequest);
	if(capResponse && capResponse.GetResultObject()) {
		const auto *capability = capResponse.GetResultObject();
		if(capability->Capabilities) {
			if(capability->Capabilities->SnapshotUri) hasSnapshotCapability = *capability->Capabilities->SnapshotUri;
			if(capability->Capabilities->StreamingCapabilities) {
				if(capability->Capabilities->StreamingCapabilities->NoRTSPStreaming)
					hasStreaming = !*capability->Capabilities->StreamingCapabilities->NoRTSPStreaming;
				if(capability->Capabilities->StreamingCapabilities->RTP_USCORETCP)
					hasRtpOverTcp = *capability->Capabilities->StreamingCapabilities->RTP_USCORETCP;
				if(capability->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP)
					hasRtpOverRtspOverTcp = *capability->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP;
				if(capability->Capabilities->StreamingCapabilities->RTPMulticast)
					hasMulticast = *capability->Capabilities->StreamingCapabilities->RTPMulticast;
			}
		}
	} else {
		qInfo() << "Couldn't get media capabilities:" << capResponse;
	}

	DetailedResult<QList<MediaProfile>> result;
	Request<_trt__GetProfiles> profilesRequest;
	auto profilesResponse = mpMediaClient->GetProfiles(profilesRequest);
	if(profilesResponse) {
		for(auto *profile : profilesResponse.GetResultObject()->Profiles) {
			if(profile) {
				MediaProfile retProfile(getDeviceId());
				retProfile.setName(profile->Name);
				retProfile.setToken(profile->token);
				if(profile->fixed) {
					retProfile.setFixed(*profile->fixed);
				}
				retProfile.mSnapshotCapability = hasSnapshotCapability;

				if(hasSnapshotCapability) {
					Request<_trt__GetSnapshotUri> snapshotRequest;
					snapshotRequest.ProfileToken = profile->token;
					auto snapshotRespoinse = mpMediaClient->GetSnapshotUri(snapshotRequest);
					if(snapshotRespoinse) {
						if(const auto *snapshotResult = snapshotRespoinse.GetResultObject()) {
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
						if(const auto *streamResult = streamResponse.GetResultObject()) {
							if(streamResult->MediaUri) {
								StreamUrl streamUrl;
								streamUrl.mProtocol = StreamUrl::SP_UDP;
								streamUrl.mUrl = streamUrl.mUrlWithCredentials = QUrl::fromUserInput(streamResult->MediaUri->Uri);
								if(!mDeviceInfo.mUser.isEmpty()) streamUrl.mUrlWithCredentials.setUserName(mDeviceInfo.mUser);
								if(!mDeviceInfo.mPassword.isEmpty()) streamUrl.mUrlWithCredentials.setPassword(mDeviceInfo.mPassword);
								retProfile.mStreamUrls.push_back(streamUrl);
								// check for audio backchannel
								auto rtspClient = QScopedPointer<OnvifRtspClient>(new OnvifRtspClient(streamUrl.mUrlWithCredentials));
								if(auto hasBackchannelResult = rtspClient->hasAudioBackchannel()) {
									retProfile.mBackchennel = true;
									retProfile.mBackchennelUrl = streamUrl.mUrlWithCredentials;
									retProfile.mBackchennelMediaDescription = hasBackchannelResult.GetResultObject();
								}
							}
						}
					}

					if(hasMulticast) {
						streamRequest.StreamSetup->Stream = tt__StreamType::RTP_Multicast;
						streamRequest.StreamSetup->Transport->Protocol = tt__TransportProtocol::UDP;
						streamResponse = mpMediaClient->GetStreamUri(streamRequest);
						if(streamResponse) {
							if(const auto *streamResult = streamResponse.GetResultObject()) {
								if(streamResult->MediaUri) {
									StreamUrl streamUrl;
									streamUrl.mProtocol = StreamUrl::SP_UDP_MULTICAST;
									streamUrl.mUrl = streamUrl.mUrlWithCredentials = QUrl::fromUserInput(streamResult->MediaUri->Uri);
									if(!mDeviceInfo.mUser.isEmpty()) streamUrl.mUrlWithCredentials.setUserName(mDeviceInfo.mUser);
									if(!mDeviceInfo.mPassword.isEmpty()) streamUrl.mUrlWithCredentials.setPassword(mDeviceInfo.mPassword);
									retProfile.mStreamUrls.push_back(streamUrl);
								}
							}
						}
					}

					if(hasRtpOverTcp) {
						streamRequest.StreamSetup->Stream = tt__StreamType::RTP_Unicast;
						streamRequest.StreamSetup->Transport->Protocol = tt__TransportProtocol::TCP;
						streamResponse = mpMediaClient->GetStreamUri(streamRequest);
						if(streamResponse) {
							if(const auto *streamResult = streamResponse.GetResultObject()) {
								if(streamResult->MediaUri) {
									StreamUrl streamUrl;
									streamUrl.mProtocol = StreamUrl::SP_TCP;
									streamUrl.mUrl = streamUrl.mUrlWithCredentials = QUrl::fromUserInput(streamResult->MediaUri->Uri);
									if(!mDeviceInfo.mUser.isEmpty()) streamUrl.mUrlWithCredentials.setUserName(mDeviceInfo.mUser);
									if(!mDeviceInfo.mPassword.isEmpty()) streamUrl.mUrlWithCredentials.setPassword(mDeviceInfo.mPassword);
									retProfile.mStreamUrls.push_back(streamUrl);
								}
							}
						}
					}

					if(hasRtpOverRtspOverTcp) {
						streamRequest.StreamSetup->Stream = tt__StreamType::RTP_Unicast;
						streamRequest.StreamSetup->Transport->Protocol = tt__TransportProtocol::RTSP;
						streamResponse = mpMediaClient->GetStreamUri(streamRequest);
						if(streamResponse) {
							if(const auto *streamResult = streamResponse.GetResultObject()) {
								if(streamResult->MediaUri) {
									StreamUrl streamUrl;
									streamUrl.mProtocol = StreamUrl::SP_RTSP;
									streamUrl.mUrl = streamUrl.mUrlWithCredentials = QUrl::fromUserInput(streamResult->MediaUri->Uri);
									if(!mDeviceInfo.mUser.isEmpty()) streamUrl.mUrlWithCredentials.setUserName(mDeviceInfo.mUser);
									if(!mDeviceInfo.mPassword.isEmpty()) streamUrl.mUrlWithCredentials.setPassword(mDeviceInfo.mPassword);
									retProfile.mStreamUrls.push_back(streamUrl);
								}
							}
						}
					}

					if(hasRtpOverRtspOverHttpOverTcp) {
						streamRequest.StreamSetup->Stream = tt__StreamType::RTP_Unicast;
						streamRequest.StreamSetup->Transport->Protocol = tt__TransportProtocol::HTTP;
						streamResponse = mpMediaClient->GetStreamUri(streamRequest);
						if(streamResponse) {
							if(const auto *streamResult = streamResponse.GetResultObject()) {
								if(streamResult->MediaUri) {
									StreamUrl streamUrl;
									streamUrl.mProtocol = StreamUrl::SP_HTTP;
									streamUrl.mUrl = streamUrl.mUrlWithCredentials = QUrl::fromUserInput(streamResult->MediaUri->Uri);
									if(!mDeviceInfo.mUser.isEmpty()) streamUrl.mUrlWithCredentials.setUserName(mDeviceInfo.mUser);
									if(!mDeviceInfo.mPassword.isEmpty()) streamUrl.mUrlWithCredentials.setPassword(mDeviceInfo.mPassword);
									retProfile.mStreamUrls.push_back(streamUrl);
								}
							}
						}
					}
				}
				profiles.push_back(retProfile);
			}
		}

		static auto functor = [](StreamUrl::StreamProtocol protocol) {
			int ret = std::numeric_limits<int>::max();
			switch(protocol) {
				case StreamUrl::SP_HTTP:
					ret = 99;
					break;
				case StreamUrl::SP_TCP:
					ret = 2;
					break;
				case StreamUrl::SP_RTSP:
					ret = 1;
					break;
				case StreamUrl::SP_UDP:
					ret = 3;
					break;
				case StreamUrl::SP_UDP_MULTICAST:
					ret = 4;
					break;
				case StreamUrl::UNKNOWN:
				default:
					ret = std::numeric_limits<int>::max();
					break;
			}
			return ret;
		};

		for(auto &rProfile : profiles) {
			// Sort the stream urls by the most preferred protocol
			std::sort(rProfile.mStreamUrls.begin(), rProfile.mStreamUrls.end(),
			          [](StreamUrl &one, StreamUrl &two) { return functor(one.mProtocol) < functor(two.mProtocol); });
			break;
		}

		result.setResultObject(profiles);
	} else {
		qWarning() << "Couldn't get media profiles:" << profilesResponse;
		return DetailedResult<QList<MediaProfile>>::fromResponse(profilesResponse, QObject::tr("Couldn't load profiles"));
	}
	return result;
}

DetailedResult<QList<MediaProfile>> OnvifDevice::getMedia2Profiles() {

	if(!mDeviceInfo.mInitialized) {
		return DetailedResult<QList<MediaProfile>>(Result::FAULT, QObject::tr("The device is not initialized"));
	}

	if(!mDeviceInfo.mHasMediaService) {
		return DetailedResult<QList<MediaProfile>>(Result::FAULT, QObject::tr("The device has no media service"));
	}

	QList<MediaProfile> profiles;
	bool hasSnapshotCapability = false;
	bool hasStreaming = true;
	bool hasRtsp = false;
	bool hasRtpOverRtspOverTcp = false;
	bool hasRtpOverRtspOverHttpOverTcp = true; // TODO: No capability check - disable for now doesn't work with test devices
	bool hasMulticast = false;

	Request<_tr2__GetServiceCapabilities> capRequest;
	auto capResponse = mpMedia2Client->GetServiceCapabilities(capRequest);
	if(capResponse && capResponse.GetResultObject()) {
		const auto *capability = capResponse.GetResultObject();
		if(capability->Capabilities) {
			if(capability->Capabilities->SnapshotUri) hasSnapshotCapability = *capability->Capabilities->SnapshotUri;
			if(capability->Capabilities->StreamingCapabilities) {
				if(capability->Capabilities->StreamingCapabilities->RTSPStreaming)
					hasRtsp = *capability->Capabilities->StreamingCapabilities->RTSPStreaming;
				if(capability->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP)
					hasRtpOverRtspOverTcp = *capability->Capabilities->StreamingCapabilities->RTP_USCORERTSP_USCORETCP;
				if(capability->Capabilities->StreamingCapabilities->RTPMulticast)
					hasMulticast = *capability->Capabilities->StreamingCapabilities->RTPMulticast;
			}
		}
	} else {
		qInfo() << "Couldn't get media capabilities:" << capResponse;
	}

	DetailedResult<QList<MediaProfile>> result;
	Request<_tr2__GetProfiles> profilesRequest;
	profilesRequest.Type = {"All"};
	auto profilesResponse = mpMedia2Client->GetProfiles(profilesRequest);
	if(profilesResponse) {
		for(auto *profile : profilesResponse.GetResultObject()->Profiles) {
			if(profile) {
				MediaProfile retProfile(getDeviceId());
				retProfile.setName(profile->Name);
				retProfile.setToken(profile->token);
				if(profile->fixed) {
					retProfile.setFixed(*profile->fixed);
				}
				retProfile.mSnapshotCapability = hasSnapshotCapability;

				if(hasSnapshotCapability) {
					Request<_tr2__GetSnapshotUri> snapshotRequest;
					snapshotRequest.ProfileToken = profile->token;
					auto snapshotRespoinse = mpMedia2Client->GetSnapshotUri(snapshotRequest);
					if(snapshotRespoinse) {
						if(const auto *snapshotResult = snapshotRespoinse.GetResultObject()) {
							retProfile.mSnapshotUrl = QUrl::fromUserInput(snapshotResult->Uri);
						}
					}
				}

				if(hasStreaming) {
					Request<_tr2__GetStreamUri> streamRequest;
					streamRequest.ProfileToken = profile->token;
					streamRequest.Protocol = "RtspUnicast";
					auto streamResponse = mpMedia2Client->GetStreamUri(streamRequest);
					if(streamResponse) {
						if(const auto *streamResult = streamResponse.GetResultObject()) {
							StreamUrl streamUrl;
							streamUrl.mProtocol = StreamUrl::SP_UDP;
							streamUrl.mUrl = streamUrl.mUrlWithCredentials = QUrl::fromUserInput(streamResult->Uri);
							if(!mDeviceInfo.mUser.isEmpty()) streamUrl.mUrlWithCredentials.setUserName(mDeviceInfo.mUser);
							if(!mDeviceInfo.mPassword.isEmpty()) streamUrl.mUrlWithCredentials.setPassword(mDeviceInfo.mPassword);
							retProfile.mStreamUrls.push_back(streamUrl);
						}
					}

					Request<tr2__GetConfiguration> configRequest;
					configRequest.ProfileToken = new QString(profile->token);
					auto configResponse = mpMedia2Client->GetAudioDecoderConfigurationOptions(configRequest);
					if(configResponse) {
					}

					Request<tr2__GetConfiguration> configARequest;
					configRequest.ProfileToken = new QString(profile->token);
					auto configAResponse = mpMedia2Client->GetAudioOutputConfigurations(configARequest);
					if(configAResponse) {
					}

					/*
					          Request<_tr2__SetAudioOutputConfiguration> adsf;
					          adsf.Configuration = new tt__AudioOutputConfiguration();
					          adsf.Configuration->OutputLevel = 99;
					          adsf.Configuration->OutputToken = configAResponse.GetResultObject()->Configurations.front()->OutputToken;
					          adsf.Configuration->token = configAResponse.GetResultObject()->Configurations.front()->token;
					          auto öldskaadflsöadsfj = mpMedia2Client->SetAudioOutputConfiguration(adsf);
					          */

					Request<tr2__GetConfiguration> aslöflaskjdflaksjdf;
					auto afödsöafdäadsf = mpMedia2Client->GetAudioOutputConfigurationOptions(aslöflaskjdflaksjdf);

					// AudioDecoderConfiguration depends on AudioOutputConfiguration
					Request<tr2__GetConfiguration> configAsRequest;
					configAsRequest.ProfileToken = new QString(profile->token);
					auto configAsResponse = mpMedia2Client->GetAudioDecoderConfigurations(configARequest);
					if(configAsResponse) {
					}


					Request<_trt__GetAudioDecoderConfigurations> asdf;
					auto resasdffdsa = mpMediaClient->GetAudioDecoderConfigurations(asdf);

					/*
					auto audioToken = QString();
					for(auto res : resasdffdsa.GetResultObject()->Configurations) {
					  Request<_trt__GetAudioDecoderConfigurationOptions> adsffdsas;
					  adsffdsas.ConfigurationToken = new QString(res->token);
					  auto asfdlk = mpMediaClient->GetAudioDecoderConfigurationOptions(adsffdsas);
					  if(asfdlk && asfdlk.GetResultObject()->Options && asfdlk.GetResultObject()->Options->G711DecOptions) {
					    audioToken = res->token;
					  }
					}

					Request<_trt__AddAudioDecoderConfiguration> jlköfa;
					jlköfa.ProfileToken = profile->token;
					jlköfa.ConfigurationToken = audioToken;
					auto llmmmmm = mpMediaClient->AddAudioDecoderConfiguration(jlköfa);
*/

					// adsffdsas.ConfigurationToken = resasdffdsa.GetResultObject()->Configurations.front()->Name;
					// mpMediaClient->GetAudioDecoderConfigurationOptions();

					/* No use
					Request<_tr2__SetAudioDecoderConfiguration> reqss;
					reqss.Configuration = new tt__AudioDecoderConfiguration();
					reqss.Configuration->Name = "";
					mpMedia2Client->SetAudioDecoderConfiguration(reqss);
					 */

					auto configName = QString("BjoernsKonfig");
					Request<_tr2__AddConfiguration> req;
					req.ProfileToken = profile->token;

					_trt__AddAudioDecoderConfiguration cads;
					// cads.mpMediaClient->AddAudioDecoderConfiguration();

					_tr2__SetAudioDecoderConfiguration dec;
					// mpMedia2Client->SetAudioDecoderConfiguration();
					//  req.Configuration = new

					// mpMedia2Client->AddConfiguration();
				}
				profiles << retProfile;
			}
			break;
		}

		static auto functor = [](StreamUrl::StreamProtocol protocol) {
			int ret = std::numeric_limits<int>::max();
			switch(protocol) {
				case StreamUrl::SP_HTTP:
					ret = 99;
					break;
				case StreamUrl::SP_TCP:
					ret = 2;
					break;
				case StreamUrl::SP_RTSP:
					ret = 1;
					break;
				case StreamUrl::SP_UDP:
					ret = 3;
					break;
				case StreamUrl::SP_UDP_MULTICAST:
					ret = 4;
					break;
				case StreamUrl::UNKNOWN:
				default:
					ret = std::numeric_limits<int>::max();
					break;
			}
			return ret;
		};

		for(auto &rProfile : profiles) {
			// Sort the stream urls by the most preferred protocol
			std::sort(rProfile.mStreamUrls.begin(), rProfile.mStreamUrls.end(),
			          [](StreamUrl &one, StreamUrl &two) { return functor(one.mProtocol) < functor(two.mProtocol); });
			break;
		}

		result.setResultObject(profiles);
	} else {
		qWarning() << "Couldn't get media profiles:" << profilesResponse;
		return DetailedResult<QList<MediaProfile>>::fromResponse(profilesResponse, QObject::tr("Couldn't load profiles"));
	}

	return result;
}

QFuture<DetailedResult<QImage>> OnvifDevice::getSnapshot(const MediaProfile &rMediaProfile, const QSize &rSize /*= QSize()*/) {

	if(!mDeviceInfo.mInitialized) {
		auto d = AsyncFuture::deferred<DetailedResult<QImage>>();
		d.complete(DetailedResult<QImage>(Result::FAULT, QObject::tr("The device is not initialized")));
		return d.future();
	}

	if(!mDeviceInfo.mHasMediaService) {
		auto d = AsyncFuture::deferred<DetailedResult<QImage>>();
		d.complete(DetailedResult<QImage>(Result::FAULT, QObject::tr("The device has no media service")));
		return d.future();
	}

	MediaProfile profile;
	for(const auto &rProfile : mDeviceInfo.mMediaProfiles) {
		if(rProfile == rMediaProfile) {
			profile = rProfile;
			break;
		}
	}

	QFuture<DetailedResult<QImage>> future;
	if(profile.isValid()) {

		auto ctx = mpDeviceClient->GetCtx();
		QSharedPointer<HttpClient> httpClient(new HttpClient(ctx));
		QSharedPointer<QAtomicInt> isCanceled(new QAtomicInt(false));

		auto user = mDeviceInfo.mUser;
		auto password = mDeviceInfo.mPassword;
		auto forceSnapshotFromStream = false;

		future = QtConcurrent::run([profile, rSize, httpClient, isCanceled, user, password, forceSnapshotFromStream]() {
			return getSnapshotInternal(profile, rSize, httpClient, isCanceled, user, password, forceSnapshotFromStream);
		});

		auto observer = AsyncFuture::observe(future);
		observer.subscribe([isCanceled]() {},
		                   [httpClient, isCanceled]() {
			                   isCanceled->storeRelaxed(true);
			                   httpClient->CancelRequest();
		                   });

	} else {
		auto d = AsyncFuture::deferred<DetailedResult<QImage>>();
		d.complete(DetailedResult<QImage>(
		 Result::FAULT, QObject::tr("The device doesn't provide a media profile with name \"%1\"").arg(rMediaProfile.getToken())));
		future = d.future();
	}

	return future;
}
