#pragma once
#include <QObject>

class BackgroundService : public QObject {

	Q_OBJECT

 public:
	BackgroundService();
	int start(int &argc, char **argv);
};
