#pragma once
#include "AbstractListModel.h"


class EventsModel : public AbstractListModel {

public:
	EventsModel(QObject* pParent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;
};
