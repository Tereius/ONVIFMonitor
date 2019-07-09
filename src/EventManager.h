#pragma once
#include "EventHandler.h"
#include "EventSource.h"
#include "Uuid.h"
#include <QHash>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QSharedPointer>


#define EventM EventManager::getGlobal()

class FutureResult;
class EventBinding;
class OnvifPullPoint;

class EventManager : public QObject {

	Q_OBJECT

public:
	EventManager(QObject *pParent = nullptr);
	static EventManager *getGlobal();
	template<typename T>
	static void registerEventHandler();
	template<typename T>
	static void registerEventSource();
	static const QHash<QString, EventHandlerInfo> getRegisteredEventHandler() { return mRegisteredEventHandler; }
	static const QHash<QString, EventSourceInfo> getRegisteredEventSources() { return mRegisteredEventSources; }
	QSharedPointer<OnvifPullPoint> getPullPoint(Uuid deviceId) const;

	Q_INVOKABLE void addBinding(const QString &rName, const QString &rDescription);
	Q_INVOKABLE Result bindSource(const Uuid &rBindingId, const QString &rEventSourceType, const QVariantMap &rProperties = QVariantMap());
	Q_INVOKABLE Result unbindSource(const Uuid &rBindingId);
	Q_INVOKABLE Result bindHandler(const Uuid &rBindingId, const QString &rEventHandlerType, const QVariantMap &rProperties = QVariantMap());
	Q_INVOKABLE Result unbindHandler(const Uuid &rBindingId);
	Q_INVOKABLE Result triggerHandler(const Uuid &rBindingId);


	Q_INVOKABLE void unbind(const Uuid &rBindingId) {}
	Q_INVOKABLE bool doesBindingNameExist(const QString &rName);
	Q_INVOKABLE void bindEvents();
	Q_INVOKABLE void getEventSourceParams() {}

	Q_INVOKABLE void initialize();
	Q_INVOKABLE FutureResult *getDeviceTopics(const Uuid &rDeviceId);

	const QHash<Uuid, QSharedPointer<EventBinding>> &getEventBindings() const { return mInstalledEventBindings; }
	QSharedPointer<EventBinding> getEventBinding(const Uuid &rBindingId) const { return mInstalledEventBindings.value(rBindingId); }

signals:
	//! Emitted if a pull point vanishes unexpectedly
	void lostPullPoint(const Uuid &rDeviceId);
	void eventBindingAdded(const Uuid &rBindingId);
	void eventBindingRemoved(const Uuid &rBindingId);
	void eventBindingChanged(const Uuid &rBindingId);
	void eventOccured(const Uuid &rBindingId);

private slots:
	//! (re)initialize a pull point for a device
	void initPullPoint(const Uuid &rDeviceId);

private:
	Q_DISABLE_COPY(EventManager);

	void initEvents();

	static QHash<QString, EventHandlerInfo> mRegisteredEventHandler;
	static QHash<QString, EventSourceInfo> mRegisteredEventSources;
	QHash<Uuid, QSharedPointer<EventBinding>> mInstalledEventBindings; // binding id -> binding
	QHash<Uuid, OnvifPullPoint *> mPullPoints; // device id -> pullpoint
	QMutex mMutex;
};

template<typename T>
void EventManager::registerEventHandler() {

	auto pHandler = new T;
	if(auto pHandlerCast = dynamic_cast<EventHandler *>(pHandler)) {
		int id = qMetaTypeId<T *>();
		auto typeName = QVariant::typeToName(id);
		EventHandlerInfo info;
		info.setTypeId(id);
		info.setName(pHandler->getName());
		info.setDescription(pHandler->getDescription());
		info.setCanonicalName(typeName);
		auto metaObj = pHandlerCast->metaObject();
		if(metaObj && metaObj->constructorCount() > 0) {
			QList<PropertyInfo> properties;
			for(auto i = 0; i < metaObj->propertyCount(); i++) {
				auto prop = metaObj->property(i);
				if(prop.isValid() && prop.isUser()) {
					PropertyInfo propInfo;
					propInfo.setName(prop.name());
					propInfo.setType(PropertyInfo::getTypeForQTypeId(prop.type()));
					propInfo.setDefaultValue(prop.read(pHandler));
					properties.push_back(propInfo);
					qDebug() << "    Found event handler property" << propInfo.getName();
				}
			}
			info.setProperties(properties);
			mRegisteredEventHandler.insert(typeName, info);
			qDebug() << "Registered event handler" << typeName << "(" << id << ")";
		} else {
			qWarning() << "Couldn't register event handler" << typeName << "(" << id << "):"
			           << "The ctor is not invokable!";
		}
	} else {
		qWarning() << "Couldn't register event handler: Unknown type";
	}
	delete pHandler;
}

template<typename T>
void EventManager::registerEventSource() {

	auto pSource = new T;
	if(auto pHandlerCast = dynamic_cast<EventSource *>(pSource)) {
		int id = qMetaTypeId<T *>();
		auto typeName = QVariant::typeToName(id);
		EventSourceInfo info;
		info.setTypeId(id);
		info.setName(pSource->getName());
		info.setDescription(pSource->getDescription());
		info.setCanonicalName(typeName);
		auto metaObj = pHandlerCast->metaObject();
		if(metaObj && metaObj->constructorCount() > 0) {
			QList<PropertyInfo> properties;
			for(auto i = 0; i < metaObj->propertyCount(); i++) {
				auto prop = metaObj->property(i);
				if(prop.isValid() && prop.isUser()) {
					PropertyInfo propInfo;
					propInfo.setName(prop.name());
					propInfo.setType(PropertyInfo::getTypeForQTypeId(prop.type()));
					propInfo.setDefaultValue(prop.read(pSource));
					properties.push_back(propInfo);
					qDebug() << "    Found event source property" << propInfo.getName();
				}
			}
			info.setProperties(properties);
			mRegisteredEventSources.insert(typeName, info);
			qDebug() << "Registered event source" << typeName << "(" << id << ")";
		} else {
			qWarning() << "Couldn't register event source" << typeName << "(" << id << "):"
			           << "The ctor is not invokable!";
		}
	} else {
		qWarning() << "Couldn't register event source: Unknown type";
	}
	delete pSource;
}
