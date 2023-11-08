#pragma once
#include <QQuickImageProvider>
#include <QObject>


class IconImageProvider : public QQuickImageProvider {

public:

	IconImageProvider();
	virtual ~IconImageProvider();
	virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

private:

	Q_DISABLE_COPY(IconImageProvider);
};