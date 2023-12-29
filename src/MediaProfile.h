#pragma once
#include "ProfileId.h"
#include "Result.h"
#include "SessionDescription.h"
#include <QMetaType>
#include <QString>
#include <QUrl>
#include <QUuid>


class StreamUrl {

	Q_GADGET
	Q_PROPERTY(QUrl url MEMBER mUrl CONSTANT)
	Q_PROPERTY(QUrl urlWithCredentials MEMBER mUrlWithCredentials CONSTANT)
	Q_PROPERTY(StreamProtocol protocol MEMBER mProtocol CONSTANT)

 public:
	enum StreamProtocol { UNKNOWN = 0, SP_UDP, SP_UDP_MULTICAST, SP_TCP, SP_RTSP, SP_HTTP };
	Q_ENUM(StreamProtocol);

	QUrl mUrl;
	QUrl mUrlWithCredentials;
	StreamProtocol mProtocol;
};

Q_DECLARE_METATYPE(QList<StreamUrl>)

class MediaProfile {

	Q_GADGET
	Q_PROPERTY(bool fixed READ getFixed CONSTANT)
	Q_PROPERTY(bool hasSnapshotCapability MEMBER mSnapshotCapability CONSTANT)
	Q_PROPERTY(QUrl snapshotUrl MEMBER mSnapshotUrl CONSTANT)
	Q_PROPERTY(bool hasBackchannel MEMBER mBackchennel CONSTANT)
	Q_PROPERTY(QUrl backchannelUrl MEMBER mBackchennelUrl CONSTANT)
	Q_PROPERTY(MediaDescription mediaDescription MEMBER mBackchennelMediaDescription CONSTANT)
	Q_PROPERTY(QList<StreamUrl> streamUrls MEMBER mStreamUrls CONSTANT)
	Q_PROPERTY(QString name READ getName)
	Q_PROPERTY(QString profileToken READ getToken)
	Q_PROPERTY(ProfileId profileId READ getProfileId)

 public:
	explicit MediaProfile(const QUuid &rOwnerDeviceId = QUuid());
	QString getName() const;
	void setName(const QString &rName);
	QString getToken() const;
	void setToken(const QString &rToken);
	QUuid getDeviceId() const;
	ProfileId getProfileId() const;
	bool getFixed() const;
	void setFixed(bool fixed);

	bool isValid() const;
	bool operator==(const MediaProfile &rOther) const;
	bool operator!=(const MediaProfile &rOther) const;

	MediaDescription mBackchennelMediaDescription = {};
	bool mBackchennel = false;
	QUrl mBackchennelUrl = {};
	bool mSnapshotCapability = false;
	QUrl mSnapshotUrl = {};
	QList<StreamUrl> mStreamUrls = {};

 private:
	QUuid mDeviceId;
	QString mName;
	QString mToken;
	bool mFixed;
};

Q_DECLARE_METATYPE(QList<MediaProfile>)
Q_DECLARE_METATYPE(DetailedResult<QList<MediaProfile>>)
Q_DECLARE_METATYPE(QFuture<DetailedResult<QList<MediaProfile>>>)
