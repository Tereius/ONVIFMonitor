#pragma once
#include <QObject>

class App : public QObject {

	Q_OBJECT

 public:
	App();
	int start(int &argc, char **argv, bool headless = false);

	static QString getDefaultUserAgent();

 private:
	static void registerMetatypes();
	static void registerQmlTypes();
};

