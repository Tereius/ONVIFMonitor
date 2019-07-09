#include "MediaService.h"


MediaService::MediaService() :
mInitialized(false),
mSnapshotUri(false),
mRotation(false),
mVideoSourceMode(false),
mOSD(false),
mTemporaryOSDText(false),
mEXICompression(false),
mMaximumNumberOfProfiles(-1),
mRTPMulticast(false),
mRTPTcp(false),
mRTPRtspTcp(false),
mNonAggregateControl(false),
mNoRTSPStreaming(false) {

}

QUrl MediaService::getServiceEndpoint() const {

	return serviceEndpoint;
}

void MediaService::setServiceEndpoint(const QUrl &val) {

	serviceEndpoint = val;
}

bool MediaService::isInitialized() const {

	return mInitialized;
}

void MediaService::setInitialized(bool initialized) {

	mInitialized = initialized;
}

bool MediaService::getSnapshotUri() const {

	return mSnapshotUri;
}

void MediaService::setSnapshotUri(bool val) {

	mSnapshotUri = val;
}

bool MediaService::getRotation() const {

	return mRotation;
}

void MediaService::setRotation(bool val) {

	mRotation = val;
}

bool MediaService::getVideoSourceMode() const {

	return mVideoSourceMode;
}

void MediaService::setVideoSourceMode(bool val) {

	mVideoSourceMode = val;
}

bool MediaService::getOSD() const {

	return mOSD;
}

void MediaService::setOSD(bool val) {

	mOSD = val;
}

bool MediaService::getTemporaryOSDText() const {

	return mTemporaryOSDText;
}

void MediaService::setTemporaryOSDText(bool val) {

	mTemporaryOSDText = val;
}

bool MediaService::getEXICompression() const {

	return mEXICompression;
}

void MediaService::setEXICompression(bool val) {

	mEXICompression = val;
}

int MediaService::getMaximumNumberOfProfiles() const {

	return mMaximumNumberOfProfiles;
}

void MediaService::setMaximumNumberOfProfiles(int val) {

	mMaximumNumberOfProfiles = val;
}

bool MediaService::getRTPMulticast() const {

	return mRTPMulticast;
}

void MediaService::setRTPMulticast(bool val) {

	mRTPMulticast = val;
}

bool MediaService::getRTPTcp() const {

	return mRTPTcp;
}

void MediaService::setRTPTcp(bool val) {

	mRTPTcp = val;
}

bool MediaService::getRTPRtspTcp() const {

	return mRTPRtspTcp;
}

void MediaService::setRTPRtspTcp(bool val) {

	mRTPRtspTcp = val;
}

bool MediaService::getNonAggregateControl() const {

	return mNonAggregateControl;
}

void MediaService::setNonAggregateControl(bool val) {

	mNonAggregateControl = val;
}

bool MediaService::getNoRTSPStreaming() const {

	return mNoRTSPStreaming;
}

void MediaService::setNoRTSPStreaming(bool val) {

	mNoRTSPStreaming = val;
}
