#include "DevicesModel.h"
#include "DeviceManager.h"
#include "Roles.h"
#include "Util.h"
#include <algorithm>


DevicesModel::DevicesModel(QObject *pParent /*= nullptr*/) : AbstractListModel(pParent), mDevices(DeviceM->getDevices()) {

	// Sort
	sortList();

	connect(
	 DeviceM, &DeviceManager::deviceAdded, this,
	 [this](const QUuid &rDeviceId) {
		 auto deviceName = DeviceM->getName(rDeviceId);
		 auto index = 0;
		 for(int i = 0; i < mDevices.size(); ++i) {
			 if(DeviceM->getName(mDevices.at(i)) >= deviceName) {
				 index = i;
				 break;
			 }
		 }
		 beginInsertRows(QModelIndex(), index, index);
		 mDevices.insert(index, rDeviceId);
		 endInsertRows();
	 },
	 Qt::QueuedConnection);

	connect(
	 DeviceM, &DeviceManager::deviceRemoved, this,
	 [this](const QUuid &rDeviceId) {
		 auto foundIndex = -1;
		 for(auto i = 0; i < mDevices.size(); ++i) {
			 if(mDevices.at(i) == rDeviceId) {
				 foundIndex = i;
				 break;
			 }
		 }
		 if(foundIndex >= 0) {
			 beginRemoveRows(QModelIndex(), foundIndex, foundIndex);
			 mDevices.removeAt(foundIndex);
			 endRemoveRows();
		 }
	 },
	 Qt::QueuedConnection);

	connect(
	 DeviceM, &DeviceManager::deviceChanged, this,
	 [this](const QUuid &rDeviceId) {
		 auto foundIndex = -1;
		 for(auto i = 0; i < mDevices.size(); ++i) {
			 if(mDevices.at(i) == rDeviceId) {
				 foundIndex = i;
				 break;
			 }
		 }
		 if(foundIndex >= 0) {
			 emit dataChanged(index(foundIndex), index(foundIndex));
		 }
	 },
	 Qt::QueuedConnection);
}

DevicesModel::~DevicesModel() = default;

int DevicesModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const {

	return mDevices.size();
}

QVariant DevicesModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const {

	auto row = index.row();
	auto column = index.column();
	auto ret = QVariant();

	if(index.isValid() && column == 0 && mDevices.size() > row) {
		auto deviceInfo = DeviceM->getDeviceInfo(mDevices.at(row));
		switch(role) {
			case Enums::Roles::NameRole:
			case Qt::DisplayRole:
				ret = DeviceM->getName(mDevices.at(row));
				break;
			case Enums::Roles::EndpointRole:
				ret = deviceInfo.mEndpoint;
				break;
			case Enums::Roles::HostRole:
				ret = deviceInfo.mEndpoint.host() + ":" + QString::number(Util::getDefaultPort(deviceInfo.mEndpoint));
				break;
			case Enums::Roles::PortRole:
				ret = Util::getDefaultPort(deviceInfo.mEndpoint);
				break;
			case Enums::Roles::IdRole:
				ret = mDevices.at(row);
				break;
			case Enums::Roles::InitializedRole:
				ret = DeviceM->isDeviceInitialized(mDevices.at(row));
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
	ret.insert(Enums::Roles::EndpointRole, "endpoint");
	ret.insert(Enums::Roles::HostRole, "host");
	ret.insert(Enums::Roles::PortRole, "port");
	ret.insert(Enums::Roles::NameRole, "name");
	ret.insert(Enums::Roles::IdRole, "id");
	ret.insert(Enums::Roles::InitializedRole, "initialized");
	return ret;
}

void DevicesModel::sortList() {

	std::sort(mDevices.begin(), mDevices.end(),
	          [](QUuid left, QUuid right) { return DeviceM->getName(left).compare(DeviceM->getName(right), Qt::CaseInsensitive); });
}
