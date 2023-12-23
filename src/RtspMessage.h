#pragma once
#include <QMultiHash>
#include <QString>

class RtspMessage {

 public:
	QList<QString> getFields(const QString &key) const;
	QString getFirstField(const QString &key) const;
	QString getFirstFieldValue(const QString &key, const QString &value) const;
	bool hasField(const QString &key) const;
	void addField(const QString &key, const QString &value);
	void replaceField(const QString &key, const QString &value);
	bool hasPayload() const;
	QByteArray getPayload() const;

 protected:
	RtspMessage(const QList<QString> &header, QByteArray payload);
	QList<QString> getHeader() const;
	void setHeader(const QList<QString> &header);
	QMultiHash<QString, QString> getAllFields() const;

 private:
	QList<QString> mHeader;
	QMultiHash<QString, QString> mFields;
	QByteArray mPayload;
};

class RtspMessageRequest : public RtspMessage {

 public:
	RtspMessageRequest(const QString &method, const QString &url);
	QString getMethod() const;
	QString getUrl() const;
	QByteArray serialize(int cSeq, const QString &authString = {}) const;
};

class RtspMessageResponse : public RtspMessage {

 public:
	explicit RtspMessageResponse(QByteArray data = {}, QByteArray payload = {});
	int getStatus() const;
	QString getStatusMessage() const;
};
