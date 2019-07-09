#include "Error.h"
#include <QObject>


Error::Error():
mErrorTitle(QObject::tr("Unknown")),
mERrorText(QObject::tr("Unknown")) {

}

Error::Error(const QString &rErrorTitle, const QString &rErrorText):
mErrorTitle(rErrorTitle),
mERrorText(rErrorText) {

}

QString Error::getErrorTitle() const {
	
	return mErrorTitle;
}

QString Error::getErrorText() const {

	return mERrorText;
}
