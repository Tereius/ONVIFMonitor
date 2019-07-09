#include "EventService.h"
#include <QMutexLocker>


EventService::EventService() :
	mInitialized(false),
	mWSSubscriptionPolicySupport(false),
	mWSPullPointSupport(false),
	mWSPausableSubscriptionManagerInterfaceSupport(false),
	mMaxNotificationProducers(false),
	mMaxPullPoints(-1),
	mPersistentNotificationStorage(false) {

}

QUrl EventService::getServiceEndpoint() const {

	return serviceEndpoint;
}

void EventService::setServiceEndpoint(const QUrl &val) {

	serviceEndpoint = val;
}

bool EventService::isInitialized() const {

	return mInitialized;
}

void EventService::setInitialized(bool initialized) {

	mInitialized = initialized;
}

bool EventService::getWSSubscriptionPolicySupport() const {

	return mWSSubscriptionPolicySupport;
}

void EventService::setWSSubscriptionPolicySupport(bool val) {

	mWSSubscriptionPolicySupport = val;
}

bool EventService::getWSPullPointSupport() const {

	return mWSPullPointSupport;
}

void EventService::setWSPullPointSupport(bool val) {

	mWSPullPointSupport = val;
}

bool EventService::getWSPausableSubscriptionManagerInterfaceSupport() const {

	return mWSPausableSubscriptionManagerInterfaceSupport;
}

void EventService::setWSPausableSubscriptionManagerInterfaceSupport(bool val) {

	mWSPausableSubscriptionManagerInterfaceSupport = val;
}

int EventService::getMaxNotificationProducers() const {

	return mMaxNotificationProducers;
}

void EventService::setMaxNotificationProducers(int val) {

	mMaxNotificationProducers = val;
}

int EventService::getMaxPullPoints() const {

	return mMaxPullPoints;
}

void EventService::setMaxPullPoints(int val) {

	mMaxPullPoints = val;
}

bool EventService::getPersistentNotificationStorage() const {

	return mPersistentNotificationStorage;
}

void EventService::setPersistentNotificationStorage(bool val) {

	mPersistentNotificationStorage = val;
}
