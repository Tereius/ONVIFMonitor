#include <Qt>

namespace Enums {

Q_NAMESPACE

enum Roles {

	EndpointRole = Qt::UserRole + 1,
	HostRole,
	PortRole,
	NameRole,
	ManufacturerRole,
	ServicesRole,
	DescriptionRole,
	ProfileRole,
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
