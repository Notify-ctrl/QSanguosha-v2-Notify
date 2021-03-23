import QtQuick 2.4


Image {
    property string kingdom
    property int value

    source: "../../../image/kingdom/handcard/" + (kingdom != "" && kingdom != "hidden" ? kingdom : "qun")

    GlowText {
        id: handcardNumText
        anchors.fill: parent
        color: "white"
        text: value
        font.pixelSize: 12
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        glow.spread: 0.7
        glow.radius: 4.0
        glow.samples: 4
        glow.color: "#000"
    }
}
