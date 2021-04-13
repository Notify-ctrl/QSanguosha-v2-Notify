import QtQuick 2.15

MouseArea {
    id: mouseArea
    acceptedButtons: Qt.NoButton
    anchors.fill: parent
    hoverEnabled: true
    preventStealing: true

    property string text: ""
    property alias hideDelay: hideTimer.interval
    property alias showDelay: showTimer.interval

    Timer {
        id: showTimer
        interval: 0
        //running: text.length !== 0 && mouseArea.containsMouse && !tip.visible
        onTriggered: {
            let p = mapToItem(rootWindowItem, mouseX, mouseY)
            p.x += rootWindow.x + 20
            p.y += rootWindow.y - 20
            tip.opacity = 1
            tip.appear(p, text);
        }
    }

    Timer {
        id: hideTimer
        interval: 0
        // running: !mouseArea.containsMouse && tip.visible
        onTriggered: {
            tip.opacity = 0
            tip.hide();
        }
    }

    onEntered: {
        showTimer.start()
    }

    onExited: {
        hideTimer.start()
    }

    onPositionChanged: {
        tip.x = mapToItem(rootWindowItem, mouseX, mouseY).x + rootWindow.x + 20
        tip.y = mapToItem(rootWindowItem, mouseX, mouseY).y + rootWindow.y - 20
    }
}
