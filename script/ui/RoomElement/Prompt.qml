import QtQuick 2.15


Image {
    property alias text: content.text

    width: 480
    height: 200
    scale: visible ? 1 : 0
    source: "../../../image/system/tip"

    signal finished()

    Text {
        id: content
        color: "white"
        x: 30
        y: 35
        width: parent.width - x * 2
        font.family: "LiSu"
        font.pixelSize: 22
        wrapMode: Text.WordWrap
    }

    MouseArea {
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XAndYAxis
    }

    Behavior on scale {
        NumberAnimation {
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }
}
