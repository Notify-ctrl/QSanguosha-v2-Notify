import QtQuick 2.15

Item {
    property string name
    property string mouseState: "normal"

    signal clicked
    signal doubleClicked

    id: button

    Image {
        source: "../../../image/button/" + name + "/" + (enabled ? mouseState : "disabled")

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onClicked: button.clicked();
            onDoubleClicked: button.doubleClicked();
            onEntered: mouseState = "hover";
            onExited: mouseState = "normal";
            onPressed: mouseState = "down";
            onReleased: mouseState = containsMouse ? "hover" : "normal";
        }
    }
}
