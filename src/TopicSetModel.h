#pragma once
#include "Topics.h"
#include "Uuid.h"
#include <AbstractListModel.h>


class TopicSetModel : public AbstractListModel {

	Q_OBJECT
	Q_PROPERTY(Uuid deviceId READ getDeviceId WRITE setDeviceId NOTIFY deviceChanged)

public:
	TopicSetModel(QObject *pParent = nullptr);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QHash<int, QByteArray> roleNames() const;
	Uuid getDeviceId() const;
	void setDeviceId(const Uuid &rDeviceId);

signals:
	void deviceChanged();

private:
	Q_DISABLE_COPY(TopicSetModel);

	void sortList();

	QList<Topic> mTopics;
	Uuid mDeviceId;
};
