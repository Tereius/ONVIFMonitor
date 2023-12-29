import QtQuick
import QtQuick.Controls

Pane {

    property bool isCurrentPage: parent.SwipeView ? parent.SwipeView.isCurrentItem : false
    property bool isNextPage: parent.SwipeView ? parent.SwipeView.isNextItem : false
    property bool isPreviousPage: parent.SwipeView ? parent.SwipeView.isPreviousItem : false

    function setCurrentIndex(index) {
        if (parent.SwipeView) {
            parent.SwipeView.view.setCurrentIndex(index)
        }
    }

    function incrementCurrentIndex() {
        if (parent.SwipeView) {
            parent.SwipeView.view.incrementCurrentIndex()
        }
    }

    function decrementCurrentIndex() {
        if (parent.SwipeView) {
            parent.SwipeView.view.decrementCurrentIndex()
        }
    }
}
