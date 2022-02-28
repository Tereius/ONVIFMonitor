#include "AdvancedQmlApplicationEngine.h"
#include "DeviceDiscoveryModel.h"
#include "DiscoveryMatch.h"
#include "ImageProvider.h"
#include "MediaService.h"
#include "OnvifMessageFilterItems.h"
#include "QuickFuture/quickfuture.h"
#include "Roles.h"
#include "info.h"
#include <QDir>
#include <QFileSystemWatcher>
#include <QMetaObject>
#include <QQmlContext>
#include <QThread>
#include <QTimer>

QUuid ApplicationData::createUuid() const {

	return QUuid::createUuid();
}

ProfileId ApplicationData::createProfileId(const QUuid &rDeviceId, const QString &rProfileToken) const {

	return ProfileId(rDeviceId, rProfileToken);
}

AdvancedQmlApplicationEngine::AdvancedQmlApplicationEngine(QObject *parent) :
 QQmlApplicationEngine(parent), mHotReloading(false), mpWatcher(new QFileSystemWatcher(this)), mpTimer(new QTimer(this)) {

	mpTimer->setTimerType(Qt::VeryCoarseTimer);
	mpTimer->setInterval(500);
	mpTimer->setSingleShot(true);
	connect(mpTimer, &QTimer::timeout, this, [this]() { reload(); });
	init();
}

void AdvancedQmlApplicationEngine::init() {

	// Q_IMPORT_PLUGIN(QuickFutureQmlPlugin);
	setImportPathList(QString::fromLocal8Bit(QML_IMPORT_PATHS).split(',', Qt::SkipEmptyParts) + importPathList());
	addImportPath("qrc:///");
	qDebug() << "Used QML import paths" << importPathList();
	qDebug() << "Used QML plugin paths" << pluginPathList();

	if(auto rootCtx = rootContext()) {
		auto appData = new ApplicationData();
		appData->setParent(this);
		rootCtx->setContextProperty("App", appData);
	} else {
		qFatal("Qml engine has no root context");
	}
}

void AdvancedQmlApplicationEngine::setHotReload(bool enable) {

	mHotReloading = enable;
	if(hasRootItem()) {
		connectWatcher();
	}
}

void AdvancedQmlApplicationEngine::loadRootItem(const QString &rootItem) {

	if(rootItem.startsWith("qrc:"))
		loadRootItem(QUrl(rootItem));
	else
		loadRootItem(QUrl::fromLocalFile(rootItem));
}

void AdvancedQmlApplicationEngine::loadRootItem(const QUrl &rootItem) {

	mRootUrl = rootItem;
	load(rootItem);
	if(rootObjects().isEmpty()) {
		qFatal("Couldn't create GUI");
	}
	if(mHotReloading) connectWatcher();
}

bool AdvancedQmlApplicationEngine::hasRootItem() const {

	return !rootObjects().isEmpty();
}

void AdvancedQmlApplicationEngine::connectWatcher() {

	disconnectWatcher();
	auto objects = rootObjects();
	if(!objects.isEmpty() && objects.first()) {
		auto ctx = contextForObject(objects.first());
		if(ctx && ctx->baseUrl().isLocalFile()) {
			QFileInfo fi(ctx->baseUrl().toLocalFile());
			QDir dir(fi.absoluteDir());
			connect(mpWatcher, &QFileSystemWatcher::directoryChanged, this, [this]() {
				if(mpTimer) mpTimer->start();
			});
			connect(mpWatcher, &QFileSystemWatcher::fileChanged, this, [this]() {
				if(mpTimer) mpTimer->start();
			});
			auto filesDirsToWatch = findQmlFilesRecursive(dir);
			for(const auto &qmlFile : filesDirsToWatch) {
				qInfo() << "Enabled hot reloading for following file:" << qmlFile;
				mpWatcher->addPath(qmlFile);
			}
		} else {
			qWarning() << "Can't install filesystem watcher. Root item it not a local file.";
		}
	} else {
		qWarning() << "Can't install filesystem watcher. Missing root item.";
	}
}

void AdvancedQmlApplicationEngine::disconnectWatcher() {

	disconnect(mpWatcher, nullptr, this, nullptr);
	mpWatcher->removePaths(mpWatcher->directories());
	mpWatcher->removePaths(mpWatcher->files());
}

QList<QString> AdvancedQmlApplicationEngine::findQmlFilesRecursive(const QDir &dir) const {

	QList<QString> ret;
	auto qmlFiles = dir.entryList({"*.qml"}, QDir::Files);
	ret.push_back(dir.absolutePath());
	for(const auto &qmlFile : qmlFiles) {
		ret.push_back(dir.absoluteFilePath(qmlFile));
	}
	auto subDirs = dir.entryList({"*"}, QDir::Dirs | QDir::NoDotAndDotDot);
	for(const auto &subDir : subDirs) {
		ret.append(findQmlFilesRecursive(QDir(dir.absoluteFilePath(subDir))));
	}
	return ret;
}

void AdvancedQmlApplicationEngine::reload() {

	for(auto rootObject : rootObjects()) {
		if(rootObject) {
			QMetaObject::invokeMethod(rootObject, "close", Qt::DirectConnection);
		}
	}
	clearComponentCache();
	QThread::msleep(50);
	load(mRootUrl);
}
