#pragma once
#include <QVariant>
#include <QtQmlIntegration>

class PropertyInfo {

	Q_GADGET
	QML_ELEMENT
	Q_PROPERTY(Type type READ getType())
	Q_PROPERTY(QString name READ getName())
	Q_PROPERTY(QString description READ getDescription())
	Q_PROPERTY(QVariant defaultValue READ getDefaultValue() WRITE setDefaultValue())

public:
	enum Type { UNKNOWN, POSITIVE_INTEGER, INTEGER, DECIMAL, STRING, BOOL, DATE, DATE_TIME, STRING_LIST, ONVIF_MESSAGE_FILTER };
	Q_ENUM(Type);

	Type getType() const { return mType; }
	void setType(Type type) { mType = type; }
	QString getDescription() const { return mDescription; }
	void setDescription(const QString &rDescription) { mDescription = rDescription; }
	QString getName() const { return mName; }
	void setName(const QString &rName) { mName = rName; }
	QVariant getDefaultValue() const { return mValue; }
	void setDefaultValue(QVariant value) { mValue = value; }

	static Type getTypeForQTypeId(QVariant::Type typeId);

private:
	QString mName;
	QString mDescription;
	Type mType;
	QVariant mValue;
};
