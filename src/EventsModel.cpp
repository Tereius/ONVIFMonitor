#include "EventsModel.h"
#include "Enums.h"

EventsModel::EventsModel(QObject *pParent /*= nullptr*/) : AbstractListModel(pParent) {}

int EventsModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const {

	return 0;
}

QVariant EventsModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const {

	return QVariant();
}

QHash<int, QByteArray> EventsModel::roleNames() const {

	auto ret = QHash<int, QByteArray>();
	ret.insert(Enums::EndpointRole, "endpoint");
	ret.insert(Enums::NameRole, "name");
	ret.insert(Enums::HostRole, "host");
	ret.insert(Enums::IdRole, "deviceId");
	ret.insert(Qt::DisplayRole, "display");
	ret.insert(Enums::InitializedRole, "initialized");
	ret.insert(Enums::ErrorRole, "error");
	return ret;
}
