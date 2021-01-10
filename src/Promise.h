#pragma once
#include <QObject>
#include <QJSValue>
#include <QJSEngine>
#include "Window.h"
#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <QThread>
#include <QQmlEngine>
#include <QUuid>

class PromiseBase {

	Q_GADGET
	Q_PROPERTY(QUuid id MEMBER mId)

public:
	PromiseBase() : mId(), mCurrentDepth(0), mChainCount(0) {}
	explicit PromiseBase(const QUuid &id) : mId(id), mCurrentDepth(0), mChainCount(0) {}

	PromiseBase(const PromiseBase &other) :
	 mId(other.mId),
	 mCurrentDepth(other.mCurrentDepth),
	 mChainCount(other.mChainCount){

	 };

	PromiseBase &operator=(const PromiseBase &other) {

		if(&other == this) return *this;
		mId = other.mId;
		mCurrentDepth = other.mCurrentDepth;
		mChainCount = other.mChainCount;
		return *this;
	};

	Q_INVOKABLE PromiseBase then(QJSValue onFulfilledCallback, QJSValue onRejectedCallback = {QJSValue::UndefinedValue}) {
		Q_ASSERT(QThread::currentThread() == qApp->thread());
		//	mOnRejectedCallback = onRejectedCallback;
		//	mOnFulfilledCallback = onFulfilledCallback;
		auto engine = qjsEngine(Window::getGlobal());
		auto globalObject = engine->globalObject();
		// Save the js value in the global object so the garbage collection won't delete it
		globalObject.setProperty("FutureResult_reject_" + mId.toString(QUuid::WithoutBraces) + "_" + mChainCount, onRejectedCallback);
		globalObject.setProperty("FutureResult_fulfill_" + mId.toString(QUuid::WithoutBraces) + "_" + mChainCount, onFulfilledCallback);
		mChainCount++;
		return *this;
	};

	Q_INVOKABLE PromiseBase error(QJSValue onRejectedCallback) { return then(QJSValue::NullValue, onRejectedCallback); };

public:
	QUuid mId;
	qint32 mCurrentDepth;
	qint32 mChainCount;
};

template<class T>
class Promise : public PromiseBase {

public:
	Promise() : PromiseBase(QUuid::createUuid()) {}
	//! Creates a Promise that is resolved immediately
	explicit Promise(T result) : PromiseBase(QUuid::createUuid()) { resolve(result); }

	Promise(const Promise &other) :
	 PromiseBase(other.mId){

	 };

	Promise &operator=(const Promise &other) {

		if(&other == this) return *this;
		PromiseBase::operator=(other);
		mChainCount = other.mChainCount;
		return *this;
	};

	/*!
	 * Notify the receiver that the task finished successful. Once this method is called all following calls of resolve or reject have no
	 * effect.
	 * \param result Provide the result of the task
	 */
	void resolve(T result) const {

		auto id = mId;
		auto depth = mCurrentDepth;
		// Will always be called by main thread
		QTimer::singleShot(0, qApp, [id, depth, result]() {
			auto engine = qjsEngine(Window::getGlobal());
			auto globalObject = engine->globalObject();
			auto usedId = id;
			auto usedDepth = depth;
			if(globalObject.hasProperty("FutureResult_forward_" + id.toString(QUuid::WithoutBraces))) {
				usedId = QUuid(globalObject.property("FutureResult_forward_" + id.toString(QUuid::WithoutBraces)).toString());
				globalObject.deleteProperty("FutureResult_forward_" + id.toString(QUuid::WithoutBraces));
			}
			if(globalObject.hasProperty("FutureResult_forwardDepth_" + id.toString(QUuid::WithoutBraces))) {
				usedDepth = globalObject.property("FutureResult_forwardDepth_" + id.toString(QUuid::WithoutBraces)).toInt();
				globalObject.deleteProperty("FutureResult_forwardDepth_" + id.toString(QUuid::WithoutBraces));
			}
			auto callback = globalObject.property("FutureResult_fulfill_" + usedId.toString(QUuid::WithoutBraces) + "_" + usedDepth);
			if(callback.isCallable()) {
				auto returnValue = callback.call({engine->toScriptValue<T>(result)});
				if(!returnValue.isError()) {
					auto chainedId = QUuid(returnValue.property("id").toString());
					// TODO: Detect the base class my the property is discouraged.
					if(!chainedId.isNull()) {
						globalObject.setProperty("FutureResult_forward_" + chainedId.toString(QUuid::WithoutBraces), usedId.toString(QUuid::WithoutBraces));
						globalObject.setProperty("FutureResult_forwardDepth_" + chainedId.toString(QUuid::WithoutBraces), usedDepth + 1);
					}
				} else {
					qWarning() << "Uncaught exception at line" << returnValue.property("lineNumber").toInt() << ":" << returnValue.toString();
					PromiseBase error(QUuid::createUuid());
				}
			}
			globalObject.deleteProperty("FutureResult_fulfill_" + usedId.toString(QUuid::WithoutBraces) + "_" + usedDepth);
			globalObject.deleteProperty("FutureResult_reject_" + usedId.toString(QUuid::WithoutBraces) + "_" + usedDepth);
		});
	}

	/*!
	 * Notify the receiver that the task finished as erroneous. Once this method is called all following calls of reject or resolve have no
	 * effect.
	 * \param cause A short informative description why the task failed
	 */
	void reject(const QString &cause) const {

		auto id = mId;
		auto depth = mCurrentDepth;
		// Will always be called by main thread
		QTimer::singleShot(0, qApp, [id, depth, cause]() {
			auto engine = qjsEngine(Window::getGlobal());
			auto globalObject = engine->globalObject();
			auto callback = globalObject.property("FutureResult_reject_" + id.toString(QUuid::WithoutBraces) + "_" + depth);
			if(callback.isCallable()) {
				callback.call({QJSValue(cause)});
			}
			globalObject.deleteProperty("FutureResult_reject_" + id.toString(QUuid::WithoutBraces) + "_" + depth);
			globalObject.deleteProperty("FutureResult_fulfill_" + id.toString(QUuid::WithoutBraces) + "_" + depth);
		});
	}
};

Q_DECLARE_METATYPE(Promise<QString>)
Q_DECLARE_METATYPE(Promise<bool>)
