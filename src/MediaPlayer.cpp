#include "MediaPlayer.h"
#include "App.h"
#include "asyncfuture.h"
#include "mdk/Player.h"
#include <QDebug>
#include <QTimer>
#include <QUrlQuery>

class VideoRendererInternal : public QQuickFramebufferObject::Renderer {
 public:
	explicit VideoRendererInternal(MediaPlayer *player) : mpPlayer(player) {}

	void render() override { mpPlayer->renderVideo(); }

	QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
		mpPlayer->setVideoSurfaceSize(size.width(), size.height());
		return new QOpenGLFramebufferObject(size);
	}

	MediaPlayer *mpPlayer;
};


MediaPlayer::MediaPlayer(QQuickItem *parent) :
 QQuickFramebufferObject(parent),
 internal_player(new mdk::Player()),
 mVideoSize(0, 0),
 mVideoDisabled(false),
 mAudioDisabled(false),
 mVolume(1.0),
 mState(Stopped),
 mFillMode(Enums::FillMode::Stretch) {

	setMirrorVertically(true);
	// internal_player->setAudioBackends({""});
	internal_player->setAspectRatio(mdk::KeepAspectRatio);
	internal_player->setProperty("avio.user_agent", App::getDefaultUserAgent().toStdString());
	internal_player->setProperty("avformat.fflags", "+nobuffer");
	internal_player->setProperty("avformat.fflags", "+discardcorrupt");
	internal_player->setProperty("avformat.avioflags", "direct");
	internal_player->setProperty("avformat.flags", "+low_delay");
	internal_player->setProperty("avformat.max_probe_packets", "0");
	internal_player->setProperty("avformat.analyzeduration", "1");
	internal_player->setProperty("avformat.probesize", "32");
	internal_player->setProperty("avformat.fpsprobesize", "0");
	internal_player->setProperty("avformat.max_delay", "0");
	internal_player->setBufferRange(0);

	internal_player->onEvent([this](const mdk::MediaEvent &event) {
		if(event.category == "video" && event.detail == "size") {
			mVideoSize = QSize(event.video.width, event.video.height);
			emit videoSizeChanged(mVideoSize);
		} else if(event.category == "render.video" && event.detail == "1st_frame") {
			emit firstFrame();
		}
		return true;
	});

	internal_player->onStateChanged([this](mdk::State state) {
		switch(state) {
			case mdk::State::Stopped:
				mState = Stopped;
				break;
			case mdk::State::Playing:
				mState = Playing;
				break;
			case mdk::State::Paused:
				mState = Paused;
				break;
		}
		emit stateChanged(mState);
	});
}

MediaPlayer::~MediaPlayer() {
	delete internal_player;
}


QString MediaPlayer::source() {
	return m_source;
}

void MediaPlayer::setSource(const QString &source) {
	if(!source.isEmpty()) {
		internal_player->setMedia(qPrintable(source));
		m_source = source;
		emit sourceChanged();
		play();
	}
}

QQuickFramebufferObject::Renderer *MediaPlayer::createRenderer() const {
	return new VideoRendererInternal(const_cast<MediaPlayer *>(this));
}

MediaPlayer::PlaybackState MediaPlayer::getState() const {
	return mState;
}

void MediaPlayer::play() {
	internal_player->set(mdk::PlaybackState::Playing);
	internal_player->setRenderCallback([this](void *) { QMetaObject::invokeMethod(this, "update"); });
}

void MediaPlayer::setPlaybackRate(float rate) {
	internal_player->setPlaybackRate(rate);
}

void MediaPlayer::setVideoSurfaceSize(int width, int height) {
	internal_player->setVideoSurfaceSize(width, height);
}

int MediaPlayer::getVideoWidth() const {
	return mVideoSize.width();
}

int MediaPlayer::getVideoHeight() const {
	return mVideoSize.height();
}

void MediaPlayer::setDisableVideo(bool disabled) {
	mVideoDisabled = disabled;
}

bool MediaPlayer::getDisableVideo() const {
	return mVideoDisabled;
}

void MediaPlayer::setDisableAudio(bool disabled) {
	mAudioDisabled = disabled;
}

bool MediaPlayer::getDisableAudio() const {
	return mAudioDisabled;
}

qreal MediaPlayer::getVolume() const {

	return mVolume;
}

void MediaPlayer::setVolume(qreal volume) {

	mVolume = volume;
	internal_player->setVolume(static_cast<float>(volume));
	emit volumeChanged();
}

Enums::FillMode MediaPlayer::getFillMode() const {

	return mFillMode;
}

void MediaPlayer::setFillMode(Enums::FillMode fillMode) {

	switch(fillMode) {
		case Enums::FillMode::Stretch:
			internal_player->setAspectRatio(mdk::IgnoreAspectRatio);
			break;
		case Enums::FillMode::PreserveAspectFit:
			internal_player->setAspectRatio(mdk::KeepAspectRatio);
			break;
		case Enums::FillMode::PreserveAspectCrop:
			internal_player->setAspectRatio(mdk::KeepAspectRatioCrop);
			break;
	}
	mFillMode = fillMode;
	emit fillModeChanged();
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
		if(!v || v.timestamp() == mdk::TimestampEOS) {
			// AOT frame(1st frame, seek end 1st frame) is not valid, but format is valid.
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
