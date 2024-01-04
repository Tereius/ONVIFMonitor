#include "App.h"
#include "AdvancedQmlApplicationEngine.h"
#include "DeviceDiscoveryModel.h"
#include "DeviceInfo.h"
#include "DeviceManager.h"
#include "DeviceProbe.h"
#include "DiscoveryMatch.h"
#include "Error.h"
#include "IconImageProvider.h"
#include "ImageProvider.h"
#include "MediaPlayer.h"
#include "MediaProfile.h"
#include "MediaProfilesModel.h"
#include "MediaService.h"
#include "MonitorGridModel.h"
#include "OnvifMessageFilterItems.h"
#include "PropertyInfo.h"
#include "QtApplicationBase.h"
#include "QuickFuture/quickfuture.h"
#include "Result.h"
#include "Roles.h"
#include "SortFilterProxyModel.h"
#include "Window.h"
#include "info.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFontDatabase>
#include <QFontInfo>
#include <QFuture>
#include <QIcon>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QStyleHints>
#include <QtGlobal>
#ifdef Q_OS_ANDROID
#include <QJniEnvironment>
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>
#endif

Q_DECLARE_METATYPE(DetailedResult<QUrl>)
Q_DECLARE_METATYPE(QFuture<DetailedResult<QUrl>>)
Q_DECLARE_METATYPE(DetailedResult<QUuid>)
Q_DECLARE_METATYPE(QFuture<DetailedResult<QUuid>>)

App::App() = default;

int App::start(int &argc, char **argv) {

	Q_ASSERT(!QCoreApplication::instance());

	qputenv("QT_MEDIA_BACKEND", "ffmpeg");
	qunsetenv("QT_STYLE_OVERRIDE");
	qunsetenv("QT_QUICK_CONTROLS_STYLE");

	QtApplicationBase<QApplication> app(argc, argv);

	QIcon::setThemeName("onvif");

	App::registerMetatypes();

	// auto deviceManager = new DeviceManager(qApp);
	// auto devicesModel = new DevicesModel(deviceManager);

	AdvancedQmlApplicationEngine qmlEngine;
	App::registerQmlTypes();
	QIcon::setThemeName("material");

	// Image provider
	qmlEngine.addImageProvider("icons", new IconImageProvider());
	qmlEngine.addImageProvider("profile", new ImageProvider(DeviceManager::getInstance()));

	// QML Singletons
	qmlRegisterSingletonType<Window>("org.global", 1, 0, "Window", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		auto window = Window::getGlobal();
		QQmlEngine::setObjectOwnership(window, QQmlEngine::CppOwnership);
		return window;
	});

#ifdef QT_DEBUG
	auto qmlMainFile = QString("Onvif/Onvif/main.qml");
	if(QFile::exists(qmlMainFile)) {
		qInfo() << "QML hot reloading enabled";
		qmlEngine.setHotReload(true);
		qmlEngine.loadRootItem(qmlMainFile);
	} else {
		qmlEngine.setHotReload(false);
		qmlEngine.loadRootItem("qrc:/qt/qml/Onvif/Onvif/main.qml");
	}
#else
	qmlEngine.setHotReload(false);
	qmlEngine.loadRootItem("qrc:/qt/qml/Onvif/Onvif/main.qml");
#endif
	DeviceManager::getInstance()->initialize();
	return app.exec();
}

void App::registerMetatypes() {

	qRegisterMetaType<MediaProfile>();
	qRegisterMetaType<DiscoveryMatch>();
	qRegisterMetaType<ProfileId>();
	qRegisterMetaType<Error>();
	QMetaType::registerConverter<ProfileId, QString>(&ProfileId::toString);
	qRegisterMetaType<Result>();
	qRegisterMetaType<QFuture<Result>>();
	QMetaType::registerConverter<Result, QString>(&Result::toString);
	// qRegisterMetaType<Uuid>();
	// QMetaType::registerConverter<Uuid, QString>(&Uuid::toString);
	// QMetaType::registerConverter<Uuid, QUuid>(&Uuid::toQuuid);
	qRegisterMetaType<DeviceInfo>();
	qRegisterMetaType<MediaService>();
	// qRegisterMetaType<EventService>();
	// qRegisterMetaType<OnvifFilterMessageExpression>();
}

void App::registerQmlTypes() {

	// QML Singletons
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

	// qmlRegisterType<EventHandlerModel>("org.onvif.event", 1, 0, "EventHandlerModel");
	// qmlRegisterType<EventSourceModel>("org.onvif.event", 1, 0, "EventSourceModel");
	// qmlRegisterType<EventBindingModel>("org.onvif.event", 1, 0, "EventBindingModel");

	// qmlRegisterType<NotItem>("org.onvif.event", 1, 0, "NotItem");
	// qmlRegisterType<ValItem>("org.onvif.event", 1, 0, "ValItem");


	// QML libONVIF tapes
	// qmlRegisterUncreatableType<DeviceProbe>("org.onvif.common", 1, 0, "DeviceProbe", "Can't be created in QML");

	// QML future types
	QuickFuture::registerType<Result>([](Result value) -> QVariant { return QVariant(); });

	QuickFuture::registerType<DetailedResult<QUuid>>(
	 [](DetailedResult<QUuid> value) -> QVariant { return value.isSuccess() ? value.GetResultObject() : QVariant(); });

	QuickFuture::registerType<DetailedResult<QUrl>>(
	 [](DetailedResult<QUrl> value) -> QVariant { return value.isSuccess() ? value.GetResultObject() : QVariant(); });

	QuickFuture::registerType<DetailedResult<QList<MediaProfile>>>([](DetailedResult<QList<MediaProfile>> value) -> QVariant {
		return value.isSuccess() ? QVariant::fromValue(value.GetResultObject()) : QVariant();
	});
}

QString App::getDefaultUserAgent() {

	static auto userAgent = QString("%1/%2.%3.%4 (%5; %6)")
	                         .arg(INFO_PROJECTNAME)
	                         .arg(INFO_VERSION_MAJOR)
	                         .arg(INFO_VERSION_MINOR)
	                         .arg(INFO_VERSION_PATCH)
	                         .arg(QSysInfo::prettyProductName())
	                         .arg(QSysInfo::currentCpuArchitecture());
	return userAgent;
}

void App::startBackgroundService() {

#ifdef Q_OS_ANDROID
	auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
	QAndroidIntent serviceIntent(activity.object(), "com/github/tereius/onvifmonitor/ForegroundService");
	QJniObject result = activity.callObjectMethod("startForegroundService", "(Landroid/content/Intent;)Landroid/content/ComponentName;",
	                                              serviceIntent.handle().object());
#endif
}

void App::stopBackgroundService() {

#ifdef Q_OS_ANDROID
	auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
	QAndroidIntent serviceIntent(activity.object(), "com/github/tereius/onvifmonitor/ForegroundService");
	serviceIntent.putExtra("STOP_FOREGROUND_SERVICE", QString("true"));
	QJniObject result = activity.callObjectMethod("startForegroundService", "(Landroid/content/Intent;)Landroid/content/ComponentName;",
	                                              serviceIntent.handle().object());
#endif
}
