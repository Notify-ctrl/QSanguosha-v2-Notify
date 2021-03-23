import QtQuick 2.4


Image {
    property alias text: content.text

    width: 480
    height: 200
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
}
