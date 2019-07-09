#include "EventSourceModel.h"
#include "EventManager.h"
#include "Roles.h"


EventSourceModel::EventSourceModel(QObject *pParent /*= nullptr*/) :
 AbstractListModel(pParent),
 mEventSourceTypes(EventM->getRegisteredEventSources().keys()) {}

int EventSourceModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const {

	return mEventSourceTypes.size();
}

QVariant EventSourceModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const {

	auto row = index.row();
	auto column = index.column();
	auto ret = QVariant();

	if(index.isValid() && column == 0 && mEventSourceTypes.size() > row &&
	   !EventM->getRegisteredEventSources().value(mEventSourceTypes.at(row)).isNull()) {

		switch(role) {
			case Roles::NameRole:
			case Qt::DisplayRole:
				ret = EventM->getRegisteredEventSources().value(mEventSourceTypes.at(row)).getName();
				break;
			case Roles::ValueRole:
				ret = mEventSourceTypes.at(row);
				break;
			case Roles::DescriptionRole:
				ret = EventM->getRegisteredEventSources().value(mEventSourceTypes.at(row)).getDescription();
				break;
			default:
				break;
		}

		if(role == Roles::PropertiesRole) {
			QVariantList list;
			for(auto prop : EventM->getRegisteredEventSources().value(mEventSourceTypes.at(row)).getProperties()) {
				list.push_back(QVariant::fromValue(prop));
			}
			ret = list;
		}
	}
	return ret;
}

QHash<int, QByteArray> EventSourceModel::roleNames() const {

	auto ret = QHash<int, QByteArray>();
	ret.insert(Qt::DisplayRole, "display");
	ret.insert(Roles::NameRole, "name");
	ret.insert(Roles::DescriptionRole, "description");
	ret.insert(Roles::ValueRole, "value");
	ret.insert(Roles::PropertiesRole, "properties");
	return ret;
}
