#include "DevicesModel.h"
#include "DeviceManager.h"
#include "Roles.h"
#include <algorithm>


DevicesModel::DevicesModel(QObject *pParent /*= nullptr*/) : AbstractListModel(pParent), mMatches(DeviceM->getDeviceInfos()) {

	// Sort
	sortList();

	connect(DeviceM, &DeviceManager::deviceAdded, this,
	        [this](const Uuid &rDeviceId) {
		        auto addedDevice = DeviceM->getDeviceInfo(rDeviceId);
		        auto index = 0;
		        for(int i = 0; i < mMatches.size(); ++i) {
			        if(mMatches.at(i).getDeviceName() >= addedDevice.getDeviceName()) {
				        index = i;
				        break;
			        }
		        }
		        beginInsertRows(QModelIndex(), index, index);
		        mMatches.insert(index, addedDevice);
		        endInsertRows();
	        },
	        Qt::QueuedConnection);

	connect(DeviceM, &DeviceManager::deviceRemoved, this,
	        [this](const Uuid &rDeviceId) {
		        auto foundIndex = -1;
		        for(auto i = 0; i < mMatches.size(); ++i) {
			        if(mMatches.at(i).getDeviceId() == rDeviceId) {
				        foundIndex = i;
				        break;
			        }
		        }
		        if(foundIndex >= 0) {
			        beginRemoveRows(QModelIndex(), foundIndex, foundIndex);
			        mMatches.removeAt(foundIndex);
			        endRemoveRows();
		        }
	        },
	        Qt::QueuedConnection);

	connect(DeviceM, &DeviceManager::deviceChanged, this,
	        [this](const Uuid &rDeviceId) {
		        beginResetModel();
		        mMatches = DeviceM->getDeviceInfos();
		        sortList();
		        endResetModel();
	        },
	        Qt::QueuedConnection);
}

DevicesModel::~DevicesModel() {}

int DevicesModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const {

	return mMatches.size();
}

QVariant DevicesModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const {

	auto row = index.row();
	auto column = index.column();
	auto ret = QVariant();

	if(index.isValid() && column == 0 && mMatches.size() > row) {
		switch(role) {
			case Roles::NameRole:
			case Qt::DisplayRole:
				ret = mMatches.at(row).getDeviceName();
				break;
			case Roles::EndpointRole:
				ret = mMatches.at(row).getEndpoint();
				break;
			case Roles::HostRole:
				ret = mMatches.at(row).getEndpoint().host() + ":" + QString::number(mMatches.at(row).getEndpoint().port());
				break;
			case Roles::IdRole:
				ret = QVariant::fromValue(mMatches.at(row).getDeviceId());
				break;
			case Roles::InitializedRole:
				ret = mMatches.at(row).isInitialized();
				break;
			case Roles::ErrorRole:
				ret = mMatches.at(row).getError();
				break;
			default:
				ret = QVariant();
				break;
		}
	}
	return ret;
}

QHash<int, QByteArray> DevicesModel::roleNames() const {

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

void DevicesModel::sortList() {

	std::sort(mMatches.begin(), mMatches.end(),
	          [](DeviceInfo left, DeviceInfo right) { return left.getDeviceName().compare(right.getDeviceName(), Qt::CaseInsensitive); });
}
