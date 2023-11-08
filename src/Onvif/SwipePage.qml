import QtQuick
import QtQuick.Controls

Pane {

    property bool isCurrentPage: parent.SwipeView ? parent.SwipeView.isCurrentItem : false
    property bool isNextPage: parent.SwipeView ? parent.SwipeView.isNextItem : false
    property bool isPreviousPage: parent.SwipeView ? parent.SwipeView.isPreviousItem : false
}
