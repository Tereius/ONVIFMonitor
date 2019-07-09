#pragma once
#include <QUrl>
#include <QMetaType>


class MediaService {

	Q_GADGET
		Q_PROPERTY(bool initialized READ isInitialized CONSTANT)
		Q_PROPERTY(bool snapshotUri READ getSnapshotUri CONSTANT)
		Q_PROPERTY(bool rotation READ getRotation CONSTANT)
		Q_PROPERTY(bool videoSourceMode READ getVideoSourceMode CONSTANT)
		Q_PROPERTY(bool osd READ getOSD CONSTANT)
		Q_PROPERTY(bool temporaryOSDText READ getTemporaryOSDText CONSTANT)
		Q_PROPERTY(bool eXICompression READ getEXICompression CONSTANT)
		Q_PROPERTY(int maximumNumberOfProfiles READ getMaximumNumberOfProfiles CONSTANT)
		Q_PROPERTY(bool rTPMulticast READ getRTPMulticast CONSTANT)
		Q_PROPERTY(bool rTPTcp READ getRTPTcp CONSTANT)
		Q_PROPERTY(bool rTPRtspTcp READ getRTPRtspTcp CONSTANT)
		Q_PROPERTY(bool nonAggregateControl READ getNonAggregateControl CONSTANT)
		Q_PROPERTY(bool noRTSPStreaming READ getNoRTSPStreaming CONSTANT)
		Q_PROPERTY(QUrl serviceEndpoint READ getServiceEndpoint CONSTANT)

public:

	MediaService();
	QUrl getServiceEndpoint() const;
	void setServiceEndpoint(const QUrl &val);
	bool isInitialized() const;
	void setInitialized(bool initialized);
	bool getSnapshotUri() const;
	void setSnapshotUri(bool val);
	bool getRotation() const;
	void setRotation(bool val);
	bool getVideoSourceMode() const;
	void setVideoSourceMode(bool val);
	bool getOSD() const;
	void setOSD(bool val);
	bool getTemporaryOSDText() const;
	void setTemporaryOSDText(bool val);
	bool getEXICompression() const;
	void setEXICompression(bool val);
	int getMaximumNumberOfProfiles() const;
	void setMaximumNumberOfProfiles(int val);
	bool getRTPMulticast() const;
	void setRTPMulticast(bool val);
	bool getRTPTcp() const;
	void setRTPTcp(bool val);
	bool getRTPRtspTcp() const;
	void setRTPRtspTcp(bool val);
	bool getNonAggregateControl() const;
	void setNonAggregateControl(bool val);
	bool getNoRTSPStreaming() const;
	void setNoRTSPStreaming(bool val);

private:

	QUrl serviceEndpoint;
	bool mInitialized;
	bool mSnapshotUri;
	bool mRotation;
	bool mVideoSourceMode;
	bool mOSD;
	bool mTemporaryOSDText;
	bool mEXICompression;
	int mMaximumNumberOfProfiles;
	bool mRTPMulticast;
	bool mRTPTcp;
	bool mRTPRtspTcp;
	bool mNonAggregateControl;
	bool mNoRTSPStreaming;
};
