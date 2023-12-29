#include <QObject>
#include <QtQmlIntegration>

namespace Enums {

Q_NAMESPACE

enum Roles {

	EndpointRole = Qt::UserRole + 1,
	HostRole,
	PortRole,
	NameRole,
	ManufacturerRole,
	OnvifProfilesRole,
	DescriptionRole,
	ProfileRole,
	HasSnapshotRole,
	HasBackchannelRole,
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

Q_ENUM_NS(Roles)
} // namespace Enums

class Roles : public QObject {

	Q_OBJECT
	QML_ELEMENT
	QML_EXTENDED_NAMESPACE(Enums)
};
