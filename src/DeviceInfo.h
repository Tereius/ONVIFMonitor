#pragma once
#include "MediaProfile.h"
#include <QStringList>
#include <QList>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QUuid>


class DeviceInfo {

	Q_GADGET
	Q_PROPERTY(QString deviceName MEMBER mDeviceName CONSTANT)
	Q_PROPERTY(QUrl deviceEndpoint MEMBER mEndpoint CONSTANT)
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
	QString mDeviceName = QString();
	QUrl mEndpoint = QUrl();
	QString mUser = QString();
	QString mPassword = QString();
	QStringList mScopes = QStringList();
	bool mInitialized = false;
	QString mError = QString();
	QString mFirmwareVersion = QString();
	QString mHardwareId = QString();
	QString mManufacturer = QString();
	QString mModel = QString();
	QString mSerialNumber = QString();
	qint64 mDateTimeOffset = 0;
	QUuid mEndpointReference = QUuid();
	bool mHasMediaService = false;
	QList<MediaProfile> mMediaProfiles = {};
};
