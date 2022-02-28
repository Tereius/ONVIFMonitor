#pragma once
#include "AbstractListModel.h"
#include <QList>
#include <QPointer>
#include <QUuid>


class DeviceManager;

class DevicesModel : public AbstractListModel {

	Q_OBJECT
	Q_PROPERTY(DeviceManager *deviceManager READ getDeviceManager WRITE setDeviceManager REQUIRED)

 public:
	explicit DevicesModel(QObject *pParent = nullptr);
	~DevicesModel() override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;
	DeviceManager *getDeviceManager() const;
	void setDeviceManager(DeviceManager *pManager);

 private:
	Q_DISABLE_COPY(DevicesModel);
	void init();
	void sortList();

	QPointer<DeviceManager> mpManager;
	QList<QUuid> mDevices;
};
