#pragma once
#include "NotificationFilter.h"
#include <QQuickItem>

class NotItem : public QQuickItem {

public:
	NotItem(QQuickItem *pParent = nullptr) : QQuickItem(pParent) { setFlag(QQuickItem::ItemAcceptsDrops, true); }

protected:
	virtual void dragEnterEvent(QDragEnterEvent *) override;
	virtual void dragMoveEvent(QDragMoveEvent *) override;
	virtual void dragLeaveEvent(QDragLeaveEvent *) override;
};

class ValItem : public QQuickItem {

public:
	ValItem(QQuickItem *pParent = nullptr) : QQuickItem(pParent) { setFlag(QQuickItem::ItemAcceptsDrops, true); }

protected:
	virtual void dragEnterEvent(QDragEnterEvent *) override;
	virtual void dragMoveEvent(QDragMoveEvent *) override;
	virtual void dragLeaveEvent(QDragLeaveEvent *) override;
};
