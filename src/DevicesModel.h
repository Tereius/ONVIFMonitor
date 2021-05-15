#pragma once
#include "AbstractListModel.h"
#include <QUuid>
#include <QList>
#include <QPointer>


class DevicesModel : public AbstractListModel {

	Q_OBJECT

 public:
	explicit DevicesModel(QObject *pParent = nullptr);
	~DevicesModel() override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

 private:
	Q_DISABLE_COPY(DevicesModel);

	void sortList();

	QList<QUuid> mDevices;
};
