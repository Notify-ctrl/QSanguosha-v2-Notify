import QtQuick 2.8
import QtQuick.Controls 2.2 // 辣鸡qml自带tooltip
import QtQuick.Window 2.1

ApplicationWindow {
    id: root

    flags: Qt.CustomizeWindowHint | Qt.FramelessWindowHint | Qt.ToolTip
    color: "transparent"
    opacity: 0

    // related to contentWidth
    property string text: "QSanguoshaQSanguoshaQSanguoshaQSanguosha"

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
            textFormat: Text.RichText
            anchors.centerIn: parent

            style: Text.Outline
            font.pixelSize: 18
            color: "#E4D5A0"
            text: root.text
        }
    }

    Behavior on opacity {
        enabled: true
        NumberAnimation {
            duration: 200
            // easing.type: Easing.InOutQuad
        }
    }

    function appear(point, text) {
        root.text = text

        // boundary detection
        var xx = point.x
        var yy = point.y
        if (xx + width > Screen.desktopAvailableWidth) {
            xx = Screen.desktopAvailableWidth - width
        }
        if (yy + height > Screen.desktopAvailableHeight) {
            yy = Screen.desktopAvailableHeight - height
        }

        root.x = xx
        root.y = yy

        show()
    }
}
