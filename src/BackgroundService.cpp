#include "BackgroundService.h"
#include "EventManager.h"
#include "QtApplicationBase.h"
#include "SecretsManager.h"
#include "info.h"
#include <QCoreApplication>

#ifdef Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif

BackgroundService::BackgroundService() = default;

int BackgroundService::start(int &argc, char **argv) {

	// qputenv("QT_LOGGING_RULES", "qt.remoteobjects.debug=true");
	Q_ASSERT(!QCoreApplication::instance());

	qRegisterMetaType<QList<QUuid>>();

#ifdef Q_OS_ANDROID
	QtApplicationBase<QAndroidService> service(argc, argv, QString("%1Service").arg(INFO_PROJECTNAME));
#else
	QtApplicationBase<QCoreApplication> service(argc, argv, QString("%1").arg(INFO_PROJECTNAME));
#endif

	// always make sure that the fallbackSettings and namespace is equal to the one in the App. Service and App have to access the same secret
	// store
	SecretsManager::setNamespace(QString("%1.%2").arg(QCoreApplication::organizationDomain(), INFO_PROJECTNAME));
	SecretsManager::setFallbackSettings(
	 std::make_unique<QSettings>(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), "Secrets"));

	EventManager::getInstance()->initialize();
	return service.start();
}
