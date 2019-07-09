#pragma once
#include "AbstractListModel.h"
#include "DiscoveryMatch.h"
#include <QList>


class OnvifDiscovery;

class DeviceDiscoveryModel : public AbstractListModel {

	Q_OBJECT

public:
	DeviceDiscoveryModel(QObject* pParent = nullptr);
	virtual ~DeviceDiscoveryModel();
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QHash<int, QByteArray> roleNames() const;
	Q_INVOKABLE void start();
	Q_INVOKABLE void reset();
	Q_INVOKABLE void stop();

private:
	Q_DISABLE_COPY(DeviceDiscoveryModel);

	OnvifDiscovery* mpDiscovery;
	QList<DiscoveryMatch> mMatches;
};
