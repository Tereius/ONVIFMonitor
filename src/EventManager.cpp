#include "EventManager.h"
#include "Device.h"
#include "DeviceManager.h"
#include "EventBinding.h"
#include "OnvifPullPoint.h"
#include "Window.h"
#include <QSettings>
#include <QtConcurrent>


Q_GLOBAL_STATIC(EventManager, globalManager)

EventManager::EventManager(QObject *pParent /*= nullptr*/) : QObject(pParent), mPullPoints(), mMutex(QMutex::Recursive) {

	connect(DeviceM, SIGNAL(deviceInitialized(const Uuid &)), this, SLOT(initPullPoint(const Uuid &)), Qt::QueuedConnection);
}

EventManager *EventManager::getGlobal() {

	return globalManager;
}

QSharedPointer<OnvifPullPoint> EventManager::getPullPoint(Uuid deviceId) const {

	return QSharedPointer<OnvifPullPoint>(mPullPoints.value(deviceId));
}

void EventManager::addBinding(const QString &rName, const QString &rDescription) {

	auto bindingId = Uuid::createUuid();
	auto binding = QSharedPointer<EventBinding>::create();
	binding->setId(bindingId);
	binding->setName(rName);
	binding->setDescription(rDescription);
	connect(binding.data(), &EventBinding::eventOccured, this, &EventManager::eventOccured);
	mMutex.lock();
	mInstalledEventBindings.insert(bindingId, binding);
	mMutex.unlock();
	QSettings settings;
	settings.beginGroup("eventBindings");
	settings.beginGroup(bindingId.toString());
	settings.setValue("name", rName);
	settings.setValue("description", rDescription);
	emit eventBindingAdded(bindingId);
}


Result EventManager::bindSource(const Uuid &rBindingId, const QString &rEventSourceType /*= QString()*/,
                                const QVariantMap &rProperties /*= QVariantMap()*/) {

	Result result;
	if(!rBindingId.isNull()) {
		if(mRegisteredEventSources.contains(rEventSourceType)) {
			mMutex.lock();
			auto binding = mInstalledEventBindings.value(rBindingId);
			if(binding) {
				result = binding->createSource(mRegisteredEventSources.value(rEventSourceType).getTypeId(), rProperties);
				if(result) {
					QSettings settings;
					settings.beginGroup("eventBindings");
					settings.beginGroup(rBindingId.toString());
					settings.value("eventSourceType", rEventSourceType);
					settings.value("eventSourceProperties", rProperties);
					if(binding->hasHandler()) {
						result = binding->bind();
					}
					emit eventBindingChanged(binding->getId());
				} else {
					qWarning() << "Couldn't bind event source";
				}
			} else {
				result = Result(Result::FAULT, tr("Couldn't connect source to non existing binding"));
				qWarning() << result;
			}
			mMutex.unlock();
		} else {
			result = Result(Result::FAULT, tr("Invalid binding Id: %1").arg(rEventSourceType));
			qWarning() << result;
		}
	} else {
		result = Result(Result::FAULT, tr("Invalid binding Id"));
		qWarning() << result;
	}
	return result;
}

Result EventManager::unbindSource(const Uuid &rBindingId) {

	Result result;
	mMutex.lock();
	auto binding = mInstalledEventBindings.value(rBindingId);
	if(binding) {
		binding->removeSource();
		QSettings settings;
		settings.beginGroup("eventBindings");
		settings.beginGroup(rBindingId.toString());
		emit eventBindingChanged(binding->getId());
	} else {
		result = Result(Result::FAULT, tr("Couldn't disconnect source from non existing binding"));
		qWarning() << result;
	}
	mMutex.unlock();
	return result;
}

Result EventManager::bindHandler(const Uuid &rBindingId, const QString &rEventHandlerType,
                                 const QVariantMap &rProperties /*= QVariantMap()*/) {

	Result result;
	if(!rBindingId.isNull()) {
		if(mRegisteredEventHandler.contains(rEventHandlerType)) {
			mMutex.lock();
			auto binding = mInstalledEventBindings.value(rBindingId);
			if(binding) {
				result = binding->createHandler(mRegisteredEventHandler.value(rEventHandlerType).getTypeId(), rProperties);
				if(result) {
					QSettings settings;
					settings.beginGroup("eventBindings");
					settings.beginGroup(rBindingId.toString());
					settings.value("eventHandlerType", rEventHandlerType);
					settings.value("eventHandlerProperties", rProperties);
					if(binding->hasSource()) {
						result = binding->bind();
					}
					emit eventBindingChanged(binding->getId());
				} else {
					qWarning() << "Couldn't bind event handler";
				}
			} else {
				result = Result(Result::FAULT, tr("Couldn't connect handler to non existing binding"));
				qWarning() << result;
			}
			mMutex.unlock();
		} else {
			result = Result(Result::FAULT, tr("Invalid binding Id: %1").arg(rEventHandlerType));
			qWarning() << result;
		}
	} else {
		result = Result(Result::FAULT, tr("Invalid binding Id"));
		qWarning() << result;
	}
	return result;
}

Result EventManager::unbindHandler(const Uuid &rBindingId) {

	Result result;
	mMutex.lock();
	auto binding = mInstalledEventBindings.value(rBindingId);
	if(binding) {
		binding->removeHandler();
		QSettings settings;
		settings.beginGroup("eventBindings");
		settings.beginGroup(rBindingId.toString());
		emit eventBindingChanged(binding->getId());
	} else {
		result = Result(Result::FAULT, tr("Couldn't disconnect handler from non existing binding"));
		qWarning() << result;
	}
	mMutex.unlock();
	return result;
}

Result EventManager::triggerHandler(const Uuid &rBindingId) {

	Result result;
	auto binding = mInstalledEventBindings.value(rBindingId);
	if(binding) {
		result = binding->triggerHandler();
	} else {
		result = Result(Result::FAULT, tr("Couldn't trigger handler from non existing binding"));
		qWarning() << result;
	}
	return result;
}

bool EventManager::doesBindingNameExist(const QString &rName) {

	bool isUnique = false;
	auto probeName = rName;
	while(!isUnique) {
		isUnique = true;
		mMutex.lock();
		for(auto binding : mInstalledEventBindings.values()) {
			if(probeName == binding->getName()) {
				isUnique = false;
				break;
			}
		}
		mMutex.unlock();
	}
	return isUnique;
}

void EventManager::bindEvents() {}

void EventManager::initialize() {

	initEvents();
}

FutureResult *EventManager::getDeviceTopics(const Uuid &rDeviceId) {

	auto pResult = new FutureResult();
	auto device = DeviceM->getDevice(rDeviceId);
	if(device) {
		QtConcurrent::run([pResult, device, rDeviceId]() {
			auto topics = device->getTopics();
			if(topics) {
				auto prof = topics.GetResultObject();
				pResult->resolveResult(QVariant::fromValue(prof));
			} else {
				pResult->resolveEmptyResult();
				Window::getGlobal()->showError(tr("Error"), topics.toString());
			}
		});
	}
	return pResult;
}

Promise<QString> EventManager::testFuture(const QString &rDeviceId) {

	Promise<QString> result;

	QtConcurrent::run([result, rDeviceId]() {
		QThread::msleep(2000);
		result.resolve("testFuture resolved for device " + rDeviceId);
	});

	return result;
}

Promise<bool> EventManager::testFutureTwo(const QString &rDeviceId) {

	Promise<bool> result;

	QtConcurrent::run([result, rDeviceId]() {
		QThread::msleep(2000);
		result.resolve(true);
	});

	return result;
}

void EventManager::initPullPoint(const Uuid &rDeviceId) {

	auto info = DeviceM->getDeviceInfo(rDeviceId);
	if(info.isInitialized()) {
		// Check if we have to initialize a pull point
		QSettings settings;
		settings.beginGroup("events");
		auto eventGroup = settings.childGroups();
		for(int i = 0; i < eventGroup.size(); ++i) {
			settings.beginGroup(eventGroup.at(i));
			Uuid eventId = Uuid(settings.value("id").toUuid());
			Uuid deviceId = Uuid(settings.value("deviceId").toUuid());
			QString filterExpression = settings.value("expression").toString();

			if(!eventId.isNull() && !deviceId.isNull()) {
				if(deviceId == rDeviceId) {
					if(info.getEventService().isInitialized()) {
						// We have to initialize a pull point
						mMutex.lock();
						if(!mPullPoints.value(deviceId)) {
							// Create new pull point
							auto pullPoint = new OnvifPullPoint(info.getEventService().getServiceEndpoint(), this);
							connect(
							 pullPoint, &OnvifPullPoint::UnsuccessfulPull, this,
							 [this, deviceId](int unsuccessfulPullcount, const SimpleResponse &rCause) {
								 if(unsuccessfulPullcount == 5) emit lostPullPoint(deviceId);
							 },
							 Qt::QueuedConnection);
							pullPoint->Start();
							mPullPoints.insert(deviceId, pullPoint);
						} else {
							//	Restart existing pull point
							mPullPoints.value(deviceId)->Stop();
							mPullPoints.value(deviceId)->Start();
						}
						mMutex.unlock();
					} else {
						qWarning() << "Couldn't get event service from device";
					}
					break;
				}
			} else {
				qWarning() << "Found invalid event from settings";
			}
			settings.endGroup();
		}
	}
}

void EventManager::initEvents() {}

QHash<QString, EventHandlerInfo> EventManager::mRegisteredEventHandler;

QHash<QString, EventSourceInfo> EventManager::mRegisteredEventSources;
