#include "RtspMessage.h"
#include "App.h"
#include <QRegularExpression>

#define LINE_ENDING "\r\n"


QList<QString> RtspMessage::getFields(const QString &key) const {

	return mFields.values(key);
}

QString RtspMessage::getFirstField(const QString &key) const {

	if(!getFields(key).isEmpty()) {
		return getFields(key).first();
	}
	return {};
}

QString RtspMessage::getFirstFieldValue(const QString &key, const QString &value) const {

	QRegularExpression reg(value + "=([^;]*)");
	QRegularExpressionMatch match = reg.match(getFirstField(key), 1);
	if(match.hasMatch()) {
		return match.captured(1);
	}
	return {};
}

bool RtspMessage::hasField(const QString &key) const {

	return !getFields(key).isEmpty();
}

void RtspMessage::addField(const QString &key, const QString &value) {

	mFields.insert(key.trimmed(), value.trimmed());
}

void RtspMessage::replaceField(const QString &key, const QString &value) {

	mFields.replace(key.trimmed(), value.trimmed());
}

bool RtspMessage::hasPayload() const {

	return !mPayload.isEmpty();
}

QByteArray RtspMessage::getPayload() const {

	return mPayload;
}

RtspMessage::RtspMessage(const QList<QString> &header, QByteArray payload) : mHeader(header), mFields(), mPayload(payload) {}

QList<QString> RtspMessage::getHeader() const {

	return mHeader;
}

void RtspMessage::setHeader(const QList<QString> &header) {

	mHeader = header;
}

QMultiHash<QString, QString> RtspMessage::getAllFields() const {

	return mFields;
}

RtspMessageRequest::RtspMessageRequest(const QString &method, const QString &url) : RtspMessage({method, url, "RTSP/1.0"}, {}) {
	replaceField("User-Agent", App::getDefaultUserAgent());
}

QString RtspMessageRequest::getMethod() const {
	if(getHeader().length() >= 1) {
		return getHeader().at(0);
	}
	return {};
}

QString RtspMessageRequest::getUrl() const {
	if(getHeader().length() >= 2) {
		return getHeader().at(1);
	}
	return {};
}

QByteArray RtspMessageRequest::serialize(int cSeq, const QString &authString /*= {}*/) const {

	auto data = getHeader().join(" ").append(LINE_ENDING);
	data = data.append("%1: %2" LINE_ENDING).arg("CSeq", QString::number(cSeq));
	if(!authString.isEmpty()) {
		data = data.append("%1: %2" LINE_ENDING).arg("Authorization", authString);
	}
	auto fields = getAllFields();
	for(auto iter = fields.constBegin(); iter != fields.constEnd(); ++iter) {
		data = data.append("%1: %2" LINE_ENDING).arg(iter.key(), iter.value());
	}
	data = data.append(LINE_ENDING);
	return data.toUtf8();
}

RtspMessageResponse::RtspMessageResponse(QByteArray data /*= {}*/, QByteArray payload /*= {}*/) : RtspMessage({}, payload) {

	auto msg = data.removeIf([](auto ele) { return ele == '\r'; });
	auto first = true;
	for(const auto &line : msg.split('\n')) {
		if(first) {
			setHeader(QString::fromUtf8(line).split(" "));
			first = false;
		} else {
			auto index = line.indexOf(":");
			if(index > 0) {
				const auto key = QString::fromUtf8(line.first(index));
				const auto value = QString::fromUtf8(line.last(line.size() - index - 1));
				addField(key, value);
			}
		}
	}
}

int RtspMessageResponse::getStatus() const {

	if(getHeader().length() >= 2) {
		return getHeader().at(1).toInt();
	}
	return -1;
}

QString RtspMessageResponse::getStatusMessage() const {
	if(getHeader().length() >= 3) {
		return getHeader().at(2);
	}
	return {};
}
