#include "AdvancedQmlApplicationEngine.h"
#include "IconImageProvider.h"
#include "ImageProvider.h"
#include "info.h"
#include "Window.h"
#include "Roles.h"
#include "DeviceManager.h"
#include "MediaService.h"
#include "DeviceInfo.h"
#include "DiscoveryMatch.h"
#include "DeviceDiscoveryModel.h"
#include "DevicesModel.h"
#include "MediaProfilesModel.h"
#include "OnvifMessageFilterItems.h"
#include "PropertyInfo.h"
#include "SortFilterProxyModel.h"
#include "QuickFuture/quickfuture.h"
#include <QCoreApplication>
#include <QDir>
#include <QQmlContext>
#include <QThread>
#include <QFileSystemWatcher>

#include <QMetaObject>
#include <QTimer>


Q_DECLARE_METATYPE(DetailedResult<QUrl>)
Q_DECLARE_METATYPE(QFuture<DetailedResult<QUrl>>)
Q_DECLARE_METATYPE(DetailedResult<QUuid>)
Q_DECLARE_METATYPE(QFuture<DetailedResult<QUuid>>)

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
	addImageProvider("icons", new IconImageProvider(qApp));
	addImageProvider("profile", new ImageProvider(qApp));
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

	AdvancedQmlApplicationEngine::registerMetatypes();
	AdvancedQmlApplicationEngine::registerQmlTypes();
}

void AdvancedQmlApplicationEngine::setHotReload(bool enable) {

	mHotReloading = enable;
	if(hasRootItem()) {
		connectWatcher();
	}
}

void AdvancedQmlApplicationEngine::registerMetatypes() {

	qRegisterMetaType<MediaProfile>();
	qRegisterMetaType<DiscoveryMatch>();
	qRegisterMetaType<ProfileId>();
	qRegisterMetaType<Error>();
	QMetaType::registerConverter<ProfileId, QString>(&ProfileId::toString);
	qRegisterMetaType<Result>();
	QMetaType::registerConverter<Result, QString>(&Result::toString);
	// qRegisterMetaType<Uuid>();
	// QMetaType::registerConverter<Uuid, QString>(&Uuid::toString);
	// QMetaType::registerConverter<Uuid, QUuid>(&Uuid::toQuuid);
	qRegisterMetaType<DeviceInfo>();
	qRegisterMetaType<MediaService>();
	// qRegisterMetaType<EventService>();
	// qRegisterMetaType<OnvifFilterMessageExpression>();
}

void AdvancedQmlApplicationEngine::registerQmlTypes() {

	// QML Singletons
	qmlRegisterSingletonType<Window>("org.global", 1, 0, "Window", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine);
		Q_UNUSED(scriptEngine);

		auto window = Window::getGlobal();
		QQmlEngine::setObjectOwnership(window, QQmlEngine::CppOwnership);
		return window;
	});

	qmlRegisterSingletonType<DeviceManager>("org.onvif.device", 1, 0, "DeviceManager",
	                                        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		                                        Q_UNUSED(engine);
		                                        Q_UNUSED(scriptEngine);

		                                        auto manager = DeviceM;
		                                        QQmlEngine::setObjectOwnership(manager, QQmlEngine::CppOwnership);
		                                        return manager;
	                                        });
	/*
	  qmlRegisterSingletonType<MediaManager>("org.onvif.media", 1, 0, "MediaManager",
	                                         [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
	                                           Q_UNUSED(engine);
	                                           Q_UNUSED(scriptEngine);

	                                           auto manager = MediaManager::getGlobal();
	                                           QQmlEngine::setObjectOwnership(manager, QQmlEngine::CppOwnership);
	                                           return manager;
	                                         });
	  qmlRegisterSingletonType<MediaManager>("org.onvif.event", 1, 0, "EventManager",
	                                         [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
	                                           Q_UNUSED(engine);
	                                           Q_UNUSED(scriptEngine);

	                                           auto manager = EventM;
	                                           QQmlEngine::setObjectOwnership(manager, QQmlEngine::CppOwnership);
	                                           return manager;
	                                         });
	                                         */

	// QML types
	qmlRegisterType<SortFilterProxyModel>("org.onvif.common", 1, 0, "SortFilterProxyModel");
	qmlRegisterUncreatableMetaObject(Enums::staticMetaObject, "org.onvif.common", 1, 0, "Enums", "Error: only enums");
	qmlRegisterType<DeviceDiscoveryModel>("org.onvif.device", 1, 0, "DeviceDiscoverModel");
	qmlRegisterType<DevicesModel>("org.onvif.device", 1, 0, "DeviceModel");
	qmlRegisterType<MediaProfilesModel>("org.onvif.media", 1, 0, "MediaProfilesModel");
	// qmlRegisterType<EventHandlerModel>("org.onvif.event", 1, 0, "EventHandlerModel");
	// qmlRegisterType<EventSourceModel>("org.onvif.event", 1, 0, "EventSourceModel");
	// qmlRegisterType<EventBindingModel>("org.onvif.event", 1, 0, "EventBindingModel");
	qmlRegisterType<NotItem>("org.onvif.event", 1, 0, "NotItem");
	qmlRegisterType<ValItem>("org.onvif.event", 1, 0, "ValItem");
	qmlRegisterUncreatableType<PropertyInfo>("org.onvif.event", 1, 0, "PropertyInfo", "Can't be created in QML");

	// QML future types
	QuickFuture::registerType<DetailedResult<QUuid>>(
	 [](DetailedResult<QUuid> value) -> QVariant { return value.isSuccess() ? value.GetResultObject() : QVariant(); });

	QuickFuture::registerType<DetailedResult<QUrl>>(
	 [](DetailedResult<QUrl> value) -> QVariant { return value.isSuccess() ? value.GetResultObject() : QVariant(); });

	QuickFuture::registerType<DetailedResult<QList<MediaProfile>>>([](DetailedResult<QList<MediaProfile>> value) -> QVariant {
		return value.isSuccess() ? QVariant::fromValue(value.GetResultObject()) : QVariant();
	});
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
			auto qmlFiles = dir.entryList({"*.qml"}, QDir::Files);
			qInfo() << "Enabled hot reloading for following dir:" << dir.absolutePath();
			mpWatcher->addPath(dir.absolutePath());
			for(const auto &qmlFile : qmlFiles) {
				qInfo() << "Enabled hot reloading for following file:" << dir.absoluteFilePath(qmlFile);
				mpWatcher->addPath(dir.absoluteFilePath(qmlFile));
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
