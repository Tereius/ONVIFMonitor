#include "BackgroundService.h"
#include "EventManager.h"
#include "QtApplicationBase.h"
#include <QCoreApplication>
#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif

BackgroundService::BackgroundService() = default;

int BackgroundService::start(int &argc, char **argv) {

	qputenv("QT_LOGGING_RULES", "qt.remoteobjects.debug=true");
	Q_ASSERT(!QCoreApplication::instance());

	qRegisterMetaType<QList<QUuid>>();

#ifdef Q_OS_ANDROID
	QtApplicationBase<QAndroidService> service(argc, argv);
#else
	QtApplicationBase<QCoreApplication> service(argc, argv);
#endif

	EventManager::getInstance()->initialize();
	return service.exec();
}
