#include "GenericDevice.h"
#include "MediaPlayer.h"
#include "asyncfuture.h"
#include <QCryptographicHash>
#include <QtConcurrent>

GenericDevice::GenericDevice() : AbstractDevice(), mDeviceInfo() {}

GenericDevice::~GenericDevice() {}

Result GenericDevice::initDevice(const QUrl &rEndpoint, const QString &rUser, const QString &rPassword) {

	mDeviceInfo = getDeviceInfo();
	mDeviceInfo.mUser = rUser;
	mDeviceInfo.mPassword = rPassword;
	mDeviceInfo.mDeviceName = "Generic Device";
	mDeviceInfo.mEndpoint = rEndpoint;
	mDeviceInfo.mInitialized = true;
	mDeviceInfo.mSerialNumber =
	 QUuid::fromRfc4122(QCryptographicHash::hash(rEndpoint.toString().toUtf8(), QCryptographicHash::Md5)).toString();
	mDeviceInfo.mHasMediaService = true;
	mDeviceInfo.mEndpointReference = getDeviceId();

	MediaProfile profile(getDeviceId());
	profile.setFixed(true);
	profile.setName("Generic profile");
	profile.setToken("Generic");
	profile.mSnapshotCapability = false;

	StreamUrl streamUrl;
	streamUrl.mUrl = rEndpoint;
	auto urlWithCredentials = QUrl(rEndpoint);
	if(!rUser.isEmpty()) urlWithCredentials.setUserName(rUser);
	if(!rPassword.isEmpty()) urlWithCredentials.setPassword(rPassword);
	streamUrl.mUrlWithCredentials = urlWithCredentials;
	streamUrl.mProtocol = StreamUrl::UNKNOWN;
	profile.mStreamUrls.append(streamUrl);
	mDeviceInfo.mMediaProfiles.append({profile});

	return !rEndpoint.isEmpty() && rEndpoint.isValid() ? Result::OK : Result::FAULT;
}

QUuid GenericDevice::getDeviceId() const {

	return QUuid::fromString(mDeviceInfo.mSerialNumber);
}

DeviceInfo GenericDevice::getDeviceInfo() const {

	return mDeviceInfo;
}

DetailedResult<QList<MediaProfile>> GenericDevice::getMediaProfiles() {

	return DetailedResult<QList<MediaProfile>>(mDeviceInfo.mMediaProfiles);
}

QFuture<DetailedResult<QImage>> GenericDevice::getSnapshot(const MediaProfile &rMediaProfile, const QSize &rSize /*= QSize()*/) {

	return QtConcurrent::run([rMediaProfile, rSize]() {
		DetailedResult<QImage> result;
		if(!rMediaProfile.mStreamUrls.isEmpty()) {
			for(auto streamUrl : rMediaProfile.mStreamUrls) {
				result = MediaPlayer::getSnapshot(streamUrl);
				if(result) {
					break;
				}
			}
		} else {
			result = DetailedResult<QImage>(Result::FAULT,
			                                QObject::tr("The device doesn't provide any stream urls \"%1\"").arg(rMediaProfile.getToken()));
		}
		return result;
	});
}
