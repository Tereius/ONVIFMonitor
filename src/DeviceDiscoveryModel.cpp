#include "DeviceDiscoveryModel.h"
#include "DeviceInfo.h"
#include "DeviceManager.h"
#include "OnvifDiscovery.h"
#include "Roles.h"
#include <QHash>
#include <QMutexLocker>


DeviceDiscoveryModel::DeviceDiscoveryModel(QObject* pParent /*= nullptr*/) :
 AbstractListModel(pParent),
 mpDiscovery(new OnvifDiscovery({}, {"tdn:NetworkVideoTransmitter"}, this)),
 mMatches() {

	connect(mpDiscovery, &OnvifDiscovery::NewMatch, this,
	        [this](const DiscoveryMatch& rMatch) {
		        int oldSize = this->mMatches.size();
		        this->beginInsertRows(QModelIndex(), oldSize, oldSize);
		        this->mMatches << rMatch;
		        this->endInsertRows();
	        },
	        Qt::QueuedConnection);

	connect(DeviceM, &DeviceManager::deviceAdded, this,
	        [this](const Uuid& rDeviceId) {
		        emit dataChanged(index(0), index(mMatches.size() - 1), {Roles::IsNewRole, Qt::DisplayRole, Roles::HostRole});
	        },
	        Qt::QueuedConnection);

	connect(DeviceM, &DeviceManager::deviceRemoved, this,
	        [this](const Uuid& rDeviceId) {
		        emit dataChanged(index(0), index(mMatches.size() - 1), {Roles::IsNewRole, Qt::DisplayRole, Roles::HostRole});
	        },
	        Qt::QueuedConnection);

	connect(DeviceM, &DeviceManager::deviceChanged, this,
	        [this](const Uuid& rDeviceId) {
		        emit dataChanged(index(0), index(mMatches.size() - 1), {Roles::IsNewRole, Qt::DisplayRole, Roles::HostRole});
	        },
	        Qt::QueuedConnection);
}

DeviceDiscoveryModel::~DeviceDiscoveryModel() {

	mpDiscovery->Stop();
}

int DeviceDiscoveryModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const {

	return mMatches.size();
}

QVariant DeviceDiscoveryModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const {

	auto row = index.row();
	auto column = index.column();
	auto ret = QVariant();

	if(index.isValid() && column == 0 && mMatches.size() > row) {

		auto endpoint = mMatches.at(row).GetDeviceEndpoint();
		auto endpointRef = mMatches.at(row).GetEndpointReference();
		auto host = endpoint.host();
		auto port = endpoint.port();

		auto deviceName = mMatches.at(row).GetDeviceEndpoint().host() + ":" + QString::number(mMatches.at(row).GetDeviceEndpoint().port());
		auto isNew = true;

		if(role == Qt::DisplayRole || role == Roles::HostRole || Roles::IsNewRole) {

			auto deviceInfo = DeviceM->getDeviceInfoByEndpointRef(endpointRef);

			if(deviceInfo.isNull()) {
				deviceInfo = DeviceM->getDeviceInfoByHost(host, port);
			}

			if(!deviceInfo.isNull()) {
				deviceName = QString("%1 (%2)").arg(deviceName).arg(deviceInfo.getDeviceName());
				isNew = false;
			}
		}

		switch(role) {
			case Roles::HostRole:
			case Qt::DisplayRole:
				ret = deviceName;
				break;
			case Roles::EndpointRole:
				ret = mMatches.at(row).GetDeviceEndpoint();
				break;
			case Roles::IsNewRole:
				ret = isNew;
				break;
			default:
				ret = QVariant();
				break;
		}
	}
	return ret;
}

QHash<int, QByteArray> DeviceDiscoveryModel::roleNames() const {

	auto ret = QHash<int, QByteArray>();
	ret.insert(Roles::EndpointRole, "endpoint");
	ret.insert(Roles::HostRole, "host");
	ret.insert(Qt::DisplayRole, "display");
	ret.insert(Roles::IsNewRole, "isNew");
	return ret;
}

void DeviceDiscoveryModel::start() {

	mpDiscovery->Start();
}

void DeviceDiscoveryModel::stop() {

	mpDiscovery->Stop();
}

void DeviceDiscoveryModel::reset() {

	beginResetModel();
	stop();
	mMatches.clear();
	mpDiscovery->ClearMatches();
	endResetModel();
}
