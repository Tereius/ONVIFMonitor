#pragma once
#include "ProfileId.h"
#include "Result.h"
#include <QQuickAsyncImageProvider>
#include <QObject>
#include <QQuickImageResponse>
#include <QRunnable>
#include <QtAV/VideoFrameExtractor.h>


class QQuickTextureFactory;

class ImageProvider : public QObject, public QQuickAsyncImageProvider {

	Q_OBJECT

public:
	ImageProvider(QObject *pParent = nullptr);

	virtual QQuickImageResponse* requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:
	Q_DISABLE_COPY(ImageProvider)

};


class AsyncImageResponse : public QQuickImageResponse, public QRunnable {

public:
	AsyncImageResponse(const ProfileId &rProfileId, const QSize &requestedSize);
	virtual ~AsyncImageResponse();
	QQuickTextureFactory* textureFactory() const;
	void run();
	virtual QString errorString() const override;

private:

	ProfileId mProfileId;
	QSize mSize;
	QImage mImage;
	Result mResult;
	QtAV::VideoFrameExtractor *mpExtractor;
};