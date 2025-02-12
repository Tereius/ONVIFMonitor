#include "MediaProfilesModel.h"
#include "AsyncFuture/asyncfuture.h"
#include "Device.h"
#include "DeviceManager.h"
#include "Enums.h"
#include "ProfileId.h"
#include "Window.h"
#include <QFutureWatcher>


#define FROM_CACHE

MediaProfilesModel::MediaProfilesModel(QObject *pParent /*= nullptr*/) : AbstractListModel(pParent), mProfiles(), mDeviceId() {}

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
			case Enums::Roles::HasBackchannelRole:
				ret = QVariant::fromValue(mProfiles.at(row).mBackchennel);
				break;
			case Enums::Roles::HasSnapshotRole:
				ret = QVariant::fromValue(mProfiles.at(row).mSnapshotCapability);
				break;
			case Enums::Roles::BackchannelSdp:
				ret = QVariant::fromValue(mProfiles.at(row).mBackchennelMediaDescription);
				break;
			case Enums::Roles::BackchannelUrl:
				ret = QVariant::fromValue(mProfiles.at(row).mBackchennelUrl);
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
	ret.insert(Enums::Roles::HasBackchannelRole, "hasBackchannel");
	ret.insert(Enums::Roles::HasSnapshotRole, "hasSnapshot");
	ret.insert(Enums::Roles::BackchannelSdp, "backchannelSdp");
	ret.insert(Enums::Roles::BackchannelUrl, "backchannelUrl");
	return ret;
}

QUuid MediaProfilesModel::getDeviceId() const {

	return mDeviceId;
}

void MediaProfilesModel::setDeviceId(const QUuid &rDeviceId) {

	Window::getGlobal()->setModalBusy(true);
	mDeviceId = rDeviceId;

#ifdef FROM_CACHE
	const auto deviceInfo = DeviceManager::getInstance()->getDeviceInfo(mDeviceId);
	beginResetModel();
	if(deviceInfo.mInitialized) {
		mProfiles = deviceInfo.mMediaProfiles;
	} else {
		mProfiles.clear();
	}
	endResetModel();
#elif
	auto mediaProfilesFuture = DeviceManager::getInstance()->getMediaProfiles(mDeviceId);
	AsyncFuture::observe(mediaProfilesFuture)
	 .subscribe(
	  [this, mediaProfilesFuture]() {
		  beginResetModel();
		  if(auto result = mediaProfilesFuture.result()) {
			  mProfiles = result.GetResultObject();
		  }
		  endResetModel();
	  },
	  [this]() {
		  beginResetModel();
		  mProfiles.clear();
		  endResetModel();
	  });
#endif

	emit deviceChanged();
}
