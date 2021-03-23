import QtQuick 2.4


Image {
    property string name
    property string type: "proactive"
    property int columns: 1
    property bool pressed: false

    readonly property string status: pressed ? "down" : mouseArea.containsMouse ? "hover" : "normal"

    width: columns === 1 ? 120 : 59
    height: 26
    source: "../../../image/button/skill/" + type + "/" + columns + "-" + (enabled ? status : "disabled")
    clip: true

    onEnabledChanged: {
        if (!enabled)
            pressed = false;
    }

    GlowText {
        text: qsTr(name)
        color: "white"
        font.family: "楷体"
        font.pixelSize: 14
        font.weight: Font.Black
        font.letterSpacing: columns === 1 ? 20 : 0
        horizontalAlignment: Text.AlignHCenter
        x: columns === 1 ? 26 : 20
        y: 6
        width: parent.width - 25
        style: Text.Outline
        styleColor: Qt.rgba(255, 255, 255, 0.1)
        glow.color: Qt.rgba(0, 0, 0, 0.65)
        glow.radius: 1
        glow.spread: 1
        glow.samples: 2
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: parent.pressed = !parent.pressed;
    }
}
