#include "IconImageProvider.h"
#include <QIcon>
#include <QDebug>
#include <QColor>


IconImageProvider::IconImageProvider(QObject *pParent /*= nullptr*/) :
QObject(pParent), 
QQuickImageProvider(QQmlImageProviderBase::Pixmap) {

}

IconImageProvider::~IconImageProvider() {

}

QPixmap IconImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {

	auto icon = QIcon::fromTheme(id);
	QImage tmp = icon.pixmap(requestedSize).toImage();
	QColor color = QColor(255, 255, 255);

	// Loop all the pixels
	for(int y = 0; y < tmp.height(); y++) {
		for(int x = 0; x < tmp.width(); x++) {
			// Read the alpha value each pixel, keeping the RGB values of your color
			color.setAlpha(tmp.pixelColor(x, y).alpha());

			// Apply the pixel color
			tmp.setPixelColor(x, y, color);
		}
	}

	// Get the coloured pixmap
	return QPixmap::fromImage(tmp);
	//if(icon.isNull()) {
	//	qDebug() << "ads";
	////}
	//return icon.pixmap(requestedSize);
}
