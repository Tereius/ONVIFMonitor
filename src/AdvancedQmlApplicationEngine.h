#pragma once
#include "ProfileId.h"
#include <QQmlApplicationEngine>
#include <QFileSystemWatcher>
#include <QUrl>
#include <QUuid>

class QFileSystemWatcher;
class QTimer;

class ApplicationData : public QObject {

	Q_OBJECT

 public:
	Q_INVOKABLE QUuid createUuid() const;
	Q_INVOKABLE ProfileId createProfileId(const QUuid &rDeviceId, const QString &rProfileToken) const;
};

class AdvancedQmlApplicationEngine : public QQmlApplicationEngine {

	Q_OBJECT

 public:
	explicit AdvancedQmlApplicationEngine(QObject *parent = nullptr);
	void setHotReload(bool enable);
	void loadRootItem(const QString &rootItem);
	void loadRootItem(const QUrl &rootItem);
	bool hasRootItem() const;

 private slots:
	void reload();

 private:
	Q_DISABLE_COPY(AdvancedQmlApplicationEngine)
	static void registerMetatypes();
	static void registerQmlTypes();

	void init();
	void connectWatcher();
	void disconnectWatcher();

	QUrl mRootUrl;
	bool mHotReloading;
	QFileSystemWatcher *mpWatcher;
	QTimer *mpTimer;
};
