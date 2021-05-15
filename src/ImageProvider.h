#pragma once
#include "ProfileId.h"
#include "Result.h"
#include <QQuickAsyncImageProvider>
#include <QObject>
#include <QQuickImageResponse>
#include <QRunnable>


class QQuickTextureFactory;

class ImageProvider : public QObject, public QQuickAsyncImageProvider {

	Q_OBJECT

 public:
	explicit ImageProvider(QObject *pParent = nullptr);
	QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

 private:
	Q_DISABLE_COPY(ImageProvider)
};


class AsyncImageResponse : public QQuickImageResponse, public QRunnable {

 public:
	AsyncImageResponse(const ProfileId &rProfileId, const QSize &requestedSize);
	QQuickTextureFactory *textureFactory() const override;
	void run() override;
	QString errorString() const override;

 private:
	ProfileId mProfileId;
	QSize mSize;
	QImage mImage;
	Result mResult;
};