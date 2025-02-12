#pragma once
#include <QObject>
#include <QtQmlIntegration>

namespace Enums {

Q_NAMESPACE

enum Roles {

	EndpointRole = Qt::UserRole + 1,
	HostRole,
	PortRole,
	NameRole,
	SettingsRole,
	ManufacturerRole,
	OnvifProfilesRole,
	DescriptionRole,
	ProfileRole,
	HasSnapshotRole,
	HasBackchannelRole,
	BackchannelSdp,
	BackchannelUrl,
	IdRole,
	PathRole,
	InitializedRole,
	IsNewRole,
	ErrorRole,
	ValueRole,
	PropertiesRole,
	SimpleItemsRole,
	StatusRole,
	HasSourceRole,
	HasHandlerRole,
	EventSourceNameRole,
	EventHandlerNameRole,
	EventSourcePropertiesRole,
	EventHandlerPropertiesRole
};

enum FillMode { Stretch = 0, PreserveAspectFit, PreserveAspectCrop };

Q_ENUM_NS(Roles)
Q_ENUM_NS(FillMode)
} // namespace Enums

class Roles : public QObject {

	Q_OBJECT
	QML_ELEMENT
	QML_EXTENDED_NAMESPACE(Enums)
};

class FillMode : public QObject {

	Q_OBJECT
	QML_ELEMENT
	QML_EXTENDED_NAMESPACE(Enums)
};
