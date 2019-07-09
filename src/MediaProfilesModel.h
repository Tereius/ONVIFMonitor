#pragma once
#include "AbstractListModel.h"
#include "MediaProfile.h"
#include "Uuid.h"


class MediaProfilesModel : public AbstractListModel {

	Q_OBJECT
	Q_PROPERTY(Uuid deviceId READ getDeviceId WRITE setDeviceId NOTIFY deviceChanged)

public:
	MediaProfilesModel(QObject* pParent = nullptr);
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QHash<int, QByteArray> roleNames() const;
	Uuid getDeviceId() const;
	void setDeviceId(const Uuid& rDeviceId);

signals:
	void deviceChanged();

private:
	Q_DISABLE_COPY(MediaProfilesModel);

	void sortList();

	QList<MediaProfile> mProfiles;
	Uuid mDeviceId;
};
