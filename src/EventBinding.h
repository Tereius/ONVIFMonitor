#pragma once
#include "Result.h"
#include <QUuid>
#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QVariantMap>


class EventHandler;
class EventSource;
class NotificationFilter;


class EventBinding : public QObject {

	Q_OBJECT
	Q_PROPERTY(QUuid bindingId READ getId)
	Q_PROPERTY(BindingStatus status READ getStatus)
	Q_PROPERTY(QString name READ getName)
	Q_PROPERTY(QString description READ getDescription)

 public:
	enum BindingStatus {

		EVENT_UNBOUND
	};
	Q_ENUM(BindingStatus)

	EventBinding(QObject *pParent = nullptr);
	BindingStatus getStatus() const;
	void setStatus(BindingStatus status);
	QString getName() const { return mName; }
	void setName(const QString &val) { mName = val; }
	QString getDescription() const { return mDescription; }
	void setDescription(const QString &val) { mDescription = val; }
	QUuid getId() const { return mId; }
	void setId(QUuid id) { mId = id; }
	bool hasSource() const { return !getSource().isNull(); }
	bool hasHandler() const { return !getHandler().isNull(); }
	QSharedPointer<EventSource> getSource() const { return mSource; }
	QSharedPointer<EventHandler> getHandler() const { return mHandler; }
	Result createSource(int sourceTypeId, const QVariantMap &rProperties = QVariantMap());
	void removeSource();
	Result createHandler(int handlerTypeId, const QVariantMap &rProperties = QVariantMap());
	void removeHandler();
	Result bind();
	Result unbind();
	//! Manually invoke the event handler
	Result triggerHandler();

 signals:
	void eventOccured(const QUuid &rBindingId);

 private:
	Q_DISABLE_COPY(EventBinding);

	BindingStatus mStatus;
	QSharedPointer<EventHandler> mHandler;
	QSharedPointer<EventSource> mSource;
	QString mName;
	QString mDescription;
	QUuid mId;
};
