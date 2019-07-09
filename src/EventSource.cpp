#include "EventSource.h"
#include "DeviceInfo.h"
#include "DeviceManager.h"
#include "EventManager.h"
#include "OnvifPullPoint.h"
#include <QCoreApplication>
#include <QMetaProperty>
#include <QMetaType>
#include <QTimer>


EventSource::EventSource() {}

QVariant EventSource::getPropertyValue(const QString &rName) {

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

QVariantMap EventSource::getPropertyValues() {

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

NullSource::NullSource() : EventSource() {}

TimerEventSource::TimerEventSource() : EventSource(), mInterval(-1), mpTimer(new QTimer()) {

	connect(mpTimer, &QTimer::timeout, this, [this]() { emit trigger(); });
}

TimerEventSource::~TimerEventSource() {
	delete mpTimer;
}

void TimerEventSource::start() {
	mpTimer->start(mInterval);
}

void TimerEventSource::stop() {
	mpTimer->stop();
}

OnvifDeviceMessage::OnvifDeviceMessage() : EventSource(), mPullPoint(), mDeviceId() {}


FilteredOnvifDeviceMessage::FilteredOnvifDeviceMessage() : EventSource(), mPullPoint(), mDeviceId(), mFilterExpression() {}

QStringList FilteredOnvifDeviceMessage::getTopics() {

	return QStringList();
}

QStringList FilteredOnvifDeviceMessage::getDevices() {

	QStringList ret;
	for(auto di : DeviceM->getDeviceInfos()) {
		ret.push_back(di.getDeviceName());
	}
	return ret;
}

EventSourceInfo::EventSourceInfo() : mTypeId(-1) {}
