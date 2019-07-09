#include "ImageProvider.h"
#include "DeviceManager.h"
#include "Device.h"
#include <QThreadPool>
#include <QCoreApplication>


ImageProvider::ImageProvider(QObject *pParent /*= nullptr*/) :
	QObject(pParent),
	QQuickAsyncImageProvider() {

}

QQuickImageResponse* ImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize) {

	auto paramList = id.split('/');
	auto profile = ProfileId();
	if(paramList.size() > 1) {
		profile = ProfileId(paramList[0], paramList[1]);
	}
	AsyncImageResponse *response = new AsyncImageResponse(profile, requestedSize);
	QThreadPool::globalInstance()->start(response);
	return response;
}

AsyncImageResponse::AsyncImageResponse(const ProfileId &rProfileId, const QSize &requestedSize) :
	QQuickImageResponse(),
	QRunnable(),
	mProfileId(rProfileId),
	mSize(requestedSize),
	mImage(),
	mResult(),
	mpExtractor(new QtAV::VideoFrameExtractor()) {

	setAutoDelete(false);

	mpExtractor->setAsync(false);
	mpExtractor->setAutoExtract(false);
	connect(mpExtractor, &QtAV::VideoFrameExtractor::frameExtracted, this, [this](const QtAV::VideoFrame& frame) {

		mImage = frame.toImage(QImage::Format_RGB32, mSize);
		emit finished();
	}, Qt::DirectConnection);

	connect(mpExtractor, &QtAV::VideoFrameExtractor::error, this, [this]() {

		mResult = Result(Result::FAULT, "Couldn't extract image from stream");
		emit finished();
	}, Qt::DirectConnection);
}

AsyncImageResponse::~AsyncImageResponse() {

	delete mpExtractor;
}

QQuickTextureFactory* AsyncImageResponse::textureFactory() const {

	return QQuickTextureFactory::textureFactoryForImage(mImage);
}

void AsyncImageResponse::run() {

	if(!mProfileId.isNull()) {
		auto device = DeviceM->getDevice(mProfileId.getDeviceId());
		if(device) {
			auto streamUrl = device->getStreamUrl(mProfileId.getProfileToken());
			if(streamUrl.isValid()) {
				mpExtractor->setSource(streamUrl.toString());
				mpExtractor->setPosition(0);
				mpExtractor->extract();
			}
			else {
				mResult = Result(Result::FAULT, "Invalid stream url");
				emit finished();
			}
		}
		else {
			mResult = Result(Result::FAULT, "Couldn't get the device");
			emit finished();
		}
	}
	else {
		mResult = Result(Result::FAULT, "The profile id is invalid");
		emit finished();
	}
}

QString AsyncImageResponse::errorString() const {

	if(!mResult) {
		return mResult.toString();
	}
	return QString();
}
