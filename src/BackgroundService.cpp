#include "BackgroundService.h"
#include "QtApplicationBase.h"
#include <QCoreApplication>
#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif

BackgroundService::BackgroundService() = default;

int BackgroundService::start(int &argc, char **argv) {

	Q_ASSERT(!QCoreApplication::instance());

#ifdef Q_OS_ANDROID
	QtApplicationBase<QAndroidService> service(argc, argv);
#else
	QtApplicationBase<QCoreApplication> service(argc, argv);
#endif
	return service.exec();
}
