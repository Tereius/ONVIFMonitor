#pragma once
#include "AbstractListModel.h"
#include "Uuid.h"
#include <QPair>


class EventBindingModel : public AbstractListModel {

	Q_OBJECT

public:
	EventBindingModel(QObject *pParent = nullptr);
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;
	Q_INVOKABLE QVariantMap get(const Uuid &rBindingId);

private:
	Q_DISABLE_COPY(EventBindingModel);

	void sortList();

	QList<QPair<QString, Uuid>> mEventBindings;
};
