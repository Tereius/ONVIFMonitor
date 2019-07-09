#include "AbstractListModel.h"


AbstractListModel::AbstractListModel(QObject* pParent /*= nullptr*/) : QAbstractListModel(pParent) {}

QVariantMap AbstractListModel::get(int row) {

	QHash<int, QByteArray> names = roleNames();
	QHashIterator<int, QByteArray> i(names);
	QVariantMap res;
	while(i.hasNext()) {
		i.next();
		QModelIndex idx = index(row, 0);
		QVariant data = idx.data(i.key());
		res[i.value()] = data;
	}
	return res;
}
