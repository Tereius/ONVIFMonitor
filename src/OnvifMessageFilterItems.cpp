#include "OnvifMessageFilterItems.h"

void NotItem::dragEnterEvent(QDragEnterEvent *) {
	throw std::logic_error("The method or operation is not implemented.");
}

void NotItem::dragMoveEvent(QDragMoveEvent *) {
	throw std::logic_error("The method or operation is not implemented.");
}

void NotItem::dragLeaveEvent(QDragLeaveEvent *) {
	throw std::logic_error("The method or operation is not implemented.");
}

void ValItem::dragEnterEvent(QDragEnterEvent *) {}

void ValItem::dragMoveEvent(QDragMoveEvent *) {}

void ValItem::dragLeaveEvent(QDragLeaveEvent *) {}
