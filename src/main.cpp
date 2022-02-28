#include "App.h"
#include "EventHandlerModel.h"
#include "MediaProfile.h"
#include "OnvifDiscovery.h"
#include "OnvifMessageFilterItems.h"
#include <QCommandLineParser>
#include <QFile>
#include <QFontDatabase>
#include <QGlobalStatic>
#include <QIcon>
#include <QString>
#include <QTextStream>
#include <QtAV/QtAV>
#include <QtGlobal>
#include <QtPlugin>

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

int main(int argc, char *argv[]) {
	
	//#ifndef NDEBUG
	//	QString storagePath = QCoreApplication::applicationDirPath();
	//#else

	//#endif

	// open log file
	if(log_file.size() > 5000) {
		log_file.resize(0);
		qWarning() << "Log file was resized.";
	}

	App app;
	return app.start(argc, argv);
}
