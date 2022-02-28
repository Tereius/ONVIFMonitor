#include "MediaPlayer.h"
#include "App.h"
#include "asyncfuture.h"
#include "mdk/Player.h"
#include <QDebug>
#include <QTimer>
#include <QUrlQuery>

class VideoRendererInternal : public QQuickFramebufferObject::Renderer {
 public:
	VideoRendererInternal(MediaPlayer *r) { this->r = r; }

	void render() override { r->renderVideo(); }

	QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
		r->setVideoSurfaceSize(size.width(), size.height());
		return new QOpenGLFramebufferObject(size);
	}

	MediaPlayer *r;
};


MediaPlayer::MediaPlayer(QQuickItem *parent) : QQuickFramebufferObject(parent), internal_player(new mdk::Player()) {
	setMirrorVertically(true);
	// internal_player->setAudioBackends({""});
}

MediaPlayer::~MediaPlayer() {
	delete internal_player;
}


QString MediaPlayer::source() {
	return m_source;
}

void MediaPlayer::setSource(const QString &s) {

	if(!s.isEmpty()) {
		internal_player->setMedia(s.toUtf8().data());
		m_source = s;
		emit sourceChanged();
		play();
	}
}

QQuickFramebufferObject::Renderer *MediaPlayer::createRenderer() const {
	return new VideoRendererInternal(const_cast<MediaPlayer *>(this));
}

void MediaPlayer::play() {
	internal_player->set(mdk::PlaybackState::Playing);
	internal_player->setRenderCallback([=](void *) { QMetaObject::invokeMethod(this, "update"); });
}

void MediaPlayer::setPlaybackRate(float rate) {
	internal_player->setPlaybackRate(rate);
}

void MediaPlayer::setVideoSurfaceSize(int width, int height) {
	internal_player->setVideoSurfaceSize(width, height);
}

void MediaPlayer::renderVideo() {
	internal_player->renderVideo();
}

DetailedResult<QImage> MediaPlayer::getSnapshot(const QUrl &rUrl, const QSize &rSize) {

	const int timeoutMs = 30000;

	auto streamUrl = rUrl;
	auto query = QUrlQuery(streamUrl);
	query.addQueryItem("mdkopt", "avformat");
	query.addQueryItem("timeout", QString::number(timeoutMs * 1000));
	query.addQueryItem("user_agent", App::getDefaultUserAgent());
	streamUrl.setQuery(query);

	QScopedPointer<mdk::Player> player(new mdk::Player());
	player->setDecoders(mdk::MediaType::Audio, {});
	player->setDecoders(mdk::MediaType::Subtitle, {});
	player->setAudioBackends({""});
	player->setVolume(0);
	player->setMedia(qPrintable(streamUrl.toString()));
	player->onSync([] { return DBL_MAX; });

	bool decoded = false;
	auto deferred = QSharedPointer<AsyncFuture::Deferred<DetailedResult<QImage>>>::create();

	player->onFrame<mdk::VideoFrame>([&decoded, deferred, rSize](mdk::VideoFrame &v, int) {
		if(decoded) return 0;
		decoded = true;
		if(!v || v.timestamp() == mdk::TimestampEOS) { // AOT frame(1st frame, seek end 1st frame) is not valid, but format is valid.
			                                             // eof frame format is invalid
			deferred->complete(DetailedResult<QImage>(Result::FAULT, QObject::tr("Could not get valid snapshot frame")));
			return 0;
		}
		if(!v.format()) {
			deferred->complete(DetailedResult<QImage>(Result::FAULT, QObject::tr("Could not get valid snapshot frame")));
			return 0;
		}
		auto rgbFrame = v.to(mdk::PixelFormat::RGB24); // TODO: Bug? Scaling doesn't work
		if(rgbFrame && rgbFrame.planeCount() == 1 && rgbFrame.width() > 0 && rgbFrame.height() > 0) {
			auto *rawFrame = rgbFrame.detach();
			auto image = QImage(
			 MDK_CALL(rawFrame, bufferData, 0), MDK_CALL(rawFrame, width, 0), MDK_CALL(rawFrame, height, 0), QImage::Format_RGB888,
			 [](void *info) {
				 if(info) {
					 auto *rawFrame = static_cast<mdkVideoFrameAPI *>(info);
					 mdkVideoFrameAPI_delete(&rawFrame);
				 }
			 },
			 rawFrame);
			if(rSize.isValid()) {
				image = image.scaled(rSize, Qt::KeepAspectRatio);
			}
			deferred->complete(DetailedResult<QImage>(image));
		} else {
			deferred->complete(DetailedResult<QImage>(Result::FAULT, QObject::tr("Could not get valid snapshot frame")));
		}
		return 0;
	});

	player->prepare(0, [deferred](int64_t pos, bool *) {
		if(pos < 0) {
			deferred->complete(DetailedResult<QImage>(Result::FAULT, QObject::tr("Seek snapshot frame failed")));
		}
		return true;
	});

	QTimer::singleShot(timeoutMs, qApp, [deferred]() { deferred->cancel(); });

	player->set(mdk::State::Running);
	auto future = deferred->future();
	future.waitForFinished();
	player->set(mdk::State::Stopped);
	if(future.isFinished() && future.resultCount() > 0) {
		return future.result();
	}
	return {Result::FAULT, QObject::tr("Snapshot was canceled")};
}

DetailedResult<QImage> MediaPlayer::getSnapshot(const StreamUrl &rStreamUrl, const QSize &rSize /*= {}*/) {

	auto streamUrl = rStreamUrl;
	auto query = QUrlQuery(streamUrl.mUrlWithCredentials);
	query.addQueryItem("mdkopt", "avformat");
	switch(streamUrl.mProtocol) {
		case StreamUrl::SP_HTTP:
			if(streamUrl.mUrlWithCredentials.scheme().compare("https") == 0) {
				query.addQueryItem("rtsp_transport", "https");
			} else {
				query.addQueryItem("rtsp_transport", "http");
			}
			break;
		case StreamUrl::SP_TCP:
		case StreamUrl::SP_RTSP:
			query.addQueryItem("rtsp_transport", "tcp");
			break;
		case StreamUrl::SP_UDP:
			query.addQueryItem("rtsp_transport", "udp");
			break;
		case StreamUrl::SP_UDP_MULTICAST:
			query.addQueryItem("rtsp_transport", "udp_multicast");
			break;
		case StreamUrl::UNKNOWN:
		default:
			break;
	}
	streamUrl.mUrlWithCredentials.setQuery(query);
	return getSnapshot(streamUrl.mUrlWithCredentials, rSize);
}
