#include "EventBinding.h"
#include "EventHandler.h"
#include "EventSource.h"
#include <QMetaProperty>
#include <QMetaType>


EventBinding::EventBinding(QObject *pParent /*= nullptr*/) : QObject(pParent), mStatus(EVENT_UNBOUND), mHandler(), mSource() {}

EventBinding::BindingStatus EventBinding::getStatus() const {

	return mStatus;
}

void EventBinding::setStatus(BindingStatus status) {

	mStatus = status;
}

Result EventBinding::createSource(int sourceTypeId, const QVariantMap &rProperties /*= QVariantMap()*/) {

	Result result;
	QMetaType metaType(sourceTypeId);
	if(auto metaObj = metaType.metaObject()) {
		if(metaObj->constructorCount() > 0) {
			if(auto instance = metaObj->newInstance()) {
				if(auto eventSource = dynamic_cast<EventSource *>(instance)) {
					qInfo() << "Creating event source:" << metaObj->className();
					for(auto iter = rProperties.begin(); iter != rProperties.end(); ++iter) {
						bool propSet = false;
						for(int i = 0; i < metaObj->propertyCount(); i++) {
							if(metaObj->property(i).name() == iter.key()) {
								if(metaObj->property(i).isUser()) {
									auto written = metaObj->property(i).write(eventSource, iter.value());
									if(written) {
										propSet = true;
										qInfo() << "    property" << iter.key() << "(" << iter.value() << ")";
									}
								} else {
									qWarning() << "Wouldn't write to non user property:" << iter.key();
								}
							}
						}
						if(!propSet) {
							result = Result(Result::FAULT, tr("Couldn't set property %1").arg(iter.key()));
							qWarning() << result;
							delete instance;
							break;
						}
					}
					if(result) mSource.reset(eventSource);
				} else {
					result = Result(Result::FAULT, tr("Invalid source event type"));
					qWarning() << result;
					delete instance;
				}
			} else {
				result = Result(Result::FAULT, tr("Couldn't instantiate source event type"));
				qWarning() << result;
			}
		} else {
			result = Result(Result::FAULT, tr("Couldn't create source event type"));
			qWarning() << result;
		}
	} else {
		result = Result(Result::FAULT, tr("Unknown source event type"));
		qWarning() << result;
	}
	return result;
}

void EventBinding::removeSource() {

	unbind();
	if(mSource && mSource->metaObject()) qInfo() << "Removing event source:" << mSource->metaObject()->className();
	mSource.clear();
}

Result EventBinding::createHandler(int handlerTypeId, const QVariantMap &rProperties /*= QVariantMap()*/) {

	Result result;
	QMetaType metaType(handlerTypeId);
	if(auto metaObj = metaType.metaObject()) {
		if(metaObj->constructorCount() > 0) {
			if(auto instance = metaObj->newInstance()) {
				if(auto eventHandler = dynamic_cast<EventHandler *>(instance)) {
					qInfo() << "Creating event handler:" << metaObj->className();
					for(auto iter = rProperties.begin(); iter != rProperties.end(); ++iter) {
						bool propSet = false;
						for(int i = 0; i < metaObj->propertyCount(); i++) {
							if(metaObj->property(i).name() == iter.key()) {
								if(metaObj->property(i).isUser()) {
									auto written = metaObj->property(i).write(eventHandler, iter.value());
									if(written) {
										propSet = true;
										qInfo() << "    property" << iter.key() << "(" << iter.value() << ")";
									}
								} else {
									qWarning() << "Wouldn't write to non user property:" << iter.key();
								}
							}
						}
						if(!propSet) {
							result = Result(Result::FAULT, tr("Couldn't set property %1").arg(iter.key()));
							qWarning() << result;
							delete instance;
							break;
						}
					}
					if(result) mHandler.reset(eventHandler);
				} else {
					result = Result(Result::FAULT, tr("Invalid source event type"));
					qWarning() << result;
					delete instance;
				}
			} else {
				result = Result(Result::FAULT, tr("Couldn't instantiate source event type"));
				qWarning() << result;
			}
		} else {
			result = Result(Result::FAULT, tr("Couldn't create source event type"));
			qWarning() << result;
		}
	} else {
		result = Result(Result::FAULT, tr("Unknown source event type"));
		qWarning() << result;
	}
	return result;
}

void EventBinding::removeHandler() {

	unbind();
	if(mHandler && mHandler->metaObject()) qInfo() << "Removing event handler:" << mHandler->metaObject()->className();
	mHandler.clear();
}

Result EventBinding::bind() {

	Result result;
	if(!hasSource()) {
		result = Result(Result::FAULT, tr("Couldn't bind. Missing event source."));
		qWarning() << result;
		return result;
	}

	if(!hasHandler()) {
		result = Result(Result::FAULT, tr("Couldn't bind. Missing event handler."));
		qWarning() << result;
		return result;
	}

	qInfo() << "Establishing binding" << mSource->getName() << "----->" << mHandler->getName();
	connect(mSource.data(), &EventSource::trigger, this, [this](const QVariantMap &rData) {
		emit eventOccured(mId);
		mHandler->gotTriggered(rData);
	});
	mSource->start();

	return result;
}

Result EventBinding::unbind() {

	if(mSource && mHandler) {
		qInfo() << "Destroying binding" << mSource->getName() << "--/-->" << mHandler->getName();
		mSource->stop();
		disconnect(mSource.data(), 0, this, 0);
	}
	return Result::OK;
}

Result EventBinding::triggerHandler() {

	if(mHandler) {
		emit eventOccured(mId);
		mHandler->gotTriggered(QVariantMap());
		return Result::OK;
	}
	return Result(Result::FAULT, tr("Couldn't invoke non existing event handler."));
}
