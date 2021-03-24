import QtQuick 2.4

MouseArea {
    id: mouseArea
    acceptedButtons: Qt.NoButton
    anchors.fill: parent
    hoverEnabled: true

    property string text: ""
    property alias hideDelay: hideTimer.interval
    property alias showDelay: showTimer.interval

    Timer {
        id: showTimer
        interval: 1000
        running: text.length !== 0 && mouseArea.containsMouse && !tip.visible
        onTriggered: {
            let p = mapToItem(rootWindowItem, mouseX, mouseY)
            p.x += rootWindow.x + 5
            p.y += rootWindow.y + 5
            tip.opacity = 1
            tip.appear(p, text);
        }
    }

    Timer {
        id: hideTimer
        interval: 100
        running: !mouseArea.containsMouse && tip.visible
        onTriggered: {
            tip.opacity = 0
            tip.hide();
        }
    }
}
