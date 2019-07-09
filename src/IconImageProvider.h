#pragma once
#include <QQuickImageProvider>
#include <QObject>


class IconImageProvider : public QObject, public QQuickImageProvider {

	Q_OBJECT

public:

	IconImageProvider(QObject *pParent = nullptr);
	virtual ~IconImageProvider();
	virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

private:

	Q_DISABLE_COPY(IconImageProvider);
};