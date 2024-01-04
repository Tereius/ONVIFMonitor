#include "Serializable.h"
#include <QMetaProperty>
#include <QSettings>


void SerializeSettings::serializeSettings(QSettings *settings) {

	if(settings) {
		if(auto meta = metaObject()) {
			for(auto i = 0; i < meta->propertyCount(); i++) {
				auto property = meta->property(i);
				if(property.isReadable() && property.isStored() && QString(property.name()).compare("objectName") != 0) {
					settings->setValue(QString("%1_%2").arg(meta->className(), property.name()), property.read(this));
				}
			}
		}
	}
}

void SerializeSettings::deserializeSettings(QSettings *settings) {

	if(settings) {
		if(auto meta = metaObject()) {
			for(auto i = 0; i < meta->propertyCount(); i++) {
				auto property = meta->property(i);
				if(property.isWritable() && property.isStored() && settings->contains(QString("%1_%2").arg(meta->className(), property.name())) &&
				   QString(property.name()).compare("objectName") != 0) {
					property.write(this, settings->value(QString("%1_%2").arg(meta->className(), property.name())));
				}
			}
		}
	}
}
