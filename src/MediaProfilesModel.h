#pragma once
#include "AbstractListModel.h"
#include "MediaProfile.h"
#include <QUuid>
#include <QtQmlIntegration>


class MediaProfilesModel : public AbstractListModel {

	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(QUuid deviceId READ getDeviceId WRITE setDeviceId NOTIFY deviceChanged)

 public:
	explicit MediaProfilesModel(QObject *pParent = nullptr);
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;
	QUuid getDeviceId() const;
	void setDeviceId(const QUuid &rDeviceId);

 signals:
	void deviceChanged();

 private:
	Q_DISABLE_COPY(MediaProfilesModel);

	QList<MediaProfile> mProfiles;
	QUuid mDeviceId;
};
