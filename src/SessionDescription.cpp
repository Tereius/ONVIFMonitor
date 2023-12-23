#include "SessionDescription.h"
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QRegularExpression>

SessionDescription::SessionDescription(QByteArray data) : mTimeDescription(), mMediaDescriptions() {

	auto msg = data.removeIf([](auto ele) { return ele == '\r'; });
	auto tmpDescription = MediaDescription();
	MediaDescription &current = tmpDescription;
	for(const auto &sdpLine : msg.split('\n')) {
		if(sdpLine.size() > 2) {
			const auto key = QString::fromUtf8(sdpLine.first(1)).trimmed();
			const auto val = QString::fromUtf8(sdpLine.last(sdpLine.size() - 2)).trimmed();
			if(key == "m") {
				mMediaDescriptions.push_back({});
				mMediaDescriptions.last().mediaName = val;
			} else if(key == "t") {
				mTimeDescription.timeActive = val;
			} else if(key == "r") {
				mTimeDescription.repeatTimes.push_back(val);
			} else if(!mMediaDescriptions.isEmpty()) {
				if(key == "i") {
					mMediaDescriptions.last().mediaTitle = val;
				} else if(key == "c") {
					mMediaDescriptions.last().connectionInfo = val;
				} else if(key == "b") {
					mMediaDescriptions.last().bandwidth.push_back(val);
				} else if(key == "k") {
					mMediaDescriptions.last().encryptionKey = val;
				} else if(key == "a") {
					mMediaDescriptions.last().attributes.push_back(val);
				}
			}
		}
	}
}

TimeDescription SessionDescription::getTimeDescription() const {

	return mTimeDescription;
}

QList<MediaDescription> SessionDescription::getMediaDescriptions() const {

	return mMediaDescriptions;
}

MediaDescription SessionDescription::getMediaDescriptionWithAttribute(const QString &attr) const {

	for(const auto &mediaDescription : mMediaDescriptions) {
		for(const auto &attribute : mediaDescription.attributes) {
			if(attribute.startsWith(attr)) {
				return mediaDescription;
			}
		}
	}
	return {};
}

QString MediaDescription::getAttributeValueFirst(const QString &key) const {

	const auto values = getAttributeValues(key);
	if(!values.isEmpty()) {
		return values.first();
	}
	return {};
}

QStringList MediaDescription::getAttributeValues(const QString &key) const {

	QStringList ret;
	for(auto const &attribute : attributes) {
		auto splitIntex = attribute.indexOf(':');
		if(splitIntex > 0) {
			const auto &attrKey = attribute.first(splitIntex);
			auto attrVal = attribute.last(attribute.size() - splitIntex - 1);
			if(attrKey == key) {
				ret.push_back(attrVal);
			}
		}
	}
	return ret;
}

bool MediaDescription::hasPayloadFormat(quint8 payloadType) const {

	return getPayloadFormat(payloadType).isValid();
}

RtpPayloadFormat MediaDescription::getPayloadFormat(quint8 payloadType) const {

	for(const auto &format : getPayloadFormats()) {
		if(format.payloadType == payloadType) {
			return format;
		}
	}
	return {};
}

QList<RtpPayloadFormat> MediaDescription::getPayloadFormats() const {

	QRegularExpression reg(R"((\d+)\s([^\/]+)\/(\d+)\/(\d+))");
	QList<RtpPayloadFormat> ret;
	for(const auto &rtmap : getAttributeValues("rtpmap")) {
		const auto match = reg.match(rtmap);
		RtpPayloadFormat format;
		format.payloadType = match.captured(1).toInt();
		format.name = match.captured(2);
		format.sampleRate = match.captured(3).toInt();
		format.numChannels = match.captured(4).toInt();
		ret.push_back(format);
	}
	return ret;
}

bool RtpPayloadFormat::isValid() const {

	return payloadType >= 0;
}
