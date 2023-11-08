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

 public:
	explicit MediaPlayer(QQuickItem *parent = nullptr);
	virtual ~MediaPlayer();
	Renderer *createRenderer() const;

	Q_INVOKABLE QString source();
	Q_INVOKABLE void setSource(const QString &s);
	Q_INVOKABLE void play();
	Q_INVOKABLE void setPlaybackRate(float rate);
	Q_INVOKABLE void setVideoSurfaceSize(int width, int height);

	void renderVideo();

	static DetailedResult<QImage> getSnapshot(const QUrl &rUrl, const QSize &rSize = {});
	static DetailedResult<QImage> getSnapshot(const StreamUrl &rStreamUrl, const QSize &rSize = {});

 signals:
	void sourceChanged();

 private:
	QString m_source;
	mdk::Player *internal_player;
};
