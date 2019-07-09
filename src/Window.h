#pragma once
#include "QObject"
#include "Error.h"
#include <QAtomicInteger>


class Window : public QObject {

	Q_OBJECT
		Q_PROPERTY(bool busy READ isBusy NOTIFY busy)
		Q_PROPERTY(bool modalBusy READ isModalBusy NOTIFY modalBusy)
		Q_PROPERTY(Error error READ getError NOTIFY error)

public:
	Window(QObject *pParent = nullptr);
	bool isModalBusy() const;
	void setModalBusy(bool busy);
	bool isBusy() const;
	void setBusy(bool busy);
	Error getError() const;
	void showError(const QString &rTitle, const QString &rText);

	static Window* getGlobal();

signals:
	void modalBusy(bool busy);
	void busy(bool busy);
	void error(const Error &rError);

private:

	Q_DISABLE_COPY(Window);
	QAtomicInteger<qint32> mModalBusy;
	QAtomicInteger<qint32> mBusy;
	Error mError;
};