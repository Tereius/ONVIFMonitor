#include "TopicSetModel.h"
#include "Device.h"
#include "DeviceManager.h"
#include "Roles.h"
#include "Window.h"
#include <QtConcurrent>


TopicSetModel::TopicSetModel(QObject *pParent /*= nullptr*/) : AbstractListModel(pParent), mTopics(), mDeviceId() {

	connect(DeviceM, &DeviceManager::deviceRemoved, this,
	        [this](const Uuid &rDeviceId) {
		        if(rDeviceId == mDeviceId) {
			        auto size = mTopics.size();
			        beginRemoveRows(QModelIndex(), 0, size - 1);
			        mTopics.clear();
			        endRemoveRows();
		        }
	        },
	        Qt::QueuedConnection);

	connect(DeviceM, &DeviceManager::deviceChanged, this, [this](const Uuid &rDeviceId) { setDeviceId(rDeviceId); }, Qt::QueuedConnection);
}

int TopicSetModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const {

	return mTopics.size();
}

QVariant TopicSetModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const {

	auto row = index.row();
	auto column = index.column();
	auto ret = QVariant();

	if(index.isValid() && column == 0 && mTopics.size() > row) {

		switch(role) {
			case Roles::NameRole:
			case Qt::DisplayRole:
				ret = mTopics.at(row).GetName();
				break;
			case Roles::PathRole:
				ret = mTopics.at(row).GetTopicPath();
				break;
			default:
				break;
		}

		if(role == Roles::SimpleItemsRole) {
			QVariantList list;
			for(auto item : mTopics.at(row).GetItems()) {
				list.push_back(QVariant::fromValue(item));
			}
			ret = list;
		}
	}
	return ret;
}

QHash<int, QByteArray> TopicSetModel::roleNames() const {

	auto ret = QHash<int, QByteArray>();
	ret.insert(Qt::DisplayRole, "display");
	ret.insert(Roles::NameRole, "name");
	ret.insert(Roles::PathRole, "path");
	ret.insert(Roles::SimpleItemsRole, "simpleItems");
	return ret;
}

Uuid TopicSetModel::getDeviceId() const {

	return mDeviceId;
}

void TopicSetModel::setDeviceId(const Uuid &rDeviceId) {

	Window::getGlobal()->setModalBusy(true);
	mDeviceId = rDeviceId;
	beginResetModel();
	auto watcher = new QFutureWatcher<void>(this);

	connect(watcher, &QFutureWatcher<void>::finished, this,
	        [=]() {
		        endResetModel();
		        Window::getGlobal()->setModalBusy(false);
		        watcher->deleteLater();
	        },
	        Qt::QueuedConnection);

	auto theFuture = QtConcurrent::run([=]() {
		auto pDevice = DeviceM->getDevice(mDeviceId);
		if(pDevice) {
			auto result = pDevice->getTopics();
			if(result) {
				mTopics = result.GetResultObject();
				sortList();
			} else {
				Window::getGlobal()->showError(tr("Device error"), result.toString());
			}
		}
	});
	watcher->setFuture(theFuture);
	emit deviceChanged();
}

void TopicSetModel::sortList() {

	std::sort(mTopics.begin(), mTopics.end(),
	          [](Topic left, Topic right) { return left.GetName().compare(right.GetName(), Qt::CaseInsensitive); });
}
