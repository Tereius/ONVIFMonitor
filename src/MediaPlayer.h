#pragma once
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
	Q_PROPERTY(int videoWidth READ getVideoWidth NOTIFY videoSizeChanged)
	Q_PROPERTY(int videoHeight READ getVideoHeight NOTIFY videoSizeChanged)
	Q_PROPERTY(int disableVideo READ getDisableVideo WRITE setDisableVideo)
	Q_PROPERTY(int disableAudio READ getDisableAudio WRITE setDisableAudio)

 public:
	explicit MediaPlayer(QQuickItem *parent = nullptr);
	virtual ~MediaPlayer();
	Renderer *createRenderer() const;

	Q_INVOKABLE QString source();
	Q_INVOKABLE void setSource(const QString &s);
	Q_INVOKABLE void play();
	Q_INVOKABLE void setPlaybackRate(float rate);
	Q_INVOKABLE void setVideoSurfaceSize(int width, int height);
	int getVideoWidth() const;
	int getVideoHeight() const;
	void setDisableVideo(bool disabled);
	bool getDisableVideo() const;
	void setDisableAudio(bool disabled);
	bool getDisableAudio() const;

	void renderVideo();

	static DetailedResult<QImage> getSnapshot(const QUrl &rUrl, const QSize &rSize = {});
	static DetailedResult<QImage> getSnapshot(const StreamUrl &rStreamUrl, const QSize &rSize = {});

 signals:
	void sourceChanged();
	void videoSizeChanged(QSize);

 private:
	QString m_source;
	mdk::Player *internal_player;
	QSize mVideoSize;
	bool mVideoDisabled;
	bool mAudioDisabled;
};
