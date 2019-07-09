#pragma once
#include "PropertyInfo.h"
#include "Result.h"
#include <QObject>
#include <QString>


class EventHandlerInfo {
	Q_GADGET

public:
	EventHandlerInfo();
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


class EventHandler : public QObject {

	Q_OBJECT

public:
	Q_INVOKABLE EventHandler();
	virtual QString getName() const { return tr("Dummy"); };
	virtual QString getDescription() const { return tr("Does nothing"); };
	virtual Result run() { return Result::OK; }
	QVariant getPropertyValue(const QString &rName);
	QVariantMap getPropertyValues();
	virtual void handle(const QVariantMap &rData){};

public slots:
	void gotTriggered(const QVariantMap &rData) { handle(rData); }
};


class LogEventHandler : public EventHandler {

	Q_OBJECT
	Q_PROPERTY(QString message MEMBER mLogMessage USER true)

public:
	Q_INVOKABLE LogEventHandler();
	virtual QString getName() const { return tr("Log Event"); }
	virtual QString getDescription() const { return tr("Writes a log message"); }
	virtual void handle(const QVariantMap &rData);

	QString mLogMessage = QString();
};
