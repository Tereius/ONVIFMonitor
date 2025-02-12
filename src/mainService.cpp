#include "BackgroundService.h"
#include "LogMessageHandler.h"
#include <QStandardPaths>


int main(int argc, char *argv[]) {

	LogMessageHandler::prepare(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
	BackgroundService service;
	return service.start(argc, argv);
}
