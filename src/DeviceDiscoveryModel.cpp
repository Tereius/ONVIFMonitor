#include "DeviceDiscoveryModel.h"
#include "DeviceInfo.h"
#include "DeviceManager.h"
#include "OnvifDiscovery.h"
#include "Roles.h"
#include "Device.h"
#include <QHash>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(ddm, "DeviceDiscoveryModel")

DeviceDiscoveryModel::DeviceDiscoveryModel(QObject *pParent /*= nullptr*/) :
 AbstractListModel(pParent), mpDiscovery(new OnvifDiscovery({}, {"tdn:NetworkVideoTransmitter"}, this)), mMatches() {

	connect(
	 mpDiscovery, &OnvifDiscovery::NewMatch, this,
	 [this](const DiscoveryMatch &rMatch) {
		 int oldSize = this->mMatches.size();
		 if(!rMatch.GetDeviceEndpoints().isEmpty() && rMatch.GetDeviceEndpoints().first().isValid()) {
			 beginInsertRows(QModelIndex(), oldSize, oldSize);
			 mMatches << rMatch;
			 endInsertRows();
		 } else {
			 qInfo(ddm) << "Skipping device with empty or invalid device endpoint";
		 }
	 },
	 Qt::QueuedConnection);

	connect(
	 DeviceM, &DeviceManager::deviceAdded, this,
	 [this](const Uuid &rDeviceId) {
		 emit dataChanged(index(0), index(mMatches.size() - 1), {Roles::IsNewRole, Qt::DisplayRole, Roles::HostRole});
	 },
	 Qt::QueuedConnection);

	connect(
	 DeviceM, &DeviceManager::deviceRemoved, this,
	 [this](const Uuid &rDeviceId) {
		 emit dataChanged(index(0), index(mMatches.size() - 1), {Roles::IsNewRole, Qt::DisplayRole, Roles::HostRole});
	 },
	 Qt::QueuedConnection);

	connect(
	 DeviceM, &DeviceManager::deviceChanged, this,
	 [this](const Uuid &rDeviceId) {
		 emit dataChanged(index(0), index(mMatches.size() - 1), {Roles::IsNewRole, Qt::DisplayRole, Roles::HostRole});
	 },
	 Qt::QueuedConnection);
}

DeviceDiscoveryModel::~DeviceDiscoveryModel() {

	mpDiscovery->Stop();
}

int DeviceDiscoveryModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const {

	return mMatches.size();
}

QVariant DeviceDiscoveryModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const {

	auto row = index.row();
	auto column = index.column();
	auto ret = QVariant();

	if(index.isValid() && column == 0 && mMatches.size() > row) {

		auto match = mMatches.at(row);
		auto endpoints = match.GetDeviceEndpoints();
		auto endpoint = !endpoints.isEmpty() ? endpoints.first() : QUrl();
		if(endpoint.isEmpty() || !endpoint.isValid()) {
			qInfo(ddm) << "Skipping device with empty or invalid device endpoint";
			return QVariant();
		}
		auto endpointRef = match.GetEndpointReference();

		switch(role) {
			case Qt::DisplayRole: {
				QString deviceName = "";
				for(const auto &scope : match.GetScopes()) {
					if(scope.startsWith("onvif://www.onvif.org/name/")) {
						deviceName = scope.mid(27);
						break;
					}
				}
				ret = !deviceName.isEmpty() ? deviceName : endpoint.host();
				break;
			}
			case Roles::NameRole: {
				ret = DeviceM->getDeviceInfo(endpointRef).getDeviceName();
				break;
			}
			case Roles::HostRole:
				ret = endpoint.host();
				break;
			case Roles::PortRole: {
				auto port = endpoint.port();
				if(port < 0) {
					auto scheme = endpoint.scheme();
					if(scheme.startsWith("https")) {
						port = 443;
					} else if(scheme.startsWith("http")) {
						port = 80;
					}
				}
				ret = port;
				break;
			}
			case Roles::EndpointRole:
				ret = endpoint;
				break;
			case Roles::IsNewRole: {
				ret = DeviceM->getDeviceInfo(endpointRef).isNull();
				break;
			}
			case Roles::IdRole:
				ret = match.GetEndpointReference();
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
	ret.insert(Roles::PortRole, "port");
	ret.insert(Qt::DisplayRole, "name");
	ret.insert(Roles::NameRole, "existingName");
	ret.insert(Roles::IsNewRole, "isNew");
	ret.insert(Roles::IdRole, "id");
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
