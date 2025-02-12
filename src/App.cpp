#include "App.h"
#include "AdvancedQmlApplicationEngine.h"
#include "DeviceDiscoveryModel.h"
#include "DeviceInfo.h"
#include "DeviceManager.h"
#include "DeviceProbe.h"
#include "DiscoveryMatch.h"
#include "Enums.h"
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
#include "SecretsManager.h"
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
	qputenv("QT_LOGGING_RULES", "qt.remoteobjects.debug=true");
	qunsetenv("QT_STYLE_OVERRIDE");
	qunsetenv("QT_QUICK_CONTROLS_STYLE");

	QtApplicationBase<QApplication> app(argc, argv);

	// always make sure that the fallbackSettings is equal to the one in BackgroundService. App and Service have to access the same secret
	// store
	SecretsManager::setNamespace(QString("%1.%2").arg(QCoreApplication::organizationDomain(), INFO_PROJECTNAME));
	SecretsManager::setFallbackSettings(
	 std::make_unique<QSettings>(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), "Secrets"));

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
		qmlEngine.loadRootItem(qmlMainFile, false);
	} else {
		qmlEngine.setHotReload(false);
		qmlEngine.loadRootItem("qrc:/qt/qml/Onvif/Onvif/main.qml", false);
	}
#else
	qmlEngine.setHotReload(false);
	qmlEngine.loadRootItem("qrc:/qt/qml/Onvif/Onvif/main.qml", false);
#endif

	initSecretsManager();
	App::initBackgroundService();

	// By writing a dummy secret we know if we can use the OS secrets manager or if we use the fallback settings file
	// If we use the OS secrets manager it will also be unlocked
	auto dummySecret = QString("probe_%1").arg(INFO_PROJECTID);
	SecretsManager::writeSecret(
	 dummySecret, QString::number(INFO_PROJECTID),
	 [this, dummySecret](bool fallback) {
		 mOsSecretsManager = !fallback;
		 emit hasOsSecretsManagerChanged(mOsSecretsManager);
		 SecretsManager::deleteSecret(dummySecret, []() {}, this);

		 // Initialize the DeviceManager after we know we can successfully read secrets
		 DeviceManager::getInstance()->initialize();
	 },
	 this);

	return app.start();
}

void App::registerMetatypes() {

	// qRegisterMetaType<DetailedResult<RtspStream>>();
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

	static auto userAgent = QString("%1/%2 (%5; %6)")
	                         .arg(INFO_PROJECTNAME)
	                         .arg(INFO_VERSIONSTRING)
	                         .arg(QSysInfo::prettyProductName(), QSysInfo::currentCpuArchitecture());
	return userAgent;
}

void App::enableBackgroundService(bool enable) {

	if(enable) {
		App::startBackgroundService();
	} else {
		App::stopBackgroundService();
	}
	QSettings settings;
	settings.beginGroup("backgroundService");
	settings.setValue("enabled", enable);
	emit backgroundServiceEnableChanged(enable);
}

bool App::isBackgroundServiceEnabled() {

	QSettings settings;
	settings.beginGroup("backgroundService");
	return settings.value("enabled", false).toBool();
}

bool App::hasOsSecretsManager() const {

	return mOsSecretsManager;
}

void App::initSecretsManager() {}

void App::initBackgroundService() {

	QSettings settings;
	settings.beginGroup("backgroundService");
	const auto enable = settings.value("enabled", false).toBool();
	if(enable) {
		App::startBackgroundService();
	}
}

void App::startBackgroundService() {

#ifdef Q_OS_ANDROID
	auto requestResult = QtAndroidPrivate::requestPermission("android.permission.POST_NOTIFICATIONS");
	requestResult.waitForFinished();
	if(requestResult.result() == QtAndroidPrivate::Authorized) {
		auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());
		QAndroidIntent serviceIntent(activity.object(), "com/github/tereius/onvifmonitor/ForegroundService");
		QJniObject result = activity.callObjectMethod("startForegroundService", "(Landroid/content/Intent;)Landroid/content/ComponentName;",
		                                              serviceIntent.handle().object());
	} else {
		qWarning() << "Failed to start background service: missing POST_NOTIFICATIONS permission";
	}
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

bool App::mOsSecretsManager = true;
