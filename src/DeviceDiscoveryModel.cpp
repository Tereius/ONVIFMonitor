#include "DeviceDiscoveryModel.h"
#include "DeviceProbe.h"
#include "OnvifDiscovery.h"
#include "Roles.h"
#include "Util.h"
#include "asyncfuture.h"
#include <QHash>
#include <QLoggingCategory>
#include <QtConcurrent>
#include <algorithm>

Q_LOGGING_CATEGORY(ddm, "DeviceDiscoveryModel")

DeviceDiscoveryModel::DeviceDiscoveryModel(QObject *pParent /*= nullptr*/) :
 AbstractListModel(pParent), mpDiscovery(new OnvifDiscovery({}, {"tdn:NetworkVideoTransmitter"}, this)), mMatches() {

	connect(
	 mpDiscovery, &OnvifDiscovery::NewMatch, this,
	 [this](const DiscoveryMatch &rMatch) {
		 auto oldSize = this->mMatches.size();
		 if(!rMatch.GetDeviceEndpoints().isEmpty() && rMatch.GetDeviceEndpoints().first().isValid()) {

			 beginInsertRows(QModelIndex(), oldSize, oldSize);
			 mMatches.push_back(qMakePair<DiscoveryMatch, DeviceProbe>(rMatch, {}));
			 endInsertRows();

			 AsyncFuture::observe(QtConcurrent::run([rMatch]() { return DeviceProbe::create(rMatch.GetDeviceEndpoints().first()); }))
			  .subscribe([this, rMatch](QFuture<DeviceProbe> probe) {
				  auto foundIndex = -1;
				  for(auto i = 0; i < mMatches.size(); ++i) {
					  if(mMatches.at(i).first == rMatch) {
						  foundIndex = i;
						  break;
					  }
				  }
				  if(foundIndex >= 0) {
					  auto other = qMakePair<DiscoveryMatch, DeviceProbe>(rMatch, probe.result());
					  mMatches[foundIndex].swap(other);
					  emit dataChanged(createIndex(foundIndex, 0), createIndex(foundIndex, 0),
					                   {Enums::Roles::ServicesRole, Enums::Roles::ManufacturerRole});
				  }
			  });

		 } else {
			 qInfo(ddm) << "Skipping device with empty or invalid device endpoint";
		 }
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

		auto match = mMatches.at(row).first;
		auto probe = mMatches.at(row).second;
		auto endpoints = match.GetDeviceEndpoints();
		auto endpoint = !endpoints.isEmpty() ? endpoints.first() : QUrl();
		if(endpoint.isEmpty() || !endpoint.isValid()) {
			qInfo(ddm) << "Skipping device with empty or invalid device endpoint";
			return QVariant();
		}
		// auto endpointRef = match.GetEndpointReference();

		switch(role) {
			case Qt::DisplayRole:
			case Enums::Roles::NameRole: {
				QString deviceName = "";
				for(const auto &scope : match.GetScopes()) {
					if(scope.startsWith("onvif://www.onvif.org/")) {
						QUrl onvifScope = QUrl(scope);
						if(onvifScope.path().startsWith("/name/")) {
							deviceName = onvifScope.path().mid(6);
							break;
						}
					}
				}
				ret = !deviceName.isEmpty() ? deviceName : endpoint.host();
				break;
			}
			case Enums::Roles::HostRole:
				ret = endpoint.host();
				break;
			case Enums::Roles::PortRole: {
				ret = Util::getDefaultPort(endpoint);
				break;
			}
			case Enums::Roles::EndpointRole:
				ret = endpoint;
				break;
			case Enums::Roles::IdRole:
				ret = !match.GetEndpointReference().isNull() ? match.GetEndpointReference() : QVariant();
				break;
			case Enums::Roles::ManufacturerRole:
				ret = probe.GetManufacturer();
				break;
			case Enums::Roles::ServicesRole: {
				QVariantList list;
				for(const auto &service : probe.GetServices()) {
					list.push_back(static_cast<int>(service));
				}
				ret = list;
				break;
			}
			default:
				ret = QVariant();
				break;
		}
	}
	return ret;
}

QHash<int, QByteArray> DeviceDiscoveryModel::roleNames() const {

	auto ret = QHash<int, QByteArray>();
	ret.insert(Enums::Roles::EndpointRole, "endpoint");
	ret.insert(Enums::Roles::HostRole, "host");
	ret.insert(Enums::Roles::PortRole, "port");
	ret.insert(Enums::Roles::NameRole, "name");
	ret.insert(Enums::Roles::ManufacturerRole, "manufacturer");
	ret.insert(Enums::Roles::ServicesRole, "services");
	ret.insert(Enums::Roles::IdRole, "id");
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

void DeviceDiscoveryModel::sortList() {

	std::sort(mMatches.begin(), mMatches.end(), [](QPair<DiscoveryMatch, DeviceProbe> left, QPair<DiscoveryMatch, DeviceProbe> right) {
		auto leftHost = !left.first.GetDeviceEndpoints().isEmpty() ? left.first.GetDeviceEndpoints().first().host() : "";
		auto rightHost = !right.first.GetDeviceEndpoints().isEmpty() ? right.first.GetDeviceEndpoints().first().host() : "";
		QString leftName(leftHost);
		QString rightName(rightHost);
		for(const auto &scope : left.first.GetScopes()) {
			if(scope.startsWith("onvif://www.onvif.org/name/")) {
				leftName = scope.mid(27);
				break;
			}
		}
		for(const auto &scope : right.first.GetScopes()) {
			if(scope.startsWith("onvif://www.onvif.org/name/")) {
				rightName = scope.mid(27);
				break;
			}
		}
		return leftName.compare(rightName, Qt::CaseInsensitive);
	});
}
