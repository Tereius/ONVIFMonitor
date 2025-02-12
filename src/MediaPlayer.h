#pragma once
#include "Enums.h"
#include "MediaProfile.h"
#include "Result.h"
#include <QDebug>
#include <QImage>
#include <QOpenGLFramebufferObject>
#include <QQuickFramebufferObject>
#include <QSize>
#include <QtQmlIntegration>


namespace mdk {
class Player;
}

class MediaPlayer : public QQuickFramebufferObject {

	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
	Q_PROPERTY(PlaybackState state READ getState NOTIFY stateChanged)
	Q_PROPERTY(qreal volume READ getVolume WRITE setVolume NOTIFY volumeChanged)
	Q_PROPERTY(Enums::FillMode fillMode READ getFillMode WRITE setFillMode)
	Q_PROPERTY(int videoWidth READ getVideoWidth NOTIFY videoSizeChanged)
	Q_PROPERTY(int videoHeight READ getVideoHeight NOTIFY videoSizeChanged)
	Q_PROPERTY(int disableVideo READ getDisableVideo WRITE setDisableVideo)
	Q_PROPERTY(int disableAudio READ getDisableAudio WRITE setDisableAudio)

 public:
	enum PlaybackState {
		Stopped,
		Playing,
		Paused,
	};

	Q_ENUM(PlaybackState)

	explicit MediaPlayer(QQuickItem *parent = nullptr);
	~MediaPlayer() override;
	Renderer *createRenderer() const;

	Q_INVOKABLE QString source();
	Q_INVOKABLE void setSource(const QString &source);
	Q_INVOKABLE void play();
	Q_INVOKABLE void setPlaybackRate(float rate);
	Q_INVOKABLE void setVideoSurfaceSize(int width, int height);
	PlaybackState getState() const;
	int getVideoWidth() const;
	int getVideoHeight() const;
	void setDisableVideo(bool disabled);
	bool getDisableVideo() const;
	void setDisableAudio(bool disabled);
	bool getDisableAudio() const;
	qreal getVolume() const;
	void setVolume(qreal volume);
	Enums::FillMode getFillMode() const;
	void setFillMode(Enums::FillMode fillMode);
	void renderVideo();

	static DetailedResult<QImage> getSnapshot(const QUrl &rUrl, const QSize &rSize = {});
	static DetailedResult<QImage> getSnapshot(const StreamUrl &rStreamUrl, const QSize &rSize = {});

 signals:
	void sourceChanged();
	void videoSizeChanged(QSize);
	void stateChanged(MediaPlayer::PlaybackState);
	void firstFrame(); // emitted when the first video frame is rendered
	void mirrorHorizontalChanged();
	void mirrorVerticalChanged();
	void volumeChanged();
	void fillModeChanged();

 private:
	QString m_source;
	mdk::Player *internal_player;
	QSize mVideoSize;
	bool mVideoDisabled;
	bool mAudioDisabled;
	qreal mVolume;
	MediaPlayer::PlaybackState mState;
	Enums::FillMode mFillMode;
};
