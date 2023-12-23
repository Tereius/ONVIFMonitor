#pragma once
#include "SessionDescription.h"
#include <QtGlobal>


class RtspStream {

 public:
	RtspStream();
	explicit RtspStream(const MediaDescription &mediaDescription);
	MediaDescription getMediaDescription() const;
	int getSsrc() const;
	void setSsrc(int mSsrc);
	quint16 getServerRtpPort() const;
	void setServerRtpPort(quint16 mServerRtpPort);
	quint16 getServerRtcpPort() const;
	void setServerRtcpPort(quint16 mServerRtcpPort);
	quint16 getClientRtpPort() const;
	void setClientRtpPort(quint16 mClientRtpPort);
	quint16 getClientRtcpPort() const;
	void setClientRtcpPort(quint16 mClientRtcpPort);

 private:
	MediaDescription mMediaDescription;
	int mSsrc;
	quint16 mServerRtpPort;
	quint16 mServerRtcpPort;
	quint16 mClientRtpPort;
	quint16 mClientRtcpPort;
};
