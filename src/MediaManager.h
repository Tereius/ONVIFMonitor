#pragma once
#include "ProfileId.h"
#include "MediaProfile.h"
#include <QObject>
#include <QMutex>
#include <QJSValue>


class FutureResult;

class MediaManager : public QObject {

	Q_OBJECT

public:
	MediaManager(QObject *pParent = nullptr);
	Q_INVOKABLE QUrl getStreamUrl(const ProfileId &rProfileId) const;
	Q_INVOKABLE FutureResult* getMediaProfile(const ProfileId &rProfileId);

	static MediaManager* getGlobal();

private:
	Q_DISABLE_COPY(MediaManager)

		QMutex mMutex;
};
