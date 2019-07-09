#pragma once
#include "ProfileId.h"
#include "Uuid.h"
#include <QMetaType>
#include <QString>


class MediaProfile {

	Q_GADGET
		Q_PROPERTY(bool fixed READ getFixed CONSTANT)

public:

	MediaProfile(const Uuid &rOwnerDeviceId = Uuid());
	QString getName() const;
	void setName(const QString &rName);
	QString getToken() const;
	void setToken(const QString &rToken);
	Uuid getDeviceId() const;
	ProfileId getProfileId() const;
	bool getFixed() const;
	void setFixed(bool fixed);

private:

	Uuid mDeviceId;
	QString mName;
	QString mToken;
	bool mFixed;
};


class VideoEncoderConfiguration {
	
public:
	VideoEncoderConfiguration() {}

};

Q_DECLARE_METATYPE(VideoEncoderConfiguration)