#include "EventsModel.h"
#include "Roles.h"

EventsModel::EventsModel(QObject* pParent /*= nullptr*/) : AbstractListModel(pParent) {}

int EventsModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const {

	return 0;
}

QVariant EventsModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const {

	return QVariant();
}

QHash<int, QByteArray> EventsModel::roleNames() const {

	auto ret = QHash<int, QByteArray>();
	ret.insert(Roles::EndpointRole, "endpoint");
	ret.insert(Roles::NameRole, "name");
	ret.insert(Roles::HostRole, "host");
	ret.insert(Roles::IdRole, "deviceId");
	ret.insert(Qt::DisplayRole, "display");
	ret.insert(Roles::InitializedRole, "initialized");
	ret.insert(Roles::ErrorRole, "error");
	return ret;
}
