#pragma once
#include <QObject>
#include <QtQmlIntegration>

class App : public QObject {

	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON
	Q_PROPERTY(
	 bool backgroundServiceEnabled READ isBackgroundServiceEnabled WRITE enableBackgroundService NOTIFY backgroundServiceEnableChanged)
	Q_PROPERTY(bool osSecretsManager READ hasOsSecretsManager NOTIFY hasOsSecretsManagerChanged)

 public:
	App();
	int start(int &argc, char **argv);

	static QString getDefaultUserAgent();
	void enableBackgroundService(bool enable);
	bool isBackgroundServiceEnabled();
	bool hasOsSecretsManager() const;

 signals:
	void backgroundServiceEnableChanged(bool);
	void hasOsSecretsManagerChanged(bool);

 private:
	void initSecretsManager();
	static void initBackgroundService();
	static void startBackgroundService();
	static void stopBackgroundService();
	static void registerMetatypes();
	static void registerQmlTypes();
	static bool mOsSecretsManager;
};
