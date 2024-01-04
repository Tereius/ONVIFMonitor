#pragma once
#include <QObject>
#include <QtQmlIntegration>

class App : public QObject {

	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

 public:
	App();
	int start(int &argc, char **argv);

	static QString getDefaultUserAgent();
	Q_INVOKABLE static void startBackgroundService();
	Q_INVOKABLE static void stopBackgroundService();

 private:
	static void registerMetatypes();
	static void registerQmlTypes();
};
