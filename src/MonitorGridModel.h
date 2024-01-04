#pragma once
#include "ProfileId.h"
#include "Serializable.h"
#include <QAbstractItemModel>
#include <QList>
#include <QPointer>
#include <QUrl>
#include <QtQmlIntegration>


class DeviceManager;

// changing the class name or one of the properties will break (de)serialization from settings file
class MonitorSettings : public SerializeSettings {

	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool enableVideoStream MEMBER enableVideoStream NOTIFY changed STORED true)
	Q_PROPERTY(bool enableAudioStream MEMBER enableAudioStream NOTIFY changed STORED true)
	Q_PROPERTY(qreal volume MEMBER volume NOTIFY changed STORED true)
	Q_PROPERTY(bool enableBackchannel MEMBER enableBackchannel NOTIFY changed STORED true)
	Q_PROPERTY(QString audioCodec MEMBER audioCodec NOTIFY changed STORED true)
	Q_PROPERTY(QString audioInputDevice MEMBER audioInputDevice NOTIFY changed STORED true)
	Q_PROPERTY(bool pushToTalk MEMBER pushToTalk NOTIFY changed STORED true)
	Q_PROPERTY(qreal micSensitivity MEMBER micSensitivity NOTIFY changed STORED true)

 public:
	bool enableVideoStream = true;
	bool enableAudioStream = true;
	qreal volume = 1.0;
	bool enableBackchannel = false;
	QString audioCodec = {};
	QString audioInputDevice = {};
	bool pushToTalk = false;
	qreal micSensitivity = 1.0;

 signals:
	void changed();
};

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

	Q_INVOKABLE void addPage(const QString &rName, const QUuid &proposedId = QUuid::createUuid());
	Q_INVOKABLE void removePage(const QUuid &rId);
	Q_INVOKABLE void addTile(const QModelIndex &parent, const QUuid &rDeviceId, MonitorSettings *monitorSettings);
	Q_INVOKABLE void editTile(const QModelIndex &parent, const QModelIndex &index, MonitorSettings *monitorSettings);
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
