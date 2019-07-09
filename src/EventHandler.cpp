#include "EventHandler.h"
#include <QDebug>
#include <QMetaProperty>
#include <QMetaType>


EventHandler::EventHandler() {}

QVariant EventHandler::getPropertyValue(const QString &rName) {

	QVariant ret;
	if(auto metaObj = metaObject()) {
		for(auto i = 0; i < metaObj->propertyCount(); i++) {
			auto metaProperty = metaObj->property(i);
			if(metaProperty.isUser(this) && metaProperty.name() == rName) {
				if(metaProperty.isReadable()) {
					ret = metaProperty.read(this);
				} else {
					qWarning() << "Couldn't read property:" << metaProperty.name();
				}
				break;
			}
		}
	}
	return ret;
}

QVariantMap EventHandler::getPropertyValues() {

	QVariantMap ret;
	if(auto metaObj = metaObject()) {
		for(auto i = 0; i < metaObj->propertyCount(); i++) {
			auto metaProperty = metaObj->property(i);
			if(metaProperty.isUser(this)) {
				if(metaProperty.isReadable()) {
					ret.insert(metaProperty.name(), metaProperty.read(this));
				} else {
					qWarning() << "Couldn't read property:" << metaProperty.name();
				}
			}
		}
	}
	return ret;
}

LogEventHandler::LogEventHandler() : EventHandler() {}

void LogEventHandler::handle(const QVariantMap &rData) {

	qInfo() << mLogMessage;
}

EventHandlerInfo::EventHandlerInfo() : mTypeId(-1) {}
