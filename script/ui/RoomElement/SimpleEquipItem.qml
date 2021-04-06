import QtQuick 2.15
import "../Util"
import "../Util/util.js" as Utility

Rectangle {
    property int cid: 0
    property string name: ""
    property string suit: ""
    property int number: 0

    property string icon: ""
    property alias text: textItem.text

    id: root
    border.color: "#88767676"
    color: Qt.rgba(255, 255, 255, 0.5)

    Image {
        id: iconItem
        anchors.verticalCenter: parent.verticalCenter
        x: 1

        source: "../../../image/card/equip/icon/background"
        Image {
            source: icon ? "../../../image/card/equip/icon/" + icon : ""
            anchors.centerIn: parent
        }
    }

    GlowText {
        id: textItem
        font.family: "LiSu"
        font.pixelSize: 14
        glow.color: "#FFFFBE"
        glow.spread: 0.9
        glow.radius: 3
        glow.samples: 6
        anchors.left: iconItem.right
        anchors.leftMargin: 3
        anchors.right: numberItem.left
        anchors.rightMargin: 4
        horizontalAlignment: Text.AlignHCenter
    }

    GlowText {
        id: numberItem
        visible: number > 0 && number < 14
        text: Utility.convertNumber(number)
        font.weight: Font.Bold
        font.pixelSize: 10
        glow.color: "#FFFFBE"
        glow.spread: 0.75
        glow.radius: 2
        glow.samples: 4
        x: parent.width - 24
        y: 1
    }

    Image {
        id: suitItem
        anchors.right: parent.right
        source: suit ? "../../../image/card/suit/" + suit : ""
        width: implicitWidth / implicitHeight * height
        height: parent.height
    }

    ParallelAnimation {
        id: showAnime

        NumberAnimation {
            target: root
            property: "x"
            duration: 200
            easing.type: Easing.InOutQuad
            from: 10
            to: 0
        }


        NumberAnimation {
            target: root
            property: "opacity"
            duration: 200
            easing.type: Easing.InOutQuad
            from: 0
            to: 1
        }
    }

    ParallelAnimation {
        id: hideAnime

        NumberAnimation {
            target: root
            property: "x"
            duration: 200
            easing.type: Easing.InOutQuad
            from: 0
            to: 10
        }


        NumberAnimation {
            target: root
            property: "opacity"
            duration: 200
            easing.type: Easing.InOutQuad
            from: 1
            to: 0
        }
    }

    ToolTipArea {
        enabled: name !== ""
        text: Sanguosha.translate(name) + "[<img src='../../../image/system/log/"
              + suit + ".png' height = 14/> " + Utility.convertNumber(number) + "] " + Sanguosha.translate(":" + name)
    }

    function reset()
    {
        cid = 0;
        name = "";
        suit = "";
        number = 0;
        text = "";
    }

    function setCard(card)
    {
        cid = card.cid;
        name = card.name;
        suit = card.suit;
        number = card.number;
        if (card.subtype === "defensive_horse") text = "+1"
        else if (card.subtype === "offensive_horse") text = "-1"
        else text = Sanguosha.translate(card.name);
    }

    function show()
    {
        showAnime.start();
    }

    function hide()
    {
        hideAnime.start();
    }
}
