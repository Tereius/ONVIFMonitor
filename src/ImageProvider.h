#pragma once
#include "ProfileId.h"
#include "Result.h"
#include <QFuture>
#include <QObject>
#include <QPointer>
#include <QQuickAsyncImageProvider>
#include <QQuickImageResponse>


class QQuickTextureFactory;
class DeviceManager;

class ImageProvider : public QQuickAsyncImageProvider {

 public:
	explicit ImageProvider(DeviceManager *pManager);
	QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

 private:
	Q_DISABLE_COPY(ImageProvider)

	QPointer<DeviceManager> mpManager;
};


class AsyncImageResponse : public QQuickImageResponse {

	Q_OBJECT

 public:
	AsyncImageResponse(ProfileId rProfileId, const QSize &requestedSize, QPointer<DeviceManager> pManager);
	QQuickTextureFactory *textureFactory() const override;
	QString errorString() const override;

 public slots:
	void cancel() override;

 private:
	ProfileId mProfileId;
	QSize mSize;
	QImage mImage;
	Result mResult;
	QPointer<DeviceManager> mpManager;
	QFuture<DetailedResult<QImage>> mSnapshotFuture;
};