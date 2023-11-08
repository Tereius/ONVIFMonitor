#include "ImageProvider.h"
#include "AbstractDevice.h"
#include "AsyncFuture/asyncfuture.h"
#include "DeviceManager.h"
#include <QThreadPool>
#include <QtConcurrent>
#include <utility>


ImageProvider::ImageProvider(DeviceManager *pManager) : mpManager(pManager), QQuickAsyncImageProvider() {

	Q_ASSERT(mpManager);
}

QQuickImageResponse *ImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize) {

	auto paramList = QUrl::fromPercentEncoding(id.toLatin1()).split('/');
	auto profile = ProfileId();
	if(paramList.size() > 1) {
		profile = ProfileId(QUuid::fromString(paramList[0]), paramList[1]);
	}
	auto response = new AsyncImageResponse(profile, requestedSize, mpManager);
	return response;
}

AsyncImageResponse::AsyncImageResponse(ProfileId rProfileId, const QSize &requestedSize, QPointer<DeviceManager> pManager) :
 QQuickImageResponse(),
 mProfileId(std::move(rProfileId)),
 mSize(requestedSize),
 mImage(),
 mResult(),
 mpManager(std::move(pManager)),
 mSnapshotFuture() {

	Q_ASSERT(mpManager);

	if(mpManager) {
		mSnapshotFuture = mpManager->getSnapshot(mProfileId.getDeviceId(), mProfileId.getProfileToken(), mSize);

		auto observer = AsyncFuture::observe(mSnapshotFuture);
		observer.subscribe(
		 [this](DetailedResult<QImage> result) {
			 if(result) {
				 mImage = result.GetResultObject();
			 } else {
				 mResult = result;
			 }
			 emit finished();
		 },
		 [this]() {
			 mResult = Result::FAULT;
			 emit finished();
		 });
	} else {
		emit finished();
	}
}

QQuickTextureFactory *AsyncImageResponse::textureFactory() const {

	return QQuickTextureFactory::textureFactoryForImage(mImage);
}

QString AsyncImageResponse::errorString() const {

	if(!mResult) {
		return mResult.toString();
	}
	return {};
}

void AsyncImageResponse::cancel() {

	qWarning() << "cancel Thread:" << QThread::currentThread();
	mSnapshotFuture.cancel();
}
