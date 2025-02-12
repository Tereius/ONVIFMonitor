#pragma once
#include "MediaProfile.h"
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QUuid>


class DeviceInfo {

	Q_GADGET
	Q_PROPERTY(QString deviceName MEMBER mDeviceName CONSTANT)
	Q_PROPERTY(QUrl deviceEndpoint MEMBER mEndpoint CONSTANT)
	Q_PROPERTY(QUrl eventEndpoint MEMBER mEventEndpoint CONSTANT)
	Q_PROPERTY(QString user MEMBER mUser CONSTANT)
	Q_PROPERTY(QString password MEMBER mPassword CONSTANT)
	Q_PROPERTY(QStringList scopes MEMBER mScopes CONSTANT)
	Q_PROPERTY(bool initialized MEMBER mInitialized CONSTANT)
	Q_PROPERTY(QString initializationError MEMBER mError CONSTANT)
	Q_PROPERTY(QString firmwareVersion MEMBER mFirmwareVersion CONSTANT)
	Q_PROPERTY(QString hardwareId MEMBER mHardwareId CONSTANT)
	Q_PROPERTY(QString manufacturer MEMBER mManufacturer CONSTANT)
	Q_PROPERTY(QString model MEMBER mModel CONSTANT)
	Q_PROPERTY(QString serialNumber MEMBER mSerialNumber CONSTANT)
	Q_PROPERTY(qint64 dateTimeOffset MEMBER mDateTimeOffset CONSTANT)
	Q_PROPERTY(QUuid endpointReference MEMBER mEndpointReference CONSTANT)
	Q_PROPERTY(bool hasMediaService MEMBER mHasMediaService CONSTANT)
	Q_PROPERTY(QList<MediaProfile> mediaProfiles MEMBER mMediaProfiles)

 public:
	QString mDeviceName;
	QUrl mEndpoint;
	QUrl mEventEndpoint;
	QString mUser;
	QString mPassword;
	QStringList mScopes;
	bool mInitialized = false;
	QString mError;
	QString mFirmwareVersion;
	QString mHardwareId;
	QString mManufacturer;
	QString mModel;
	QString mSerialNumber;
	qint64 mDateTimeOffset = 0;
	QUuid mEndpointReference;
	bool mHasMediaService = false;
	QList<MediaProfile> mMediaProfiles;
};
