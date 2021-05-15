#pragma once
#include "AbstractListModel.h"
#include "MediaProfile.h"
#include <QUuid>


class MediaProfilesModel : public AbstractListModel {

	Q_OBJECT
	Q_PROPERTY(QUuid deviceId READ getDeviceId WRITE setDeviceId NOTIFY deviceChanged)

 public:
	MediaProfilesModel(QObject *pParent = nullptr);
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QHash<int, QByteArray> roleNames() const;
	QUuid getDeviceId() const;
	void setDeviceId(const QUuid &rDeviceId);

 signals:
	void deviceChanged();

 private:
	Q_DISABLE_COPY(MediaProfilesModel);

	void sortList();

	QList<MediaProfile> mProfiles;
	QUuid mDeviceId;
};
