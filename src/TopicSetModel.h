#pragma once
#include "Topics.h"
#include <QUuid>
#include <AbstractListModel.h>


class TopicSetModel : public AbstractListModel {

	Q_OBJECT
	Q_PROPERTY(QUuid deviceId READ getDeviceId WRITE setDeviceId NOTIFY deviceChanged)

 public:
	TopicSetModel(QObject *pParent = nullptr);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QHash<int, QByteArray> roleNames() const;
	QUuid getDeviceId() const;
	void setDeviceId(const QUuid &rDeviceId);

 signals:
	void deviceChanged();

 private:
	Q_DISABLE_COPY(TopicSetModel);

	void sortList();

	QList<Topic> mTopics;
	QUuid mDeviceId;
};
