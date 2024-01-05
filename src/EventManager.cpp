#include "EventManager.h"
#include "EventBinding.h"
#include "FutureResult.h"
#include "OnvifPullPoint.h"
#include "Window.h"
#include <QGlobalStatic>
#include <QRemoteObjectNode>
#include <QRemoteObjectPendingCall>
#include <QSettings>
#include <QVariant>


class EventManagerrSingleton : public EventManager {};

Q_GLOBAL_STATIC(EventManagerrSingleton, instance)

EventManager *EventManager::getInstance() {

	return instance;
}

EventManager::EventManager(QObject *pParent /*= nullptr*/) :
 QObject(pParent), mPullPoints(), mEventDevices(), mMutex(), mpReplica(nullptr) {

	// connect(DeviceManager::getInstance(), SIGNAL(deviceInitialized(const Uuid &)), this, SLOT(initPullPoint(const Uuid &)),
	//         Qt::QueuedConnection);
}

EventManager::~EventManager() = default;

void EventManager::initialize() {

	auto *repNode = new QRemoteObjectNode(this); // create remote object node
	repNode->setHeartbeatInterval(10000);
	qInfo() << "connect to remote object" << repNode->connectToNode(QUrl(QStringLiteral("local:replica"))); // connect with remote host node
	mpReplica = repNode->acquireDynamic("DeviceManager");
	mpReplica->setParent(this);
	connect(mpReplica, &QRemoteObjectDynamicReplica::initialized, this, [this]() {
		connect(mpReplica, SIGNAL(deviceAdded(const QUuid &)), SLOT(deviceAdded(const QUuid &)));
		connect(mpReplica, SIGNAL(deviceRemoved(const QUuid &)), SLOT(deviceRemoved(const QUuid &)));
		connect(mpReplica, SIGNAL(deviceInitialized(const QUuid &)), SLOT(deviceInitialized(const QUuid &)));
		connect(mpReplica, SIGNAL(deviceChanged(const QUuid &)), SLOT(deviceChanged(const QUuid &)));

		QTimer::singleShot(0, this, [this]() {
			QRemoteObjectPendingCall deviceIdsResponse;
			QMetaObject::invokeMethod(mpReplica, "getDevices", Q_RETURN_ARG(QRemoteObjectPendingCall, deviceIdsResponse));
			deviceIdsResponse.waitForFinished();
			for(const auto &deviceId : deviceIdsResponse.returnValue().value<QList<QUuid>>()) {
				updateDevice(deviceId);
			}
		});
	});
}

QSharedPointer<OnvifPullPoint> EventManager::getPullPoint(QUuid deviceId) const {

	return QSharedPointer<OnvifPullPoint>(mPullPoints.value(deviceId));
}

void EventManager::addBinding(const QString &rName, const QString &rDescription) {

	auto bindingId = QUuid::createUuid();
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


Result EventManager::bindSource(const QUuid &rBindingId, const QString &rEventSourceType /*= QString()*/,
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

Result EventManager::unbindSource(const QUuid &rBindingId) {

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

Result EventManager::bindHandler(const QUuid &rBindingId, const QString &rEventHandlerType,
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

Result EventManager::unbindHandler(const QUuid &rBindingId) {

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

Result EventManager::triggerHandler(const QUuid &rBindingId) {

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

FutureResult *EventManager::getDeviceTopics(const QUuid &rDeviceId) {

	/*
	auto pResult = new FutureResult();
	auto device = DeviceManager::getInstance()->getDevice(rDeviceId);
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
	 */
	return nullptr;
}

void EventManager::initPullPoint(const QUuid &rDeviceId) {
	/*
	auto info = DeviceManager::getInstance()->getDeviceInfo(rDeviceId);
	if(info.mInitialized) {
	  // Check if we have to initialize a pull point
	  QSettings settings;
	  settings.beginGroup("events");
	  auto eventGroup = settings.childGroups();
	  for(int i = 0; i < eventGroup.size(); ++i) {
	    settings.beginGroup(eventGroup.at(i));
	    QUuid eventId = settings.value("id").toUuid();
	    QUuid deviceId = settings.value("deviceId").toUuid();
	    QString filterExpression = settings.value("expression").toString();

	    if(!eventId.isNull() && !deviceId.isNull()) {
	      if(deviceId == rDeviceId) {
	        if(info.mInitialized) {
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
	 */
}

void EventManager::deviceAdded(const QUuid &rAddedDeviceId) {

	updateDevice(rAddedDeviceId);
}

void EventManager::deviceRemoved(const QUuid &rRemovedDeviceId) {

	mEventDevices.remove(rRemovedDeviceId);
}

void EventManager::deviceInitialized(const QUuid &rRemovedDeviceId) {

	qInfo() << "deviceInitialized";
}

void EventManager::deviceChanged(const QUuid &rRemovedDeviceId) {

	updateDevice(rRemovedDeviceId);
}

void EventManager::updateDevice(const QUuid &deviceId) {

	QRemoteObjectPendingCall deviceIdsResponse;
	QMetaObject::invokeMethod(mpReplica, "getEventEndpoint", Q_RETURN_ARG(QRemoteObjectPendingCall, deviceIdsResponse),
	                          Q_ARG(QUuid, deviceId));
	deviceIdsResponse.waitForFinished(1000);
	mEventDevices.insert(deviceId, deviceIdsResponse.returnValue().toUrl());
}

QHash<QString, EventHandlerInfo> EventManager::mRegisteredEventHandler;

QHash<QString, EventSourceInfo> EventManager::mRegisteredEventSources;
