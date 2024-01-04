#pragma once
#include <QObject>

class QSettings;

class SerializeSettings : public QObject {

 public:
	// Q_PROPERTY must enable: STORED true
	virtual void serializeSettings(QSettings *settings);
	// Q_PROPERTY must enable: STORED true
	virtual void deserializeSettings(QSettings *settings);
};
