#pragma once
#include "ProfileId.h"
#include <QAbstractItemModel>
#include <QList>
#include <QPointer>
#include <QUrl>
#include <QtQmlIntegration>

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
	QML_ELEMENT
	Q_PROPERTY(DeviceManager *deviceManager READ getDeviceManager WRITE setDeviceManager REQUIRED)
	Q_PROPERTY(int pageCount READ getPageCount NOTIFY pageCountChanged)
	Q_PROPERTY(int monitorCount READ getMonitorCount NOTIFY monitorCountChanged)

 public:
	explicit MonitorGridModel(QObject *pParent = nullptr);
	~MonitorGridModel() override;

	Q_INVOKABLE void addPage(const QString &rName);
	Q_INVOKABLE void removePage(const QUuid &rId);
	Q_INVOKABLE void addTile(const QModelIndex &parent, const QUuid &rDeviceId);
	Q_INVOKABLE void removeTile(const QModelIndex &parent, const QModelIndex &index);

	int columnCount(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	DeviceManager *getDeviceManager() const;
	void setDeviceManager(DeviceManager *pManager);
	int getPageCount() const;
	int getMonitorCount() const;
	Q_INVOKABLE void move(const QModelIndex &sourceParent, int source, int destination);

 signals:
	void pageCountChanged();
	void monitorCountChanged();

 private:
	Q_DISABLE_COPY(MonitorGridModel);
	void init();

	QPointer<DeviceManager> mpManager;
	QList<Page> mMonitorGrid;
};
