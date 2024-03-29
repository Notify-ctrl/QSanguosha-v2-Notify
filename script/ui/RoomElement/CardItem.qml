import QtQuick 2.15
import QtGraphicalEffects 1.0
import "../Util"
import "../Util/util.js" as Utility
import "../Util/skin-bank.js" as SkinBank

Item {
    property int cid: 0
    property string suit: ""
    property int number: 0
    property string name: "slash"
    readonly property string color: (suit == "heart" || suit == "diamond") ? "red" : "black"
    property string subtype: ""
    property int homeX: 0
    property int homeY: 0
    property real homeOpacity: 1.0
    property int goBackDuration: 500
    property bool selectable: true
    property bool selected: false
    property bool draggable: false
    property bool autoBack: true
    property alias glow: glowItem
    property alias footnote: footnoteItem.text
    property bool footnoteVisible: true
    property alias card: cardItem
    property alias goBackAnim: goBackAnimation
    property bool isClicked: false
    property bool moveAborted: false
    property bool isKnown: true
    property alias description: tipArea.text

    signal toggleDiscards()
    signal clicked()
    signal doubleClicked()
    signal thrown()
    signal released()
    signal entered()
    signal exited()
    signal moveFinished()
    signal generalChanged()
    signal hoverChanged(bool enter)

    id: root
    width: 93
    height: 130

    RectangularGlow {
        id: glowItem
        anchors.fill: parent
        glowRadius: 8
        spread: 0
        color: "#88FFFFFF"
        cornerRadius: 8
        visible: false
    }

    Image {
        id: cardItem
        source: isKnown ? (name != "" ? SkinBank.CARD_DIR + name : "") : SkinBank.CARD_BACK
        anchors.fill: parent
    }

    Image {
        id: suitItem
        visible: isKnown
        source: suit != "" ? SkinBank.CARD_SUIT_DIR + suit : ""
        x: 3
        y: 19
        width: 21
        height: 17
    }

    Image {
        id: numberItem
        visible: isKnown
        source: (color != "" && number > 0) ? SkinBank.CARD_NUMBER_DIR + color + "/" + number : ""
        x: 0
        y: 2
        width: 27
        height: 28
    }

    GlowText {
        id: footnoteItem
        x: 6
        y: parent.height - height - 6
        width: root.width - x * 2
        color: "white"
        visible: footnoteVisible
        wrapMode: Text.WrapAnywhere
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 12
        glow.color: "black"
        glow.spread: 1
        glow.radius: 1
        glow.samples: 12
    }

    Rectangle {
        visible: !root.selectable
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.5)
        opacity: 0.7
    }

    MouseArea {
        anchors.fill: parent
        drag.target: draggable ? parent : undefined
        drag.axis: Drag.XAndYAxis
        hoverEnabled: true

        onReleased: {
            root.isClicked = mouse.isClick;
            parent.released();
            if (autoBack)
                goBackAnimation.start();
        }

        onEntered: {
            parent.entered();
            if (draggable) {
                glow.visible = true;
                root.z++;
            }
        }

        onExited: {
            parent.exited();
            if (draggable) {
                glow.visible = false;
                root.z--;
            }
        }

        onClicked: {
            selected = selectable ? !selected : false;
            parent.clicked();
        }

        ToolTipArea {
            id: tipArea
            enabled: name !== "card-back"
            text: Sanguosha.translate(name) + "[<img src='../../../image/system/log/"
                  + suit + ".png' height = 14/> " + Utility.convertNumber(number) + "] " + Sanguosha.translate(":" + name)
        }
    }

    ParallelAnimation {
        id: goBackAnimation

        PropertyAnimation {
            target: root
            property: "x"
            to: homeX
            easing.type: Easing.OutQuad
            duration: goBackDuration
        }

        PropertyAnimation {
            target: root
            property: "y"
            to: homeY
            easing.type: Easing.OutQuad
            duration: goBackDuration
        }

        SequentialAnimation {
            PropertyAnimation {
                target: root
                property: "opacity"
                to: 1
                easing.type: Easing.OutQuad
                duration: goBackDuration * 0.8
            }

            PropertyAnimation {
                target: root
                property: "opacity"
                to: homeOpacity
                easing.type: Easing.OutQuad
                duration: goBackDuration * 0.2
            }
        }

        onStopped: {
            if (!moveAborted)
                root.moveFinished();
        }
    }

    function setData(data)
    {
        cid = data.cid;
        name = data.name;
        suit = data.suit;
        number = data.number;
        subtype = data.subtype;
    }

    function toData()
    {
        var data = {
            cid: cid,
            name: name,
            suit: suit,
            number: number,
            subtype: subtype
        };
        return data;
    }

    function goBack(animated)
    {
        if (animated) {
            moveAborted = true;
            goBackAnimation.stop();
            moveAborted = false;
            goBackAnimation.start();
        } else {
            x = homeX;
            y = homeY;
            opacity = homeOpacity;
        }
    }

    function destroyOnStop()
    {
        root.moveFinished.connect(function(){
            destroy();
        });
    }
}

