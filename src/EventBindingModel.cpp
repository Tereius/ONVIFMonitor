#include "EventBindingModel.h"
#include "EventBinding.h"
#include "EventManager.h"
#include "Roles.h"
#include <algorithm>


EventBindingModel::EventBindingModel(QObject *pParent /*= nullptr*/) : AbstractListModel(pParent), mEventBindings() {

	for(auto binding : EventM->getEventBindings()) {
		mEventBindings.push_back(QPair<QString, QUuid>(binding->getName(), binding->getId()));
	}
	sortList();

	connect(
	 EventM, &EventManager::eventBindingAdded, this,
	 [this](const QUuid &rBindingId) {
		 auto addedBinding = EventM->getEventBinding(rBindingId);
		 auto index = 0;
		 for(int i = 0; i < mEventBindings.size(); ++i) {
			 if(mEventBindings.at(i).first >= addedBinding->getName()) {
				 index = i;
				 break;
			 }
		 }
		 beginInsertRows(QModelIndex(), index, index);
		 mEventBindings.insert(index, QPair<QString, QUuid>(addedBinding->getName(), addedBinding->getId()));
		 endInsertRows();
	 },
	 Qt::QueuedConnection);

	connect(
	 EventM, &EventManager::eventBindingRemoved, this,
	 [this](const QUuid &rBindingId) {
		 auto foundIndex = -1;
		 for(auto i = 0; i < mEventBindings.size(); ++i) {
			 if(mEventBindings.at(i).second == rBindingId) {
				 foundIndex = i;
				 break;
			 }
		 }
		 if(foundIndex >= 0) {
			 beginRemoveRows(QModelIndex(), foundIndex, foundIndex);
			 mEventBindings.removeAt(foundIndex);
			 endRemoveRows();
		 }
	 },
	 Qt::QueuedConnection);

	connect(
	 EventM, &EventManager::eventBindingChanged, this,
	 [this](const QUuid &rBindingId) {
		 auto foundIndex = -1;
		 for(auto i = 0; i < mEventBindings.size(); ++i) {
			 if(mEventBindings.at(i).second == rBindingId) {
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

int EventBindingModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const {

	return mEventBindings.size();
}

QVariant EventBindingModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const {

	auto row = index.row();
	auto column = index.column();
	auto ret = QVariant();

	if(index.isValid() && column == 0 && mEventBindings.size() > row) {
		QSharedPointer<EventBinding> binding = EventM->getEventBinding(mEventBindings.at(row).second);
		switch(role) {
			case Roles::NameRole:
			case Qt::DisplayRole:
				if(binding) ret = binding->getName();
				break;
			case Roles::DescriptionRole:
				if(binding) ret = binding->getDescription();
				break;
			case Roles::IdRole:
				if(binding) ret = QVariant::fromValue(binding->getId());
				break;
			case Roles::StatusRole:
				if(binding) ret = binding->getStatus();
				break;
			case Roles::HasSourceRole:
				if(binding) ret = !binding->getSource().isNull();
				break;
			case Roles::HasHandlerRole:
				if(binding) ret = !binding->getHandler().isNull();
				break;
			case Roles::EventSourceNameRole:
				if(binding && binding->getSource()) ret = binding->getSource()->getName();
				break;
			case Roles::EventHandlerNameRole:
				if(binding && binding->getHandler()) ret = binding->getHandler()->getName();
				break;
			case Roles::EventSourcePropertiesRole:
				if(binding && binding->getSource()) ret = binding->getSource()->getPropertyValues();
				break;
			case Roles::EventHandlerPropertiesRole:
				if(binding && binding->getHandler()) ret = binding->getHandler()->getPropertyValues();
				break;
			default:
				ret = QVariant();
				break;
		}
	}
	return ret;
}

QHash<int, QByteArray> EventBindingModel::roleNames() const {

	auto ret = QHash<int, QByteArray>();
	ret.insert(Qt::DisplayRole, "display");
	ret.insert(Roles::NameRole, "name");
	ret.insert(Roles::DescriptionRole, "description");
	ret.insert(Roles::IdRole, "bindingId");
	ret.insert(Roles::StatusRole, "status");
	ret.insert(Roles::HasSourceRole, "hasSource");
	ret.insert(Roles::HasHandlerRole, "hasHandler");
	ret.insert(Roles::EventSourceNameRole, "eventSource");
	ret.insert(Roles::EventHandlerNameRole, "eventHandler");
	ret.insert(Roles::EventSourcePropertiesRole, "eventSourceProperties");
	ret.insert(Roles::EventHandlerPropertiesRole, "eventHandlerProperties");

	return ret;
}

Q_INVOKABLE QVariantMap EventBindingModel::get(const QUuid &rBindingId) {

	QVariantMap ret;
	auto index = 0;
	for(auto entry : mEventBindings) {
		if(entry.second == rBindingId) {
			ret = AbstractListModel::get(index);
			break;
		}
		index++;
	}
	return ret;
}

void EventBindingModel::sortList() {

	std::sort(mEventBindings.begin(), mEventBindings.end(),
	          [](QPair<QString, QUuid> left, QPair<QString, QUuid> right) { return left.first.compare(right.first, Qt::CaseInsensitive); });
}
