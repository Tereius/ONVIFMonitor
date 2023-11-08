#include "MonitorGridModel.h"
#include "DeviceManager.h"
#include "Roles.h"
#include <QSettings>

MonitorGridModel::MonitorGridModel(QObject *pParent /*= nullptr*/) : QAbstractItemModel(pParent), mpManager(nullptr), mMonitorGrid() {}

MonitorGridModel::~MonitorGridModel() = default;

void MonitorGridModel::addPage(const QString &rName) {

	beginInsertRows(QModelIndex(), mMonitorGrid.size(), mMonitorGrid.size());
	auto page = Page();
	page.mIndex = mMonitorGrid.size();
	page.mId = QUuid::createUuid();
	page.mName = rName;
	page.mProfiles = QList<Tile>();
	mMonitorGrid.push_back(page);
	QSettings settings;
	settings.beginGroup("monitoring");
	settings.beginGroup(page.mId.toString());
	settings.setValue("index", page.mIndex);
	settings.setValue("id", page.mId.toString());
	settings.setValue("name", page.mName);
	settings.endGroup();
	settings.endGroup();
	settings.sync();
	endInsertRows();
}

void MonitorGridModel::removePage(const QUuid &rId) {

	for(auto i = 0; i < mMonitorGrid.size(); i++) {
		auto page = mMonitorGrid.at(i);
		if(page.mId == rId) {
			beginRemoveRows(QModelIndex(), i, i);
			mMonitorGrid.removeAt(i);
			QSettings settings;
			settings.beginGroup("monitoring");
			settings.remove(page.mId.toString());
			settings.endGroup();
			settings.sync();
			endRemoveRows();
			break;
		}
	}
}

void MonitorGridModel::addTile(const QModelIndex &parent, const QUuid &rDeviceId) {

	auto page = parent.row();
	auto column = parent.column();
	if(parent.isValid() && column == 0 && mMonitorGrid.size() > page && mpManager->containsDevice(rDeviceId)) {
		auto info = mpManager->getDeviceInfo(rDeviceId);
		if(!info.mMediaProfiles.isEmpty()) {
			auto &pageInfo = mMonitorGrid[page];
			beginInsertRows(parent, pageInfo.mProfiles.size(), pageInfo.mProfiles.size());
			auto tile = Tile();
			tile.mIndex = pageInfo.mProfiles.size();
			tile.mId = QUuid::createUuid();
			tile.mName = "";
			tile.mProfile = info.mMediaProfiles.first().getProfileId();
			pageInfo.mProfiles.push_back(tile);
			mMonitorGrid[page] = pageInfo;
			QSettings settings;
			settings.beginGroup("monitoring");
			settings.beginGroup(pageInfo.mId.toString());
			settings.beginGroup(tile.mId.toString());
			settings.setValue("index", tile.mIndex);
			settings.setValue("id", tile.mId.toString());
			settings.setValue("name", "");
			settings.setValue("profile_deviceId", tile.mProfile.getDeviceId().toString());
			settings.setValue("profile_token", tile.mProfile.getProfileToken());
			settings.endGroup();
			settings.endGroup();
			settings.endGroup();
			settings.sync();
			endInsertRows();
		}
	}
}

void MonitorGridModel::removeTile(const QModelIndex &parent, const QUuid &rDeviceId) {

	auto page = parent.row();
	auto column = parent.column();
	if(parent.isValid() && column == 0 && mMonitorGrid.size() > page && mpManager->containsDevice(rDeviceId)) {
		auto info = mpManager->getDeviceInfo(rDeviceId);
		if(!info.mMediaProfiles.isEmpty()) {
			auto &pageInfo = mMonitorGrid[page];
			beginRemoveRows(parent, pageInfo.mProfiles.size(), pageInfo.mProfiles.size());
			auto tile = Tile();
			tile.mIndex = pageInfo.mProfiles.size();
			tile.mId = QUuid::createUuid();
			tile.mName = "";
			tile.mProfile = info.mMediaProfiles.first().getProfileId();
			pageInfo.mProfiles.push_back(tile);
			mMonitorGrid[page] = pageInfo;
			QSettings settings;
			settings.beginGroup("monitoring");
			settings.beginGroup(pageInfo.mId.toString());
			settings.beginGroup(tile.mId.toString());
			settings.setValue("index", tile.mIndex);
			settings.setValue("id", tile.mId.toString());
			settings.setValue("name", "");
			settings.setValue("profile_deviceId", tile.mProfile.getDeviceId().toString());
			settings.setValue("profile_token", tile.mProfile.getProfileToken());
			settings.endGroup();
			settings.endGroup();
			settings.endGroup();
			settings.sync();
			endInsertRows();
		}
	}
}

int MonitorGridModel::rowCount(const QModelIndex &parent) const {

	if(parent.isValid()) {
		auto row = parent.row();
		if(row >= mMonitorGrid.size()) {
			qWarning() << "Out of bounds";
		} else {
			return mMonitorGrid.at(row).mProfiles.size();
		}
	}
	return mMonitorGrid.size();
}

QVariant MonitorGridModel::data(const QModelIndex &index, int role) const {

	auto ret = QVariant();
	auto row = index.row();
	auto column = index.column();
	if(!index.parent().isValid()) {
		// pages
		if(index.isValid() && column == 0 && mMonitorGrid.size() > row) {
			auto monitorTile = mMonitorGrid.at(row);
			switch(role) {
				case Enums::Roles::NameRole:
				case Qt::DisplayRole:
					ret = QString("Page %1").arg(row);
					break;
				case Enums::Roles::IdRole:
					ret = monitorTile.mId;
					break;
				default:
					ret = QVariant();
					break;
			}
		}
	} else {
		// tiles
		auto page = index.parent().row();
		if(index.isValid() && column == 0 && mMonitorGrid.size() > page && mMonitorGrid.at(page).mProfiles.size() > row) {
			auto monitorTile = mMonitorGrid.at(page).mProfiles.at(row);
			switch(role) {
				case Enums::Roles::NameRole:
				case Qt::DisplayRole:
					ret = monitorTile.mProfile.getProfileToken();
					break;
				case Enums::Roles::IdRole:
					ret = monitorTile.mProfile.getDeviceId();
					break;
				case Enums::Roles::ProfileRole:
					ret = monitorTile.mProfile.getProfileToken();
					break;
				default:
					ret = QVariant();
					break;
			}
		}
	}
	return ret;
}

QHash<int, QByteArray> MonitorGridModel::roleNames() const {

	auto ret = QHash<int, QByteArray>();
	ret.insert(Enums::Roles::NameRole, "name");
	ret.insert(Enums::Roles::IdRole, "deviceId");
	ret.insert(Enums::Roles::ProfileRole, "token");
	return ret;
}

void MonitorGridModel::init() {

	beginResetModel();
	mMonitorGrid.clear();
	QSettings settings;
	settings.beginGroup("monitoring");
	auto keys = settings.childGroups();
	for(auto i = 0; i < keys.size(); i++) {
		settings.beginGroup(keys.at(i));
		auto page = Page();
		page.mIndex = settings.value("index").toInt();
		page.mId = settings.value("id").toUuid();
		page.mName = settings.value("name").toString();
		page.mProfiles = QList<Tile>();
		auto pages = settings.childGroups();
		for(auto ii = 0; ii < pages.size(); ii++) {
			settings.beginGroup(pages.at(ii));
			auto tile = Tile();
			tile.mIndex = settings.value("index").toInt();
			tile.mId = settings.value("id").toUuid();
			tile.mName = settings.value("name").toString();
			tile.mProfile = ProfileId(QUuid(settings.value("profile_deviceId").toString()), settings.value("profile_token").toString());
			page.mProfiles.push_back(tile);
			settings.endGroup();
		}
		std::sort(page.mProfiles.begin(), page.mProfiles.end(), [](const Tile &left, const Tile &right) { return left.mIndex < right.mIndex; });
		mMonitorGrid.push_back(page);
		settings.endGroup();
	}
	std::sort(mMonitorGrid.begin(), mMonitorGrid.end(), [](const Page &left, const Page &right) { return left.mIndex < right.mIndex; });
	settings.endGroup();
	endResetModel();
}

int MonitorGridModel::columnCount(const QModelIndex &index) const {
	return 1;
}

QModelIndex MonitorGridModel::index(int row, int column, const QModelIndex &parent) const {

	if(!hasIndex(row, column, parent)) return QModelIndex();

	if(!parent.isValid())
		return createIndex(row, column, std::numeric_limits<qintptr>::max());
	else
		return createIndex(row, column, parent.row());

	return hasIndex(row, column, parent) ? createIndex(row, column, row) : QModelIndex();
}

QModelIndex MonitorGridModel::parent(const QModelIndex &index) const {

	if(index.internalId() == std::numeric_limits<qintptr>::max()) {
		// top lvl
		return {};
	} else {
		return createIndex(index.internalId(), 0, std::numeric_limits<qintptr>::max());
	}
}

DeviceManager *MonitorGridModel::getDeviceManager() const {
	return mpManager;
}

void MonitorGridModel::setDeviceManager(DeviceManager *pManager) {

	mpManager = pManager;
	init();
}

void MonitorGridModel::move(const QModelIndex &sourceParent, int source, int destination) {

	auto page = sourceParent.row();
	auto column = sourceParent.column();
	if(sourceParent.isValid() && column == 0 && source >= 0 && mMonitorGrid.size() > page &&
	   mMonitorGrid.at(page).mProfiles.size() > destination) {
		auto count = 1;
		if(source + count - 1 < destination) {
			if(beginMoveRows(sourceParent, source, source + count - 1, sourceParent, destination + 1)) {
				auto &pageInfo = mMonitorGrid[page];
				QSettings settings;
				settings.beginGroup("monitoring");
				settings.beginGroup(pageInfo.mId.toString());

				settings.beginGroup(pageInfo.mProfiles.at(source).mId.toString());
				settings.setValue("index", destination);
				settings.endGroup();

				settings.beginGroup(pageInfo.mProfiles.at(destination).mId.toString());
				settings.setValue("index", source);
				settings.endGroup();

				settings.endGroup();
				settings.endGroup();
				settings.sync();
				pageInfo.mProfiles.move(source, destination);
				endMoveRows();
			}
		} else if(source > destination) {
			if(beginMoveRows(sourceParent, source, source + count - 1, sourceParent, destination)) {
				auto &pageInfo = mMonitorGrid[page];
				QSettings settings;
				settings.beginGroup("monitoring");
				settings.beginGroup(pageInfo.mId.toString());

				settings.beginGroup(pageInfo.mProfiles.at(source).mId.toString());
				settings.setValue("index", destination);
				settings.endGroup();

				settings.beginGroup(pageInfo.mProfiles.at(destination).mId.toString());
				settings.setValue("index", source);
				settings.endGroup();

				settings.endGroup();
				settings.endGroup();
				settings.sync();
				pageInfo.mProfiles.move(source, destination);
				endMoveRows();
			}
		}
	}
}
