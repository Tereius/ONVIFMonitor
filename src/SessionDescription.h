#pragma once
#include <QByteArray>
#include <QList>
#include <QObject>
#include <QString>


struct TimeDescription {

	QString timeActive;
	QList<QString> repeatTimes;
};

struct RtpPayloadFormat {

	bool isValid() const;

	int payloadType = -1;
	QString name;
	int sampleRate = -1;
	int numChannels = -1;
};

class MediaDescription {

	Q_GADGET

 public:
	bool isNull() const { return mediaName.isNull(); }
	QString getAttributeValueFirst(const QString &key) const;
	QStringList getAttributeValues(const QString &key) const;
	bool hasPayloadFormat(quint8 payloadType) const;
	RtpPayloadFormat getPayloadFormat(quint8 payloadType) const;
	QList<RtpPayloadFormat> getPayloadFormats() const;

	QString mediaName; // m (mandatory)
	QString mediaTitle; // i (optional)
	QString connectionInfo; // c (optional)
	QList<QString> bandwidth; // b (zero or more)
	QString encryptionKey; // k (optional)
	QList<QString> attributes; // a (zero or more)
};

class SessionDescription {

 public:
	explicit SessionDescription(QByteArray data);
	TimeDescription getTimeDescription() const;
	QList<MediaDescription> getMediaDescriptions() const;
	MediaDescription getMediaDescriptionWithAttribute(const QString &attr) const;

 private:
	TimeDescription mTimeDescription;
	QList<MediaDescription> mMediaDescriptions;
};
