#pragma once
#include <QObject>

class QSettings;

class SerializeSettings : public QObject {

 public:
	explicit SerializeSettings(QObject *parent = nullptr) : QObject(parent) {}

	// Q_PROPERTY must enable: STORED true
	virtual void serializeSettings(QSettings *settings);
	// Q_PROPERTY must enable: STORED true
	virtual void deserializeSettings(QSettings *settings);
};
