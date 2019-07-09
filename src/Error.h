#pragma once
#include <QMetaObject>
#include <QString>


class Error {

	Q_GADGET

public:
	Error();
	Error(const QString &rErrorTitle, const QString &rErrorText);
	Q_INVOKABLE QString getErrorTitle() const;
	Q_INVOKABLE QString getErrorText() const;

private:
	QString mErrorTitle;
	QString mERrorText;
};