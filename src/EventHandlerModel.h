#pragma once
#include "AbstractListModel.h"


/*!
 *
 * \brief Lists all available event handler
 *
 */
class EventHandlerModel : public AbstractListModel {

	Q_OBJECT

public:
	EventHandlerModel(QObject* pParent = nullptr);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	Q_DISABLE_COPY(EventHandlerModel);

	QList<QString> mEventHanlerTypes;
};
