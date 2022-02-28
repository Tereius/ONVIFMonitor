#include "LogMessageHandler.h"
#include <QRegularExpression>
#ifdef _WIN32
#include <qt_windows.h> // we need this for OutputDebugString()
#endif
#ifdef Q_OS_ANDROID
#include <android/log.h>
#endif
#include <QCoreApplication>
#include <QDateTime>
#include <QMutex>
#include <QSettings>
#include <algorithm>
#include <mdk/global.h>
extern "C" {
#include "libavutil/log.h"
}


#define MAX_CAT_LENGTH 14
Q_LOGGING_CATEGORY(libav, "libav")
Q_LOGGING_CATEGORY(mdklog, "mdk")

namespace {
static QFile log_file;
static QString log_dir;
static QMutex mutex;
} // namespace

QTextStream &qStdOut() {
	static QTextStream ts(stdout);
	return ts;
}

void mdk_log_callback(mdk::LogLevel lvl, const char *str) {

	auto fmtStr = QString::fromLocal8Bit(str);
	fmtStr.remove('\n');
	if(str) {
		switch(lvl) {
			case mdk::LogLevel::All:
				qCDebug(mdklog).noquote() << "mdk:" << qPrintable(fmtStr);
				break;
			case mdk::LogLevel::Debug:
				qCDebug(mdklog).noquote() << "mdk:" << qPrintable(fmtStr);
				break;
			case mdk::LogLevel::Info:
				qCInfo(mdklog).noquote() << "mdk:" << qPrintable(fmtStr);
				break;
			case mdk::LogLevel::Warning:
				qCWarning(mdklog).noquote() << "mdk:" << qPrintable(fmtStr);
				break;
			case mdk::LogLevel::Error:
				qCCritical(mdklog).noquote() << "mdk:" << qPrintable(fmtStr);
				break;
			case mdk::LogLevel::Off:
			default:
				break;
		}
	}
}

void LogMessageHandler::prepare(const QString &dataPath) {

	// open log file
	if(log_file.size() > 400000) {
		log_file.resize(0); // TODO: Rolling log
		qWarning() << "Log file was resized.";
	}

	QSettings settings;
	settings.beginGroup("general");
	if(settings.contains("logLevel")) {
		QLoggingCategory::setFilterRules(settings.value("logLevel").toString());
	}
	settings.endGroup();

	auto date = QDateTime::currentDateTime();
	auto deleteDate = date.addDays(-7);
	QDir logDir(dataPath);
	log_dir = dataPath;
	auto strRexExp = QString("_(?<date>[\\d\\-_]+)\\.log").prepend(QRegularExpression::escape(QCoreApplication::applicationName()));
	auto logDateRegExp = QRegularExpression(strRexExp);
	auto oldLogFiles = logDir.entryList({QString("%1_*.log").arg(QCoreApplication::applicationName())}, QDir::Files);
	for(const auto &oldLogFile : oldLogFiles) {
		auto match = logDateRegExp.match(oldLogFile);
		if(match.hasMatch()) {
			auto logDate = QDateTime::fromString(match.captured("date"), "yyyy-MM-dd_HH-mm-ss");
			if(logDate.isValid()) {
				if(logDate < deleteDate) {
					if(QFile::remove(logDir.filePath(oldLogFile)))
						qDebug() << "Removed old log file" << oldLogFile;
					else
						qWarning() << "Couldn't remove old log file" << oldLogFile;
				}
			} else {
				qWarning() << "Extracted invalid datetime format log file" << oldLogFile;
			}
		} else {
			qWarning() << "Couldn't extract datetime from log file" << oldLogFile;
		}
	}

	log_file.setFileName(
	 logDir.absoluteFilePath(QString("%2_%3.log").arg(QCoreApplication::applicationName()).arg(date.toString("yyyy-MM-dd_HH-mm-ss"))));
	auto success = log_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
	if(success) {
		qInstallMessageHandler(dbug_msg_handler);
		auto cb = ffmpeg_log_callback;
		av_log_set_level(AV_LOG_TRACE);
		av_log_set_callback(cb);
		mdk::setLogHandler(mdk_log_callback);
		mdk::SetGlobalOption("logLevel", "Debug");
	} else {
		qWarning() << "Couldn't open log file: " << log_file.errorString() << log_file.fileName();
	}
}

void LogMessageHandler::dbug_msg_handler(QtMsgType type, const QMessageLogContext &rContext, const QString &rMessage) {

	QMutexLocker mutex_locker(&mutex);

	auto text = QString("[%1]").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss"));

	switch(type) {
		case QtDebugMsg:
#ifdef QT_NO_DEBUG
			text += QString(" DEBUG    : %1").arg(rMessage);
#else
			text += QString(" DEBUG    %1: %2%3 (%4, %5)")
			         .arg(rContext.category)
			         .arg(QString(MAX_CAT_LENGTH - QString(rContext.category).size(), QChar(' ')))
			         .arg(rMessage)
			         .arg(rContext.file)
			         .arg(rContext.line);
#endif // QT_NO_DEBUG
			break;

		case QtInfoMsg:
#ifdef QT_NO_DEBUG
			text += QString(" INFO    : %1").arg(rMessage);
#else
			text += QString(" INFO     %1: %2%3 (%4, %5)")
			         .arg(rContext.category)
			         .arg(QString(MAX_CAT_LENGTH - QString(rContext.category).size(), QChar(' ')))
			         .arg(rMessage)
			         .arg(rContext.file)
			         .arg(rContext.line);
#endif // QT_NO_DEBUG
			break;

		case QtWarningMsg:
#ifdef QT_NO_DEBUG
			text += QString(" WARNING  : %1").arg(rMessage);
#else
			text += QString(" WARNING  %1: %2%3 (%4, %5)")
			         .arg(rContext.category)
			         .arg(QString(MAX_CAT_LENGTH - QString(rContext.category).size(), QChar(' ')))
			         .arg(rMessage)
			         .arg(rContext.file)
			         .arg(rContext.line);
#endif
			break;

		case QtCriticalMsg:
#ifdef QT_NO_DEBUG
			text += QString(" CRITICAL : %1").arg(rMessage);
#else
			text += QString(" CRITICAL %1: %2%3 (%4, %5)")
			         .arg(rContext.category)
			         .arg(QString(MAX_CAT_LENGTH - QString(rContext.category).size(), QChar(' ')))
			         .arg(rMessage)
			         .arg(rContext.file)
			         .arg(rContext.line);
#endif
			break;

		case QtFatalMsg:
#ifdef QT_NO_DEBUG
			text += QString(" FATAL    : %1").arg(rMessage);
#else
			text += QString(" FATAL    %1: %2%3 (%4, %5)")
			         .arg(rContext.category)
			         .arg(QString(MAX_CAT_LENGTH - QString(rContext.category).size(), QChar(' ')))
			         .arg(rMessage)
			         .arg(rContext.file)
			         .arg(rContext.line);
#endif
			text += "\nApplication will be terminated due to Fatal-Error.";
			break;
	}

	QTextStream tStream(&log_file);
	tStream << text << "\n";
#ifdef Q_OS_ANDROID
	switch(type) {
		case QtDebugMsg:
			__android_log_write(ANDROID_LOG_DEBUG, qPrintable(QCoreApplication::applicationName()), qPrintable(text));
			break;
		case QtInfoMsg:
			__android_log_write(ANDROID_LOG_INFO, qPrintable(QCoreApplication::applicationName()), qPrintable(text));
			break;
		case QtWarningMsg:
			__android_log_write(ANDROID_LOG_WARN, qPrintable(QCoreApplication::applicationName()), qPrintable(text));
			break;
		case QtCriticalMsg:
			__android_log_write(ANDROID_LOG_ERROR, qPrintable(QCoreApplication::applicationName()), qPrintable(text));
			break;
		case QtFatalMsg:
		default:
			__android_log_write(ANDROID_LOG_FATAL, qPrintable(QCoreApplication::applicationName()), qPrintable(text));
	}
#else
	qStdOut() << text << "\n";
	qStdOut().flush();
#endif

#ifdef OutputDebugString
	OutputDebugString(reinterpret_cast<LPCWSTR>(qUtf16Printable(text.append("\n"))));
#endif // OutputDebugString

	if(type == QtFatalMsg) {
		QCoreApplication::instance()->exit(1);
	}
}

void LogMessageHandler::ffmpeg_log_callback(void *arb, int level, const char *fmt, va_list vl) {

	Q_UNUSED(arb);

	auto str = QString::fromLocal8Bit(fmt);
	str.remove('\n');
	if(str.isEmpty() == false) {
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

QString LogMessageHandler::getLogTail(int lines, QDateTime logTime /*= {}*/) {

	const int readSize = 1024;

	QStringList ret;
	ret.push_back({});

	QFile logFile(log_file.fileName());
	if(logTime.isValid()) logFile.setFileName(getLogfilePath(logTime));

	if(logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		auto oldPos = logFile.size() - 1;
		for(auto pos = qMax(oldPos - readSize, (qint64)0); pos >= 0 && ret.size() < lines + 1; pos = qMax(pos - readSize, (qint64)0)) {
			logFile.seek(pos);
			QByteArray data(oldPos - pos + 1, 0);
			auto read = logFile.read(data.data(), oldPos - pos);
			if(read > 0) {
				auto splitString = QString::fromUtf8(data.data(), read).split("\n");
				for(auto i = splitString.size() - 1; i >= 0; i--) {
					auto str = splitString[i];
					if(i == splitString.size() - 1)
						ret.first().prepend(str);
					else if(ret.size() < lines + 1)
						ret.push_front(str);
					else
						break;
				}
			} else {
				break;
			}
			oldPos = pos;
		}
		logFile.close();
	}

	if(!ret.isEmpty()) ret.pop_front();
	return !ret.first().isEmpty() ? ret.join("\n") : QString();
}

QList<QDateTime> LogMessageHandler::getLogHistory() {

	QList<QDateTime> logDates;
	QDir logDir(log_dir);
	auto strRexExp = QString("_(?<date>[\\d\\-_]+)\\.log").prepend(QRegularExpression::escape(QCoreApplication::applicationName()));
	auto logDateRegExp = QRegularExpression(strRexExp);
	auto oldLogFiles = logDir.entryList({QString("%1_*.log").arg(QCoreApplication::applicationName())}, QDir::Files);
	for(const auto &oldLogFile : oldLogFiles) {
		auto match = logDateRegExp.match(oldLogFile);
		if(match.hasMatch()) {
			auto logDate = QDateTime::fromString(match.captured("date"), "yyyy-MM-dd_HH-mm-ss");
			if(logDate.isValid()) {
				logDates.push_back(logDate);
			} else {
				qWarning() << "Extracted invalid datetime format log file" << oldLogFile;
			}
		} else {
			qWarning() << "Couldn't extract datetime from log file" << oldLogFile;
		}
	}

	std::sort(logDates.begin(), logDates.end());
	return logDates;
}

QString LogMessageHandler::getLogfilePath(const QDateTime &logTime) {

	QDir logDir(log_dir);
	return logDir.absoluteFilePath(
	 QString("%2_%3.log").arg(QCoreApplication::applicationName()).arg(logTime.toString("yyyy-MM-dd_HH-mm-ss")));
}
