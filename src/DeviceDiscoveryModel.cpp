#include "DeviceDiscoveryModel.h"
#include "OnvifDiscovery.h"
#include "Roles.h"
#include "Util.h"
#include <QHash>
#include <QLoggingCategory>
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
			 mMatches.push_back(rMatch);
			 endInsertRows();
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

		auto match = mMatches.at(row);
		auto endpoints = match.GetDeviceEndpoints();
		auto endpoint = !endpoints.isEmpty() ? endpoints.first() : QUrl();
		if(endpoint.isEmpty() || !endpoint.isValid()) {
			qInfo(ddm) << "Skipping device with empty or invalid device endpoint";
			return {};
		}
		// auto endpointRef = match.GetEndpointReference();

		switch(role) {
			case Qt::DisplayRole:
			case Enums::Roles::NameRole: {
				auto deviceName = DeviceDiscoveryModel::extractFromScope(match.GetScopes(), "name");
				ret = !deviceName.isEmpty() ? deviceName.first() : endpoint.host();
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
			case Enums::Roles::OnvifProfilesRole: {
				auto profiles = DeviceDiscoveryModel::extractFromScope(match.GetScopes(), "profile");
				ret = !profiles.isEmpty() ? profiles : QVariant();
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
	ret.insert(Enums::Roles::IdRole, "id");
	ret.insert(Enums::Roles::OnvifProfilesRole, "profiles");
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

	std::sort(mMatches.begin(), mMatches.end(), [](DiscoveryMatch left, DiscoveryMatch right) {
		auto leftHost = !left.GetDeviceEndpoints().isEmpty() ? left.GetDeviceEndpoints().first().host() : "";
		auto rightHost = !right.GetDeviceEndpoints().isEmpty() ? right.GetDeviceEndpoints().first().host() : "";
		QString leftName(leftHost);
		QString rightName(rightHost);
		for(const auto &scope : left.GetScopes()) {
			if(scope.startsWith("onvif://www.onvif.org/name/", Qt::CaseInsensitive)) {
				leftName = scope.mid(27);
				break;
			}
		}
		for(const auto &scope : right.GetScopes()) {
			if(scope.startsWith("onvif://www.onvif.org/name/", Qt::CaseInsensitive)) {
				rightName = scope.mid(27);
				break;
			}
		}
		return leftName.compare(rightName, Qt::CaseInsensitive);
	});
}

QStringList DeviceDiscoveryModel::extractFromScope(const QStringList &scopes, const QString &name) {

	QStringList ret;
	for(const auto &scope : scopes) {
		auto urlDecodedString = QUrl::fromPercentEncoding(scope.toLocal8Bit());
		auto urlPrefix = "onvif://www.onvif.org/" + name + "/";
		if(urlDecodedString.startsWith(urlPrefix, Qt::CaseInsensitive)) {
			ret.push_back(urlDecodedString.mid(urlPrefix.length()));
		}
	}
	return ret;
}
