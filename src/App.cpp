#include <QtGlobal>
#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif
#include "AdvancedQmlApplicationEngine.h"
#include "App.h"
#include "DeviceDiscoveryModel.h"
#include "DeviceInfo.h"
#include "DeviceManager.h"
#include "DevicesModel.h"
#include "DiscoveryMatch.h"
#include "Error.h"
#include "IconImageProvider.h"
#include "ImageProvider.h"
#include "LogMessageHandler.h"
#include "MediaPlayer.h"
#include "MediaProfile.h"
#include "MediaProfilesModel.h"
#include "MediaService.h"
#include "MonitorGridModel.h"
#include "OnvifMessageFilterItems.h"
#include "PropertyInfo.h"
#include "Result.h"
#include "Roles.h"
#include "SortFilterProxyModel.h"
#include "Window.h"
#include "info.h"
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFontDatabase>
#include <QFuture>
#include <QIcon>
#include <QQuickStyle>
#include <QSettings>
#include <QStandardPaths>
#include <QStyleHints>
#include <QSysInfo>

Q_DECLARE_METATYPE(DetailedResult<QUrl>)
Q_DECLARE_METATYPE(QFuture<DetailedResult<QUrl>>)
Q_DECLARE_METATYPE(DetailedResult<QUuid>)
Q_DECLARE_METATYPE(QFuture<DetailedResult<QUuid>>)

App::App() = default;

int App::start(int &argc, char **argv, bool headless) {

	Q_ASSERT(!QCoreApplication::instance());

	qInfo() << "Starting app using Qt" << qVersion();
	QSettings::setDefaultFormat(QSettings::IniFormat);
	QCoreApplication::setApplicationName(INFO_PROJECTNAME);
	QCoreApplication::setApplicationVersion(QString("%1.%2.%3").arg(INFO_VERSION_MAJOR).arg(INFO_VERSION_MINOR).arg(INFO_VERSION_PATCH));
	QCoreApplication::setOrganizationName("");
	QCoreApplication::setOrganizationDomain("com.github.Tereius");
	QCoreApplication::setLibraryPaths(QString::fromLocal8Bit(QT_PLUGIN_PATHS).split(',', Qt::SkipEmptyParts) +
	                                  QCoreApplication::libraryPaths());
	QCoreApplication::addLibraryPath("./");
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#ifdef QT_DEBUG
	QDir storagePath(QCoreApplication::applicationDirPath());
#else
	QDir storagePath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
#endif
	qInfo() << "Using writableLocation:" << storagePath.absolutePath();
	QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, storagePath.absolutePath());
	QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, storagePath.absolutePath());
	LogMessageHandler logMessageHandler;
	logMessageHandler.prepare(storagePath.absolutePath());

	QSettings settings;
	settings.setValue("version", QString("%1.%2.%3").arg(INFO_VERSION_MAJOR).arg(INFO_VERSION_MINOR).arg(INFO_VERSION_PATCH));
	settings.sync();

#ifdef Q_OS_ANDROID
	QGuiApplication app(argc, argv);
#else
	// Default to org.kde.desktop style unless the user forces another style
	QApplication app(argc, argv);
#endif

	QIcon::setThemeName("onvif");

	QCommandLineParser parser;
	parser.addOption({"u", "Uninstall persistent data."});
	parser.parse(QCoreApplication::arguments());

	if(parser.isSet("u")) {
		// Delete the persistence
		return storagePath.removeRecursively() ? 0 : 1;
	}

	// Install fonts
	QFontDatabase fontDb;
	QDir fontsDir(":/fonts");
	for(const auto &entry : fontsDir.entryList({"*.ttf", "*.otf"}, QDir::Files)) {
		auto fontId = QFontDatabase::addApplicationFont(fontsDir.absoluteFilePath(entry));
		if(fontId >= 0) {
			auto fontFamily = QFontDatabase::applicationFontFamilies(fontId);
			qInfo() << "Font registered:" << QFontDatabase::applicationFontFamilies(fontId);
		} else {
			qWarning() << "Couldn't install font.";
		}
	}

	App::registerMetatypes();

	auto deviceManager = new DeviceManager(qApp);
	auto devicesModel = new DevicesModel(deviceManager);

	if(!headless) {
		AdvancedQmlApplicationEngine qmlEngine;
		App::registerQmlTypes();

		// Image provider
		qmlEngine.addImageProvider("icons", new IconImageProvider(qApp));
		qmlEngine.addImageProvider("profile", new ImageProvider(deviceManager));

		// QML Singletons
		qmlRegisterSingletonType<Window>("org.global", 1, 0, "Window", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
			Q_UNUSED(engine)
			Q_UNUSED(scriptEngine)

			auto window = Window::getGlobal();
			QQmlEngine::setObjectOwnership(window, QQmlEngine::CppOwnership);
			return window;
		});

		qmlRegisterSingletonType<DeviceManager>("org.onvif.device", 1, 0, "DeviceManager",
		                                        [deviceManager](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
			                                        Q_UNUSED(engine)
			                                        Q_UNUSED(scriptEngine)

			                                        QQmlEngine::setObjectOwnership(deviceManager, QQmlEngine::CppOwnership);
			                                        return deviceManager;
		                                        });

#ifdef QT_DEBUG
		auto qmlMainFile = QString("%1/root.qml").arg(QML_BASE_PATH);
		if(QFile::exists(qmlMainFile)) {
			qInfo() << "QML hot reloading enabled";
			qmlEngine.setHotReload(true);
			qmlEngine.loadRootItem(qmlMainFile);
		} else {
			qmlEngine.setHotReload(false);
			qmlEngine.loadRootItem("qrc:///gui/root.qml");
		}
#else
		qmlEngine.setHotReload(false);
		qmlEngine.loadRootItem("qrc:///gui/root.qml");
#endif
		return app.exec();
	}
	return app.exec();
}

void App::registerMetatypes() {

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
	qmlRegisterType<SortFilterProxyModel>("org.onvif.common", 1, 0, "SortFilterProxyModel");
	qmlRegisterUncreatableMetaObject(Enums::staticMetaObject, "org.onvif.common", 1, 0, "Enums", "Error: only enums");
	qmlRegisterType<DeviceDiscoveryModel>("org.onvif.device", 1, 0, "DeviceDiscoverModel");
	qmlRegisterType<DevicesModel>("org.onvif.device", 1, 0, "DevicesModel");
	qmlRegisterType<MediaProfilesModel>("org.onvif.media", 1, 0, "MediaProfilesModel");
	// qmlRegisterType<EventHandlerModel>("org.onvif.event", 1, 0, "EventHandlerModel");
	// qmlRegisterType<EventSourceModel>("org.onvif.event", 1, 0, "EventSourceModel");
	// qmlRegisterType<EventBindingModel>("org.onvif.event", 1, 0, "EventBindingModel");
	qmlRegisterType<NotItem>("org.onvif.event", 1, 0, "NotItem");
	qmlRegisterType<ValItem>("org.onvif.event", 1, 0, "ValItem");
	qmlRegisterType<MediaPlayer>("org.onvif.media", 1, 0, "MediaPlayer");
	qmlRegisterType<MonitorGridModel>("org.onvif.monitoring", 1, 0, "MonitorGridModel");
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
