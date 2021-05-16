#include "ImageProvider.h"
#include "DeviceManager.h"
#include "AsyncFuture/asyncfuture.h"
#include <QThreadPool>
#include <QCoreApplication>
#include <QtAV/VideoFrameExtractor.h>


ImageProvider::ImageProvider(QObject *pParent /*= nullptr*/) : QObject(pParent), QQuickAsyncImageProvider() {}

QQuickImageResponse *ImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize) {

	auto paramList = QUrl::fromPercentEncoding(id.toLatin1()).split('/');
	auto profile = ProfileId();
	if(paramList.size() > 1) {
		profile = ProfileId(QUuid::fromString(paramList[0]), paramList[1]);
	}
	auto response = new AsyncImageResponse(profile, requestedSize);
	QThreadPool::globalInstance()->start(response);
	return response;
}

AsyncImageResponse::AsyncImageResponse(const ProfileId &rProfileId, const QSize &requestedSize) :
 QQuickImageResponse(), QRunnable(), mProfileId(rProfileId), mSize(requestedSize), mImage(), mResult() {

	setAutoDelete(false);
}

QQuickTextureFactory *AsyncImageResponse::textureFactory() const {

	return QQuickTextureFactory::textureFactoryForImage(mImage);
}

void AsyncImageResponse::run() {

	if(!mProfileId.isNull()) {
		auto futureSnapshot = DeviceM->getSnapshot(mProfileId.getDeviceId(), mProfileId.getProfileToken());
		futureSnapshot.waitForFinished();
		auto snapshotResult = futureSnapshot.result();
		if(snapshotResult && !snapshotResult.GetResultObject().isNull()) {
			mImage = snapshotResult.GetResultObject();
			if(mSize.isValid()) {
				mImage = mImage.scaled(mSize, Qt::KeepAspectRatio);
			}
		} else {
			auto deviceInfo = DeviceM->getDeviceInfo(mProfileId.getDeviceId());
			MediaProfile profile;
			for(const auto &rProfile : deviceInfo.mMediaProfiles) {
				if(rProfile.getToken().compare(mProfileId.getProfileToken()) == 0) {
					profile = rProfile;
					break;
				}
			}
			if(profile.isValid() && !profile.mStreamUrls.isEmpty()) {
				auto streamUrl = profile.mStreamUrls.first();
				for(const auto &url : profile.mStreamUrls) {
					// prefer TCP over UDP
					if(url.mProtocol == StreamUrl::SP_HTTP || url.mProtocol == StreamUrl::SP_RTSP || url.mProtocol == StreamUrl::SP_TCP) {
						streamUrl = url;
						break;
					}
				}
				streamUrl.mUrl.setUserName(deviceInfo.mUser);
				streamUrl.mUrl.setPassword(deviceInfo.mPassword);

				auto mpExtractor = new QtAV::VideoFrameExtractor();
				mpExtractor->setAsync(false);
				mpExtractor->setAutoExtract(false);
				mpExtractor->setPosition(0);
				mpExtractor->setSource(streamUrl.mUrl.toString());

				connect(
				 mpExtractor, &QtAV::VideoFrameExtractor::frameExtracted, this,
				 [this](const QtAV::VideoFrame &frame) {
					 mImage = frame.toImage(QImage::Format_ARGB32);
					 if(mSize.isValid()) {
						 mImage = mImage.scaled(mSize, Qt::KeepAspectRatio);
					 }
				 },
				 Qt::DirectConnection);

				connect(
				 mpExtractor, &QtAV::VideoFrameExtractor::error, this,
				 [this](const QString &errorMessage) { mResult = Result(Result::FAULT, "Couldn't extract image from stream: " + errorMessage); },
				 Qt::DirectConnection);

				mpExtractor->extract();
				mpExtractor->deleteLater();
			} else {
				mResult = Result(Result::FAULT, "The device doesn't provide a stream url");
			}
		}
	} else {
		mResult = Result(Result::FAULT, "The profile id is invalid");
	}
	emit finished();
}

QString AsyncImageResponse::errorString() const {

	if(!mResult) {
		return mResult.toString();
	}
	return QString();
}
