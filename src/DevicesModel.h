#pragma once
#include "AbstractListModel.h"
#include "DeviceInfo.h"
#include "Uuid.h"
#include <QList>
#include <QPointer>


class DevicesModel : public AbstractListModel {

	Q_OBJECT

public:
	DevicesModel(QObject* pParent = nullptr);
	virtual ~DevicesModel();
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	Q_DISABLE_COPY(DevicesModel);

	void sortList();

	QList<DeviceInfo> mMatches;
};
