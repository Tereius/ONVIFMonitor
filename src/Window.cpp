#include "Window.h"
#include <QGlobalStatic>


Q_GLOBAL_STATIC(Window, globalWindow)

Window::Window(QObject *pParent /*= nullptr*/) :
mModalBusy(false),
mBusy(false),
mError() {

}

bool Window::isModalBusy() const {

	return mModalBusy;
}

void Window::setModalBusy(bool busy) {

	mModalBusy = busy;
	emit modalBusy(busy);
}

bool Window::isBusy() const {

	return mBusy;
}

void Window::setBusy(bool isBusy) {

	mBusy = isBusy;
	emit busy(isBusy);
}

Error Window::getError() const {

	return mError;
}

void Window::showError(const QString &rTitle, const QString &rText) {

	mError = Error(rTitle, rText);
	emit error(mError);
}

Window* Window::getGlobal() {

	return globalWindow;
}
