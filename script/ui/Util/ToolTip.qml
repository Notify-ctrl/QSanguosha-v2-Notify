import QtQuick 2.8
import QtQuick.Controls 2.2 // 辣鸡qml自带tooltip
import QtQuick.Window 2.1

Window {
    id: root

    flags: Qt.CustomizeWindowHint | Qt.FramelessWindowHint | Qt.ToolTip
    color: "transparent"
    opacity: 0

    // related to contentWidth
    property string text: "话说我们这是停工几个月了优先级不改改吗"

    width: main_rect.width
    height: main_rect.height

    Rectangle {
        id: main_rect
        radius: 8
        width: tipText.contentWidth + 16
        height: tipText.contentHeight + 16
        opacity: 0.8
        border.color: "#676554"
        border.width: 1
        color: "#2E2C27"
        Text {
            id: tipText
            wrapMode: Text.WordWrap
            width: 480
            textFormat: Text.RichText
            anchors.centerIn: parent

            style: Text.Outline
            font.pixelSize: 18
            color: "#E4D5A0"
            text: root.text
        }
    }

    Behavior on opacity {
        NumberAnimation {
            duration: 200
        }
    }

    function appear(point, text) {
        root.text = text

        root.x = point.x
        root.y = point.y

        show()
    }
}
