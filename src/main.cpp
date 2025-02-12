#include "App.h"
// #include "BackgroundService.h"
#include "LogMessageHandler.h"
#include "QtApplicationBase.h"
#include "mdk/global.h"
#include "stdsoap2.h"
#include <QFontDatabase>
#include <QIcon>
#include <QTextStream>
#include <QtGlobal>
extern "C" {
#include "libavutil/log.h"
#include "openssl/ssl.h"
}

#ifdef Q_OS_WINDOWS
#include "qt_windows.h"
#elifdef Q_OS_ANDROID
#include <QJniEnvironment>
#include <QJniObject>
#include <QtCore/private/qandroidextras_p.h>
extern "C" {
#include "libavcodec/jni.h"
}
#endif

Q_LOGGING_CATEGORY(libavlog, "libav")
Q_LOGGING_CATEGORY(mdklog, "mdk")

static void ffmpeg_log_callback(void *arb, int level, const char *fmt, va_list vl) {

	Q_UNUSED(arb);

	auto str = QString::fromLocal8Bit(fmt);
	str.remove('\n');
	if(!str.isEmpty()) {
		switch(level) {
			case AV_LOG_VERBOSE:
			case AV_LOG_DEBUG:
			case AV_LOG_TRACE:
				qCDebug(libavlog).noquote() << QString::vasprintf(qPrintable(str), vl);
				break;
			case AV_LOG_INFO:
				qCInfo(libavlog).noquote() << QString::vasprintf(qPrintable(str), vl);
				break;
			case AV_LOG_WARNING:
				qCWarning(libavlog).noquote() << QString::vasprintf(qPrintable(str), vl);
				break;
			case AV_LOG_ERROR:
			case AV_LOG_FATAL:
			case AV_LOG_PANIC:
				qCCritical(libavlog).noquote() << QString::vasprintf(qPrintable(str), vl);
				break;
			case AV_LOG_QUIET:
			default:
				break;
		}
	}
}

static void mdk_log_callback(mdk::LogLevel level, const char *msg) {

	if(msg) {
		auto str = QString::fromLocal8Bit(msg);
		str.remove('\n');
		switch(level) {
			case mdk::All:
			case mdk::Debug:
				qCDebug(mdklog).noquote() << qPrintable(str);
				break;
			case mdk::Info:
				qCInfo(mdklog).noquote() << qPrintable(str);
				break;
			case mdk::Warning:
				qCWarning(mdklog).noquote() << qPrintable(str);
				break;
			case mdk::Error:
				qCCritical(mdklog).noquote() << qPrintable(str);
				break;
			case mdk::Off:
			default:
				break;
		}
	}
}


int main(int argc, char *argv[]) {

	LogMessageHandler::prepare(QStandardPaths::writableLocation(QStandardPaths::TempLocation));

	qunsetenv("QT_STYLE_OVERRIDE");
	qunsetenv("QT_QUICK_CONTROLS_STYLE");
	// qputenv("QT_DEBUG_PLUGINS", QByteArray("1"));

	mdk::setLogHandler(mdk_log_callback);
#ifdef Q_OS_ANDROID
	qInfo() << "Setting mdk jvm";
	mdk::SetGlobalOption("JavaVM", QJniEnvironment::javaVM());
	av_jni_set_java_vm(QJniEnvironment::javaVM(), nullptr);
#endif

	int version = 0;
	mdk::GetGlobalOption("ffmpeg.version", &version);
	mdk::SetGlobalOption("log", mdk::Info);

	qInfo() << "Using ffmpeg" << version;

	av_log_set_level(AV_LOG_DEBUG);
	av_log_set_callback(ffmpeg_log_callback);

	auto dedicatedService = false;

	for(auto i = 0; i < argc; i++) {
		if(strcmp(argv[i], "-service") == 0) {
			dedicatedService = true;
		}
	}

	if(dedicatedService) {
		// BackgroundService service;
		// return service.start(argc, argv);
	} else {
		App app;
		return app.start(argc, argv);
	}
}
