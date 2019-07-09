#pragma once
#include <QAbstractListModel>


class AbstractListModel : public QAbstractListModel {

	Q_OBJECT

public:
	AbstractListModel(QObject *pParent = nullptr);
	Q_INVOKABLE QVariantMap get(int row);

private:
	Q_DISABLE_COPY(AbstractListModel);
};
