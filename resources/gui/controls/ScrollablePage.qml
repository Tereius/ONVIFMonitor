import QtQuick 2.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.14 as Kirigami

Kirigami.ScrollablePage {

    padding: 10
    leftPadding: 14
    rightPadding: 14

    property bool isCurrentPage: parent.SwipeView.isCurrentItem
    property bool isNextPage: parent.SwipeView.isNextItem
    property bool isPreviousPage: parent.SwipeView.isPreviousItem

    background: Item {}
}
