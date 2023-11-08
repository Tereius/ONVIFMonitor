#include "AdvancedQmlApplicationEngine.h"
#include "QtApplicationBase.h"
#include "App.h"
#include "EventHandlerModel.h"
#include "OnvifDiscovery.h"
#include "OnvifMessageFilterItems.h"
#include <QCommandLineParser>
#include <QFile>
#include <QFontDatabase>
#include <QIcon>
#include <QTextStream>
#include <QtGlobal>
#include <QtPlugin>

#ifdef QT_OS_WINDOWS
#include "qt_windows.h"
#endif


int main(int argc, char *argv[]) {

	qunsetenv("QT_STYLE_OVERRIDE");
	qunsetenv("QT_QUICK_CONTROLS_STYLE");

	App app;
	return app.start(argc, argv);
}
