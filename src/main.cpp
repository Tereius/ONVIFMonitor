#include "DeviceDiscoveryModel.h"
#include "DeviceInfo.h"
#include "DevicesModel.h"
#include "Error.h"
#include "EventBindingModel.h"
#include "EventHandler.h"
#include "EventHandlerModel.h"
#include "AdvancedQmlApplicationEngine.h"
#include "MediaManager.h"
#include "MediaProfile.h"
#include "MediaProfilesModel.h"
#include "OnvifDiscovery.h"
#include "OnvifMessageFilterItems.h"
#include "ProfileId.h"
#include "PropertyInfo.h"
#include "Result.h"
#include "info.h"

#include <QCommandLineParser>
#include <QtPlugin>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGlobalStatic>
#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QQuickStyle>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>
#include <QtAV/QtAV>

#ifdef QT_OS_WINDOWS
#include "qt_windows.h"
#endif

namespace {
static QFile log_file;
static QMutex mutex;
} // namespace

static void fatal_dbug_msg_handler(QtMsgType type, const QMessageLogContext &rContext, const QString &rMessage) {

	QMutexLocker mutex_locker(&mutex);

	if(type == QtFatalMsg) {
		QCoreApplication::instance()->exit(1);
	}
}

static void dbug_msg_handler(QtMsgType type, const QMessageLogContext &rContext, const QString &rMessage) {

	QMutexLocker mutex_locker(&mutex);

	auto text = QString("[%1]").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss"));

	switch(type) {
		case QtDebugMsg:
#ifdef NDEBUG
			text += QString(" DEBUG    : %1").arg(rMessage);
#else
			text += QString(" DEBUG    (%1: %2): %3").arg(rContext.file).arg(rContext.line).arg(rMessage);
#endif // NDEBUG
			break;

		case QtInfoMsg:
#ifdef NDEBUG
			text += QString(" INFO    : %1").arg(rMessage);
#else
			text += QString(" INFO     (%1: %2): %3").arg(rContext.file).arg(rContext.line).arg(rMessage);
#endif // NDEBUG
			break;

		case QtWarningMsg:
#ifdef NDEBUG
			text += QString(" WARNING  : %1").arg(rMessage);
#else
			text += QString(" WARNING  (%1: %2): %3").arg(rContext.file).arg(rContext.line).arg(rMessage);
#endif
			break;

		case QtCriticalMsg:
#ifdef NDEBUG
			text += QString(" CRITICAL : %1").arg(rMessage);
#else
			text += QString(" CRITICAL (%1: %2): %3").arg(rContext.file).arg(rContext.line).arg(rMessage);
#endif
			break;

		case QtFatalMsg:
#ifdef NDEBUG
			text += QString(" FATAL    : %1").arg(rMessage);
#else
			text += QString(" FATAL    (%1: %2): %3").arg(rContext.file).arg(rContext.line).arg(rMessage);
#endif
			text += "\nApplication will be terminated due to Fatal-Error.";
			break;
	}

	QTextStream tStream(&log_file);
	tStream << text << "\n";

#ifdef _WIN32
	OutputDebugString(reinterpret_cast<LPCSTR>(qPrintable(text.append("\n"))));
#elif defined Q_OS_LINUX || defined Q_OS_MAC
	auto txt = text.append("\n").toStdString();
	fputs(txt.c_str(), stderr);
	fflush(stderr);
#endif // OS_WIN32

	if(type == QtFatalMsg) {
		QCoreApplication::instance()->exit(1);
	}
}

void writeSettings() {

	QSettings settings;
	settings.setValue("version", QString("%1.%2.%3").arg(INFO_VERSION_MAJOR).arg(INFO_VERSION_MINOR).arg(INFO_VERSION_PATCH));
	settings.sync();
}

int main(int argc, char *argv[]) {

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

	//#ifndef NDEBUG
	//	QString storagePath = QCoreApplication::applicationDirPath();
	//#else
	QDir storagePath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
	auto path = storagePath.absolutePath();
	QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, storagePath.absolutePath());
	QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, storagePath.absolutePath());
	writeSettings();
	//#endif

#ifdef Q_OS_ANDROID
	QGuiApplication app(argc, argv);
	// QQuickStyle::setStyle(QStringLiteral("Material"));
#else
	// Default to org.kde.desktop style unless the user forces another style
	QApplication app(argc, argv);
#endif

	// QQuickStyle::setStyle("Kirigami.BasicThemeDefinition");
	QIcon::setThemeName("onvif");

	QCommandLineParser parser;
	parser.addOption({"u", "Uninstall persistent data."});
	parser.parse(app.arguments());

	if(parser.isSet("u")) {

		// Delete the persistence
		exit(storagePath.removeRecursively() ? 0 : 1);
	}

	QFontDatabase fontDb;
	if(fontDb.families().isEmpty()) {
		auto fontId = QFontDatabase::addApplicationFont(":/fonts/LiberationSans-Regular.ttf");
		if(fontId >= 0) {
			qInfo() << "Fallback font registered:" << QFontDatabase::applicationFontFamilies(fontId);
			QFont font(QFontDatabase::applicationFontFamilies(fontId).first());
			QGuiApplication::setFont(font);
		} else {
			qWarning() << "Couldn't install fallback font.";
		}
	}

	// open log file
	if(log_file.size() > 5000) {
		log_file.resize(0);
		qWarning() << "Log file was resized.";
	}

	/*
	log_file.setFileName(storagePath.absoluteFilePath("test.log"));
	auto success = log_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	if(success) {
	  qInstallMessageHandler(dbug_msg_handler);
	} else {
	  qWarning() << "Couldn't open log file: " << log_file.errorString() << log_file.fileName();
	  qInstallMessageHandler(fatal_dbug_msg_handler);
	}*/

	// Event handler
	// EventManager::registerEventHandler<LogEventHandler>();

	// Event sources
	// EventManager::registerEventSource<TimerEventSource>();
	// EventManager::registerEventSource<OnvifDeviceMessage>();
	// EventManager::registerEventSource<FilteredOnvifDeviceMessage>();

	AdvancedQmlApplicationEngine engine;
#if !defined(NDEBUG)
	auto qmlMainFile = QString("%1/main.qml").arg(QML_BASE_PATH);
	if(QFile::exists(qmlMainFile)) {
		qInfo() << "QML hot reloading enabled";
		engine.setHotReload(true);
		engine.loadRootItem(qmlMainFile);
	} else {
		engine.setHotReload(false);
		engine.loadRootItem("qrc:///gui/main.qml");
	}
#else
	engine.setHotReload(false);
	engine.loadRootItem("qrc:///gui/main.qml");
#endif

	return app.exec();
}
