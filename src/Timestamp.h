#pragma once
#include <QDateTime>


class Timestamp {

public:
	

	QString toString() const;

	QDateTime getEntity() const { return mEntity; }
	void setEntity(const QDateTime &rVal) { mEntity = rVal; }

private:
	QDateTime mEntity;
};