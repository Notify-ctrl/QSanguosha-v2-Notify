import QtQuick 2.4
import QtGraphicalEffects 1.0


Item {
    property alias title: titleItem
    signal accepted() //Read result
    signal finished() //Close the box

    id: root

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#B0000000"
        radius: 5
        border.color: "#A6967A"
        border.width: 1
    }

    DropShadow {
        source: background
        anchors.fill: background
        color: "#B0000000"
        radius: 5
        samples: 12
        spread: 0.2
        horizontalOffset: 5
        verticalOffset: 4
        transparentBorder: true
    }

    Text {
        id: titleItem
        color: "#E4D5A0"
        width: parent.width
        height: 32
        font.pixelSize: 18
        horizontalAlignment: Text.AlignHCenter
        x: 6
        y: 6

        transform: Scale {
            xScale: Math.min(1.0, (root.width - 12)) / titleItem.width
        }
    }

    MouseArea {
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XAndYAxis
    }

    function close()
    {
        accepted();
        finished();
    }
}
