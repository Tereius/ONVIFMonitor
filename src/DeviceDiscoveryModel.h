#pragma once
#include "AbstractListModel.h"
#include "DeviceProbe.h"
#include "DiscoveryMatch.h"
#include <QList>
#include <QPair>


class OnvifDiscovery;

class DeviceDiscoveryModel : public AbstractListModel {

	Q_OBJECT

 public:
	explicit DeviceDiscoveryModel(QObject *pParent = nullptr);
	~DeviceDiscoveryModel() override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;
	Q_INVOKABLE void start();
	Q_INVOKABLE void reset();
	Q_INVOKABLE void stop();

 private:
	Q_DISABLE_COPY(DeviceDiscoveryModel);

	void sortList();

	OnvifDiscovery *mpDiscovery;
	QList<QPair<DiscoveryMatch, DeviceProbe>> mMatches;
};
