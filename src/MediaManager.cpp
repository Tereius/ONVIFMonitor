#include "MediaManager.h"
#include "DeviceManager.h"
#include "Device.h"
#include "Window.h"
#include "FutureResult.h"
#include <QGlobalStatic>
#include <QJSEngine>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QFuture>


Q_GLOBAL_STATIC(MediaManager, globalManager)

MediaManager::MediaManager(QObject *pParent /*= nullptr*/) :
	QObject(pParent),
	mMutex() {

}

QUrl MediaManager::getStreamUrl(const ProfileId &rProfileId) const {

	QUrl ret;
	auto device = DeviceM->getDevice(rProfileId.getDeviceId());
	if(device) {
		ret = device->getStreamUrl(rProfileId.getProfileToken());
	}
	return ret;
}

FutureResult* MediaManager::getMediaProfile(const ProfileId &rProfileId) {

	auto pResutl = new FutureResult();
	//QQmlEngine::setObjectOwnership(pResutl, QQmlEngine::JavaScriptOwnership);
	auto device = DeviceM->getDevice(rProfileId.getDeviceId());
	if(device) {
		QtConcurrent::run([pResutl, device, rProfileId]() {
			auto profile = device->getMediaProfile(rProfileId.getProfileToken());
			if(profile) {
				auto prof = profile.GetResultObject();
				pResutl->resolveResult(QVariant::fromValue(prof));
			}
			else {
				pResutl->resolveEmptyResult();
				Window::getGlobal()->showError(tr("Error"), profile.toString());
			}
		});
	}
	return pResutl;
}

MediaManager* MediaManager::getGlobal() {

	return globalManager;
}
