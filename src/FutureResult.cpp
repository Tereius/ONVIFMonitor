#include "FutureResult.h"


class ChangeOwnershipEvent : public QEvent {

public:
	ChangeOwnershipEvent(QQmlEngine::ObjectOwnership ownership) : QEvent(customEventType), mOwnership(ownership) {}

	static QEvent::Type customEventType;
	QQmlEngine::ObjectOwnership getOwnership() const { return mOwnership; }

private:
	QQmlEngine::ObjectOwnership mOwnership;
};

QEvent::Type ChangeOwnershipEvent::customEventType = static_cast<QEvent::Type>(QEvent::registerEventType());


bool FutureResult::event(QEvent *e) {

	if(e->type() == ChangeOwnershipEvent::customEventType) {

		auto pEvent = static_cast<ChangeOwnershipEvent*>(e);
		QQmlEngine::setObjectOwnership(this, pEvent->getOwnership());
		return true;
	}

	return QObject::event(e);
}

FutureResult::FutureResult() :
	QObject() {

	QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
	moveToThread(qApp->thread());
}

QVariant FutureResult::getResult() {

	QMutexLocker lock(&mMutex);
	return mResult;
}

void FutureResult::resolveResult(const QVariant &rResult) {

	mMutex.lock();
	mResult = rResult;
	mMutex.unlock();
	emit resultChanged(mResult);
	qApp->postEvent(this, new ChangeOwnershipEvent(QQmlEngine::JavaScriptOwnership));
}

void FutureResult::resolveEmptyResult() {

	qApp->postEvent(this, new ChangeOwnershipEvent(QQmlEngine::JavaScriptOwnership));
}
