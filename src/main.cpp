#include "AdvancedQmlApplicationEngine.h"
#include "App.h"
#include "EventHandlerModel.h"
#include "LogMessageHandler.h"
#include "OnvifDiscovery.h"
#include "OnvifMessageFilterItems.h"
#include "QtApplicationBase.h"
#include "mdk/global.h"
#include <QCommandLineParser>
#include <QFile>
#include <QFontDatabase>
#include <QIcon>
#include <QTextStream>
#include <QtGlobal>
#include <QtPlugin>
extern "C" {
#include "libavutil/log.h"
}

#ifdef Q_OS_WINDOWS
#include "qt_windows.h"
#elifdef Q_OS_ANDROID
#include <QJniEnvironment>
extern "C" {
#include "libavcodec/jni.h"
}
#endif

Q_LOGGING_CATEGORY(libav, "libav")

void ffmpeg_log_callback(void *arb, int level, const char *fmt, va_list vl) {

	Q_UNUSED(arb);

	auto str = QString::fromLocal8Bit(fmt);
	str.remove('\n');
	if(!str.isEmpty()) {
		switch(level) {
			case AV_LOG_VERBOSE:
			case AV_LOG_DEBUG:
			case AV_LOG_TRACE:
				qCDebug(libav).noquote() << "FFmpeg:" << QString::vasprintf(qPrintable(str), vl);
				break;
			case AV_LOG_INFO:
				qCInfo(libav).noquote() << "FFmpeg:" << QString::vasprintf(qPrintable(str), vl);
				break;
			case AV_LOG_WARNING:
				qCWarning(libav).noquote() << "FFmpeg:" << QString::vasprintf(qPrintable(str), vl);
				break;
			case AV_LOG_ERROR:
			case AV_LOG_FATAL:
			case AV_LOG_PANIC:
				qCCritical(libav).noquote() << "FFmpeg:" << QString::vasprintf(qPrintable(str), vl);
				break;
			case AV_LOG_QUIET:
			default:
				break;
		}
	}
}

void mdk_log_callback(mdk::LogLevel level, const char *msg) {

	if(msg) {
		auto str = QString::fromLocal8Bit(msg);
		str.remove('\n');
		switch(level) {
			case mdk::All:
			case mdk::Debug:
				qCDebug(libav).noquote() << qPrintable(str);
				break;
			case mdk::Info:
				qCInfo(libav).noquote() << qPrintable(str);
				break;
			case mdk::Warning:
				qCWarning(libav).noquote() << qPrintable(str);
				break;
			case mdk::Error:
				qCCritical(libav).noquote() << qPrintable(str);
				break;
			case mdk::Off:
			default:
				break;
		}
	}
}


int main(int argc, char *argv[]) {

	LogMessageHandler::prepare(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
	mdk::setLogHandler(mdk_log_callback);

	qunsetenv("QT_STYLE_OVERRIDE");
	qunsetenv("QT_QUICK_CONTROLS_STYLE");
	// qputenv("QT_DEBUG_PLUGINS", QByteArray("1"));

#ifdef Q_OS_ANDROID
	qInfo() << "Setting mdk jvm";
	mdk::SetGlobalOption("JavaVM", QJniEnvironment::javaVM());
	av_jni_set_java_vm(QJniEnvironment::javaVM(), nullptr);
#else
	qInfo() << "Using mdk avutil_lib" << LIB_AVUTIL_NAME;
	mdk::SetGlobalOption("avutil_lib", LIB_AVUTIL_NAME);
	qInfo() << "Using mdk avcodec_lib" << LIB_AVCODEC_NAME;
	mdk::SetGlobalOption("avcodec_lib", LIB_AVCODEC_NAME);
	qInfo() << "Using mdk avformat_lib" << LIB_AVFORMAT_NAME;
	mdk::SetGlobalOption("avformat_lib", LIB_AVFORMAT_NAME);
	qInfo() << "Using mdk avfilter_lib" << LIB_AVFILTER_NAME;
	mdk::SetGlobalOption("avfilter_lib", LIB_AVFILTER_NAME);
	qInfo() << "Using mdk swresample_lib" << LIB_SWRESAMPLE_NAME;
	mdk::SetGlobalOption("swresample_lib", LIB_SWRESAMPLE_NAME);
	qInfo() << "Using mdk swscale_lib" << LIB_SWSCALE_NAME;
	mdk::SetGlobalOption("swscale_lib", LIB_SWSCALE_NAME);
#endif

	int version = 0;
	mdk::GetGlobalOption("ffmpeg.version", &version);
	mdk::setLogLevel(mdk::Info);

	qInfo() << "Using ffmpeg" << version;

	// av_log_set_level(AV_LOG_DEBUG);
	// av_log_set_callback(ffmpeg_log_callback);

	App app;
	return app.start(argc, argv);
}
