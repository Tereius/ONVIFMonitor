#pragma once
#include "MediaService.h"
#include "EventService.h"
#include "Uuid.h"
#include <QUrl>
#include <QString>
#include <QMetaType>


class DeviceInfo {

	Q_GADGET
		Q_PROPERTY(Uuid deviceId READ getDeviceId CONSTANT)
		Q_PROPERTY(QString deviceName READ getDeviceName CONSTANT)
		Q_PROPERTY(bool initialized READ isInitialized CONSTANT)
		Q_PROPERTY(QString error READ getError CONSTANT)
		Q_PROPERTY(QString firmwareVersion READ getFirmwareVersion CONSTANT)
		Q_PROPERTY(QString hardwareId READ getHardwareId CONSTANT)
		Q_PROPERTY(QString manufacturer READ getManufacturer CONSTANT)
		Q_PROPERTY(QString model READ getModel CONSTANT)
		Q_PROPERTY(QString serialNumber READ getSerialNumber CONSTANT)
		Q_PROPERTY(qint64 dateTimeOffset READ getDateTimeOffset CONSTANT)
		Q_PROPERTY(QUrl deviceEndpoint READ getEndpoint CONSTANT)
		Q_PROPERTY(Uuid endpointReference READ getEndpointReference CONSTANT)
		Q_PROPERTY(QString host READ getHost CONSTANT)
		Q_PROPERTY(int port READ getPort CONSTANT)
		Q_PROPERTY(EventService eventService READ getEventService CONSTANT)
		Q_PROPERTY(MediaService mediaService READ getMediaService CONSTANT)

public:
	DeviceInfo();
	bool isNull() const;
	Uuid getDeviceId() const;
	void setDeviceId(const Uuid &rid);
	QString getFirmwareVersion() const;
	void setFirmwareVersion(const QString &val);
	QString getHardwareId() const;
	void setHardwareId(const QString &val);
	QString getManufacturer() const;
	void setManufacturer(const QString &val);
	QString getModel() const;
	void setModel(const QString &val);
	QString getSerialNumber() const;
	void setSerialNumber(const QString &val);
	qint64 getDateTimeOffset() const;
	void setDateTimeOffset(qint64 val);
	QUrl getEndpoint() const;
	void setEndpoint(const QUrl &val);
	QString getDeviceName() const;
	void setDeviceName(const QString &val);
	bool isInitialized() const;
	void setInitialized(bool initialized);
	QString getHost() const;
	int getPort() const;
	Uuid getEndpointReference() const;
	void setEndpointReference(const Uuid &val);
	QString getError() const;
	void  setError(const QString &rError);
	EventService getEventService() const;
	void setEventService(const EventService &rService);
	MediaService getMediaService() const;
	void setMediaService(const MediaService &rService);

private:

	Uuid mDeviceId;
	QString mDeviceName;
	bool mInitialized;
	QString mError;
	QString mFirmwareVersion;
	QString mHardwareId;
	QString mManufacturer;
	QString mModel;
	QString mSerialNumber;
	qint64 mDateTimeOffset;
	QUrl mEndpoint;
	Uuid mEndpointReference;
	EventService mEventService;
	MediaService mMediaService;
};
