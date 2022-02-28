import QtQuick 2.12
import TilesGrid 1.0

TilesGrid {

    id: layout

    readonly property real effectiveAtomicWidth: layout.contentItem.atomicWidth
    readonly property real effectiveAtomicHeight: layout.contentItem.atomicHeight

    onWidthChanged: {

        if (layout.contentItem)
            layout.contentItem.atomicWidth
                    = (layout.width - layout.leftPadding - layout.rightPadding
                       - (layout.columns - 1) * layout.columnSpacing) / layout.columns
    }

    onHeightChanged: {
        if (layout.contentItem)
            layout.contentItem.atomicHeight
                    = (layout.height - layout.topPadding - layout.bottomPadding
                       - (layout.rows - 1) * layout.rowSpacing) / layout.rows
    }
}
