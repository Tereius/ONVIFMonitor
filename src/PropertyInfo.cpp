#include "PropertyInfo.h"


PropertyInfo::Type PropertyInfo::getTypeForQTypeId(QVariant::Type typeId) {

	Type ret = Type::UNKNOWN;
	switch(typeId) {
		case QVariant::Bool:
			ret = Type::BOOL;
			break;
		case QVariant::Date:
			ret = Type::DATE;
			break;
		case QVariant::DateTime:
			ret = Type::DATE_TIME;
			break;
		case QVariant::Double:
			ret = Type::DECIMAL;
			break;
		case QVariant::Int:
		case QVariant::LongLong:
			ret = Type::INTEGER;
			break;
		case QVariant::String:
			ret = Type::STRING;
			break;
		case QVariant::UInt:
		case QVariant::ULongLong:
			ret = Type::POSITIVE_INTEGER;
			break;
		case QVariant::StringList:
			ret = Type::STRING_LIST;
			break;
		case QVariant::UserType:
			ret = Type::ONVIF_MESSAGE_FILTER;
			break;
		default:
			ret = Type::UNKNOWN;
			break;
	}
	return ret;
}
