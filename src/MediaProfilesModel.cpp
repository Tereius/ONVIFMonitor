#include "MediaProfilesModel.h"
#include "Device.h"
#include "DeviceManager.h"
#include "ProfileId.h"
#include "Roles.h"
#include "Window.h"
#include <QFutureWatcher>
#include <QtConcurrent>


MediaProfilesModel::MediaProfilesModel(QObject *pParent /*= nullptr*/) : AbstractListModel(pParent), mProfiles(), mDeviceId() {

	connect(
	 DeviceM, &DeviceManager::deviceRemoved, this,
	 [this](const QUuid &rDeviceId) {
		 if(rDeviceId == mDeviceId) {
			 auto size = mProfiles.size();
			 beginRemoveRows(QModelIndex(), 0, size - 1);
			 mProfiles.clear();
			 endRemoveRows();
		 }
	 },
	 Qt::QueuedConnection);

	connect(
	 DeviceM, &DeviceManager::deviceChanged, this, [this](const QUuid &rDeviceId) { setDeviceId(rDeviceId); }, Qt::QueuedConnection);
}

int MediaProfilesModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const {

	return mProfiles.size();
}

QVariant MediaProfilesModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const {

	auto row = index.row();
	auto column = index.column();
	auto ret = QVariant();

	if(index.isValid() && column == 0 && mProfiles.size() > row) {

		switch(role) {
			case Enums::Roles::NameRole:
			case Qt::DisplayRole:
				ret = mProfiles.at(row).getName();
				break;
			case Enums::Roles::IdRole:
				ret = QVariant::fromValue(mProfiles.at(row).getProfileId());
				break;
			default:
				break;
		}
	}
	return ret;
}

QHash<int, QByteArray> MediaProfilesModel::roleNames() const {

	auto ret = QHash<int, QByteArray>();
	ret.insert(Qt::DisplayRole, "display");
	ret.insert(Enums::Roles::NameRole, "name");
	ret.insert(Enums::Roles::IdRole, "profileId");
	return ret;
}

QUuid MediaProfilesModel::getDeviceId() const {

	return mDeviceId;
}

void MediaProfilesModel::setDeviceId(const QUuid &rDeviceId) {

	Window::getGlobal()->setModalBusy(true);
	mDeviceId = rDeviceId;
	beginResetModel();
	auto watcher = new QFutureWatcher<void>(this);

	connect(
	 watcher, &QFutureWatcher<void>::finished, this,
	 [=]() {
		 endResetModel();
		 Window::getGlobal()->setModalBusy(false);
		 watcher->deleteLater();
	 },
	 Qt::QueuedConnection);

	emit deviceChanged();
}

void MediaProfilesModel::sortList() {

	std::sort(mProfiles.begin(), mProfiles.end(),
	          [](MediaProfile left, MediaProfile right) { return left.getName().compare(right.getName(), Qt::CaseInsensitive); });
}
