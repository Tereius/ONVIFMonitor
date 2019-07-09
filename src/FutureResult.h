#pragma once
#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QVariant>
#include <QQmlEngine>
#include <QEvent>
#include <QCoreApplication>


class FutureResult : public QObject {

	Q_OBJECT
		Q_PROPERTY(QVariant result READ getResult NOTIFY resultChanged)

public:

	FutureResult();

	QVariant getResult();
	/*!
	 *
	 * \brief Set the result. Mustn't be called more than one time
	 * 
	 * As soon as this method is called the ownership is taken from cpp and given to qml.
	 * From then on it's not safe to use this object again.
	 * 
	 */
	void resolveResult(const QVariant &rResult);
	void resolveEmptyResult();

	bool event(QEvent *e);

signals:

	void resultChanged(const QVariant &rResult);

private:

	QVariant mResult;
	QMutex mMutex;
};
