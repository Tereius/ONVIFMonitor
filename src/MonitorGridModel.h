#pragma once
#include "ProfileId.h"
#include <QAbstractItemModel>
#include <QList>
#include <QPointer>
#include <QUrl>


class DeviceManager;

class Tile {

 public:
	int mIndex = 0;
	QUuid mId = QUuid();
	QString mName = QString();
	ProfileId mProfile = ProfileId();
};

class Page {

 public:
	int mIndex = 0;
	QUuid mId = QUuid();
	QString mName = QString();
	QList<Tile> mProfiles = QList<Tile>();
};

class MonitorGridModel : public QAbstractItemModel {

	Q_OBJECT
	Q_PROPERTY(DeviceManager *deviceManager READ getDeviceManager WRITE setDeviceManager REQUIRED)

 public:
	explicit MonitorGridModel(QObject *pParent = nullptr);
	~MonitorGridModel() override;

	Q_INVOKABLE void addPage(const QString &rName);
	Q_INVOKABLE void removePage(const QUuid &rId);
	Q_INVOKABLE void addTile(const QModelIndex &parent, const QUuid &rDeviceId);

	int columnCount(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	DeviceManager *getDeviceManager() const;
	void setDeviceManager(DeviceManager *pManager);
	Q_INVOKABLE void move(const QModelIndex &sourceParent, int source, int destination);

 private:
	Q_DISABLE_COPY(MonitorGridModel);
	void init();

	QPointer<DeviceManager> mpManager;
	QList<Page> mMonitorGrid;
};
