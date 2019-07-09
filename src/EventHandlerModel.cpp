#include "EventHandlerModel.h"
#include "EventManager.h"
#include "Roles.h"


EventHandlerModel::EventHandlerModel(QObject *pParent /*= nullptr*/) :
 AbstractListModel(pParent),
 mEventHanlerTypes(EventM->getRegisteredEventHandler().keys()) {}

int EventHandlerModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const {

	return mEventHanlerTypes.size();
}

QVariant EventHandlerModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const {

	auto row = index.row();
	auto column = index.column();
	auto ret = QVariant();

	if(index.isValid() && column == 0 && mEventHanlerTypes.size() > row &&
	   !EventM->getRegisteredEventHandler().value(mEventHanlerTypes.at(row)).isNull()) {
		switch(role) {
			case Roles::NameRole:
			case Qt::DisplayRole:
				ret = EventM->getRegisteredEventHandler().value(mEventHanlerTypes.at(row)).getName();
				break;
			case Roles::ValueRole:
				ret = mEventHanlerTypes.at(row);
				break;
			case Roles::DescriptionRole:
				ret = EventM->getRegisteredEventHandler().value(mEventHanlerTypes.at(row)).getDescription();
				break;
			default:
				break;
		}

		if(role == Roles::PropertiesRole) {
			QVariantList list;
			for(auto prop : EventM->getRegisteredEventHandler().value(mEventHanlerTypes.at(row)).getProperties()) {
				list.push_back(QVariant::fromValue(prop));
			}
			ret = list;
		}
	}
	return ret;
}

QHash<int, QByteArray> EventHandlerModel::roleNames() const {

	auto ret = QHash<int, QByteArray>();
	ret.insert(Qt::DisplayRole, "display");
	ret.insert(Roles::NameRole, "name");
	ret.insert(Roles::DescriptionRole, "description");
	ret.insert(Roles::ValueRole, "value");
	ret.insert(Roles::PropertiesRole, "properties");
	return ret;
}
