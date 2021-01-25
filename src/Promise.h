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

	Q_INVOKABLE PromiseBase then(QJSValue onFulfilledCallback = {QJSValue::UndefinedValue},
	                             QJSValue onRejectedCallback = {QJSValue::UndefinedValue}) {
		Q_ASSERT(QThread::currentThread() == qApp->thread());
		//	mOnRejectedCallback = onRejectedCallback;
		//	mOnFulfilledCallback = onFulfilledCallback;
		auto engine = qjsEngine(Window::getGlobal());
		auto globalObject = engine->globalObject();
		// Save the js value in the global object so the garbage collection won't delete it
		globalObject.setProperty("FutureResult_reject_" + mId.toString(QUuid::WithoutBraces) + "_" + mCurrentDepth, onRejectedCallback);
		globalObject.setProperty("FutureResult_fulfill_" + mId.toString(QUuid::WithoutBraces) + "_" + mCurrentDepth, onFulfilledCallback);

		auto childId = QUuid::createUuid();
		globalObject.setProperty("FutureResult_child_" + mId.toString(QUuid::WithoutBraces) + "_" + mCurrentDepth,
		                         childId.toString(QUuid::WithoutBraces));
		globalObject.setProperty("FutureResult_depth_" + mId.toString(QUuid::WithoutBraces), mCurrentDepth + 1);

		mCurrentDepth++;
		return PromiseBase(childId);
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

	static void _resolve(const QUuid &id, const QJSValue &result) {

		auto engine = qjsEngine(Window::getGlobal());
		auto globalObject = engine->globalObject();

		if(globalObject.hasProperty("FutureResult_depth_" + id.toString(QUuid::WithoutBraces))) {
			auto depth = globalObject.property("FutureResult_depth_" + id.toString(QUuid::WithoutBraces)).toInt();
			for(auto i = 0; i < depth; i++) {
				auto callback = globalObject.property("FutureResult_fulfill_" + id.toString(QUuid::WithoutBraces) + "_" + i);
				if(callback.isCallable()) {
					auto returnValue = callback.call({result});
					if(!returnValue.isError()) {
						auto promise2 =
						 QUuid::fromString(globalObject.property("FutureResult_child_" + id.toString(QUuid::WithoutBraces) + "_" + i).toString());
						if(returnValue.hasProperty("then")) {
							// A thenable was returned
							auto x = QUuid(returnValue.property("id").toString());
							if(promise2 != x) {

							} else {
								engine->throwError(
								 QJSValue::TypeError,
								 QObject::tr("The Promise Resolution Procedure failed because the returned promises refers the same object"));
							}
						} else {
							_resolve(promise2, returnValue);
						}
					} else {
						// An exception was thrown
						qWarning() << "Uncaught exception at line" << returnValue.property("lineNumber").toInt() << ":" << returnValue.toString();
						auto promise2 =
						 QUuid::fromString(globalObject.property("FutureResult_child_" + id.toString(QUuid::WithoutBraces) + "_" + i).toString());
						_reject(promise2, returnValue.toString());
					}
				} else {
					auto promise2 =
					 QUuid::fromString(globalObject.property("FutureResult_child_" + id.toString(QUuid::WithoutBraces) + "_" + i).toString());
					_resolve(promise2, result);
				}
				globalObject.deleteProperty("FutureResult_fulfill_" + id.toString(QUuid::WithoutBraces) + "_" + i);
				globalObject.deleteProperty("FutureResult_reject_" + id.toString(QUuid::WithoutBraces) + "_" + i);
			}
		}
	}

	/*!
	 * Notify the receiver that the task finished successful. Once this method is called all following calls of resolve or reject have no
	 * effect.
	 * \param result Provide the result of the task
	 */
	void resolve(T result) const {

		auto id = mId;
		auto depth = mCurrentDepth;
		// Will always be called by main thread
		QTimer::singleShot(0, qApp, [id, result, this]() {
			auto engine = qjsEngine(Window::getGlobal());
			_resolve(id, engine->toScriptValue<T>(result));
		});
	}

	static void _reject(const QUuid &id, const QString &cause) {

		auto engine = qjsEngine(Window::getGlobal());
		auto globalObject = engine->globalObject();
		auto callback = globalObject.property("FutureResult_reject_" + id.toString(QUuid::WithoutBraces) + "_");
		if(callback.isCallable()) {
			callback.call({QJSValue(cause)});
		}
		globalObject.deleteProperty("FutureResult_reject_" + id.toString(QUuid::WithoutBraces) + "_");
		globalObject.deleteProperty("FutureResult_fulfill_" + id.toString(QUuid::WithoutBraces) + "_");
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
