#pragma once
#include "PropertyInfo.h"
#include "Uuid.h"
#include <QHash>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QVariant>


class OnvifPullPoint;
class QTimer;

class EventSourceInfo {

	Q_GADGET
	Q_PROPERTY(QString name READ getName())
	Q_PROPERTY(QString description READ getDescription())
	Q_PROPERTY(QList<PropertyInfo> properties READ getProperties())

public:
	EventSourceInfo();
	bool isNull() const { return mTypeId <= 0; }
	int getTypeId() const { return mTypeId; }
	void setTypeId(int id) { mTypeId = id; }
	QString getName() const { return mName; }
	void setName(const QString &rName) { mName = rName; }
	QString getDescription() const { return mDescription; }
	void setDescription(const QString &rDescription) { mDescription = rDescription; }
	const QList<PropertyInfo> &getProperties() const { return mProperties; }
	void setProperties(const QList<PropertyInfo> &rProperties) { mProperties = rProperties; }
	QString getCanonicalName() const { return mCanonicalName; }
	void setCanonicalName(const QString &rCanonicalName) { mCanonicalName = rCanonicalName; }

private:
	int mTypeId;
	QString mName;
	QString mDescription;
	QString mCanonicalName;
	QList<PropertyInfo> mProperties;
};


class EventSource : public QObject {

	Q_OBJECT

public:
	EventSource();
	virtual QString getName() const = 0;
	virtual QString getDescription() const = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
	QVariant getPropertyValue(const QString &rName);
	QVariantMap getPropertyValues();

signals:
	void trigger(const QVariantMap &rData = QVariantMap());
};


/*
 *
 * \brief Does nothing
 *
 */
class NullSource : public EventSource {

	Q_OBJECT

public:
	Q_INVOKABLE NullSource();
	virtual QString getName() const override { return QObject::tr("Null"); };
	virtual QString getDescription() const override { return QObject::tr("Does nothing"); }
	virtual void start(){};
	virtual void stop(){};
};


/*
 *
 * \brief Triggered at a fixed interval
 *
 */
class TimerEventSource : public EventSource {

	Q_OBJECT
	Q_PROPERTY(int msInterval MEMBER mInterval USER true)

public:
	Q_INVOKABLE TimerEventSource();
	virtual ~TimerEventSource();
	virtual QString getName() const override { return QObject::tr("Timer event"); }
	virtual QString getDescription() const override { return QObject::tr("An event that is triggered at a fixed interval"); }
	virtual void start();
	virtual void stop();

	int mInterval = 1000;

private:
	QTimer *mpTimer;
};


/*
 *
 * \brief Triggered by every message
 *
 */
class OnvifDeviceMessage : public EventSource {

	Q_OBJECT

public:
	Q_INVOKABLE OnvifDeviceMessage();
	virtual QString getName() const override { return QObject::tr("Onvif Message"); }
	virtual QString getDescription() const override { return QObject::tr("Triggered by every received message"); }
	virtual void start(){};
	virtual void stop(){};

private:
	QSharedPointer<OnvifPullPoint> mPullPoint;
	Uuid mDeviceId;
};


class OnvifFilterMessageExpression {

	Q_GADGET
	Q_PROPERTY(QString filterExpression MEMBER filterExpression)

public:
	bool operator!=(OnvifFilterMessageExpression &rOther) { return filterExpression != rOther.filterExpression; }
	QString filterExpression;
};

/*
 *
 * \brief Triggered by every received message that matches a filter expression
 *
 */
class FilteredOnvifDeviceMessage : public EventSource {

	Q_OBJECT
	Q_PROPERTY(QStringList devices READ getDevices USER true)
	Q_PROPERTY(OnvifFilterMessageExpression topics MEMBER mExpression USER true)

public:
	Q_INVOKABLE FilteredOnvifDeviceMessage();
	virtual QString getName() const override { return QObject::tr("Filtered Onvif Message"); }
	virtual QString getDescription() const override {
		return QObject::tr("Triggered by every received message that matches a filter expression");
	}
	virtual void start(){};
	virtual void stop(){};
	QStringList getTopics();
	QStringList getDevices();

	OnvifFilterMessageExpression mExpression;

private:
	QSharedPointer<OnvifPullPoint> mPullPoint;
	Uuid mDeviceId;
	QString mFilterExpression;
};
