#include "RtspStream.h"


RtspStream::RtspStream() : RtspStream::RtspStream(MediaDescription()) {}

RtspStream::RtspStream(const MediaDescription &mediaDescription) :
 mMediaDescription(mediaDescription), mSsrc(0), mServerRtpPort(0), mServerRtcpPort(0), mClientRtpPort(0), mClientRtcpPort(0) {}

MediaDescription RtspStream::getMediaDescription() const {

	return mMediaDescription;
}

int RtspStream::getSsrc() const {

	return mSsrc;
}

void RtspStream::setSsrc(int ssrc) {

	RtspStream::mSsrc = ssrc;
}

quint16 RtspStream::getServerRtpPort() const {

	return mServerRtpPort;
}

void RtspStream::setServerRtpPort(quint16 serverRtpPort) {

	RtspStream::mServerRtpPort = serverRtpPort;
}

quint16 RtspStream::getServerRtcpPort() const {

	return mServerRtcpPort;
}

void RtspStream::setServerRtcpPort(quint16 serverRtcpPort) {

	RtspStream::mServerRtcpPort = serverRtcpPort;
}

quint16 RtspStream::getClientRtpPort() const {

	return mClientRtpPort;
}

void RtspStream::setClientRtpPort(quint16 clientRtpPort) {

	RtspStream::mClientRtpPort = clientRtpPort;
}

quint16 RtspStream::getClientRtcpPort() const {

	return mClientRtcpPort;
}

void RtspStream::setClientRtcpPort(quint16 clientRtcpPort) {

	RtspStream::mClientRtcpPort = clientRtcpPort;
}
