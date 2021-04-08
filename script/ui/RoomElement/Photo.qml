import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Sanguosha 1.0
import "../Util"

Item {
    property string headGeneral: ""
    property string deputyGeneral: ""
    property ClientPlayer clientPlayer: null
    property alias screenName: screenNameItem.text
    property alias faceTurned: faceTurnedCover.visible
    property string userRole: "unknown"
    property string kingdom: "unknown"
    property alias handcardNum: handcardNumItem.value
    property alias maxHp: hpBar.maxValue
    property alias hp: hpBar.value
    property alias handcardArea: handcardAreaItem
    property alias equipArea: equipAreaItem
    property alias delayedTrickArea: delayedTrickAreaItem
    property alias specialArea: specialArea
    property string phase: "inactive"
    property bool chained: false
    property bool dying: false
    property bool alive: true
    property bool drunk: clientPlayer === null ? false : clientPlayer.getMark("drank") > 0
    property bool faceup: true
    property alias progressBar: progressBarItem
    property int seat: 0
    property bool selectable: false
    property bool selected: false

    id: root
    width: 157
    height: 182
    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: outerGlow
                visible: false
            }
        },
        State {
            name: "candidate"
            PropertyChanges {
                target: outerGlow
                color: "#EEB300"
                visible: root.selectable && root.selected
            }
        },
        State {
            name: "playing"
            PropertyChanges {
                target: outerGlow
                color: "#BE85EE"
                visible: true
            }
        },
        State {
            name: "responding"
            PropertyChanges {
                target: outerGlow
                color: "#51D659"
                visible: true
            }
        },
        State {
            name: "sos"
            PropertyChanges {
                target: outerGlow
                color: "#ED8B96"
                visible: true
            }
        }
    ]
    state: "normal"

    RectangularGlow {
        id: outerGlow
        anchors.fill: parent
        visible: true
        glowRadius: 8
        spread: 0.4
        cornerRadius: 8
    }

    Item {
        width: deputyGeneral != "" ? 75 : 155
        height: 182

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: "../../../image/general/full/" + (headGeneral != "" ? headGeneral : "anjiang")
        }
    }

    Item {
        x: 80
        width: 75
        height: 182

        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: deputyGeneral != "" ? "../../../image/general/full/" + deputyGeneral : ""
        }
    }

    ToolTipArea {
        enabled: clientPlayer !== null
        text: clientPlayer === null ? "" : clientPlayer.getSkillDescription()
    }

    Rectangle {
        color: Qt.rgba(250, 0, 0, 0.45)
        anchors.fill: parent
        visible: parent.drunk
    }

    Image {
        source: "../../../image/general/circle-photo"
        visible: deputyGeneral != ""
    }

    Image {
        id: faceTurnedCover
        anchors.fill: parent
        source: "../../../image/general/faceturned"
        visible: !faceup
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: "black"
        border.width: 2

        Rectangle {
            color: Qt.rgba(0, 0, 0, 0.5)
            width: parent.width
            height: 20
        }
    }

    SimpleEquipArea {
        id: equipAreaItem

        width: parent.width - 20
        height: 60
        y: parent.height - height
    }

    HandcardNumber {
        id: handcardNumItem
        x: -10
        y: 102
        kingdom: parent.kingdom
        value: handcardArea.length
    }

    Item {
        width: 17
        height: maxHp > 5 ? 72 : 6 + 18 * maxHp
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 1
        anchors.rightMargin: 2
        clip: true

        Image {
            source: "../../../image/general/magatamas/bg"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            visible: hpBar.visible
        }

        HpBar {
            id: hpBar
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 3
            visible: maxHp > 0

            transform: Scale {
                origin.x: hpBar.width / 2
                origin.y: hpBar.height
                xScale: 15 / hpBar.width
                yScale: xScale
            }
        }
    }

    Text {
        id: screenNameItem
        color: "white"
        font.pixelSize: 12
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 35
        horizontalAlignment: Text.AlignHCenter
        y: 3
    }

    GlowText {
        id: headGeneralNameItem
        color: "white"
        x: 2
        y: 30
        font.pixelSize: 18
        font.family: "LiSu"
        font.weight: Font.Bold
        width: 24
        wrapMode: Text.WrapAnywhere
        lineHeight: text.length === 2 ? 1.5 : 1
        horizontalAlignment: Text.AlignHCenter
        text: Sanguosha.translate(headGeneral)

        glow.color: "black"
        glow.spread: 0.7
        glow.radius: 6
        glow.samples: 24
    }

    GlowText {
        id: deputyGeneralNameItem
        color: "white"
        x: 80
        y: 30
        font.pixelSize: 18
        font.family: "LiSu"
        font.weight: Font.Bold
        width: 32
        wrapMode: Text.WrapAnywhere
        lineHeight: 1.5
        horizontalAlignment: Text.AlignHCenter
        text: Sanguosha.translate(deputyGeneral)

        glow.color: "black"
        glow.spread: 0.7
        glow.radius: 6
        glow.samples: 24
    }

    Image {
        source: "../../../image/system/chain"
        anchors.centerIn: parent
        visible: parent.chained
    }

    Image {
        source: "../../../image/system/death/save-me"
        anchors.centerIn: parent
        visible: parent.dying
    }

    Rectangle {
        id: disableMask
        anchors.fill: parent
        color: "black"
        opacity: 0.3
        visible: root.state == "candidate" && !root.selectable
    }

    DelayedTrickArea {
        id: delayedTrickAreaItem
        columns: 1
        x: -15
        y: 18
    }

    Image {
        source: root.phase != "inactive" ? "../../../image/system/phase/" + root.phase + ".png" : ""
        width: parent.width * 0.9
        height: implicitHeight / implicitWidth * width
        x: (parent.width - width) / 2
        y: parent.height - 3
        visible: root.phase != "inactive"
    }

    Colorize {
        anchors.fill: parent
        source: parent
        hue: 0
        saturation: 0
        lightness: 0
        visible: !parent.alive
    }

    Image {
        anchors.centerIn: parent
        source: "../../../image/system/death/" + userRole
        visible: !parent.alive
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (parent.state != "candidate" || !parent.selectable)
                return;
            parent.selected = !parent.selected;
        }
    }

    KingdomBox {
        x: parent.width - width - 4
        y: 4
        value: parent.kingdom
    }

    RoleComboBox {
        x: 3
        y: 2
        value: parent.userRole
    }

    ProgressBar {
        id: progressBarItem
        width: parent.width
        height: 10
        y: parent.height + 10
        visible: false
    }

    Text {
        id: marks
        width: 32
        x: parent.width + 2
        font.pixelSize: 14
        color: "white"
        // wrapMode: Text.WrapAnywhere
        text: clientPlayer === null ? "" : clientPlayer.mark_doc
    }

    InvisibleCardArea {
        id: handcardAreaItem
        anchors.centerIn: parent
    }

    InvisibleCardArea {
        id: defaultArea
        anchors.centerIn: parent
    }

    SpecialArea {
        id: specialArea
    }

    SequentialAnimation {
        id: trembleAnimation
        running: false
        PropertyAnimation {
            target: root
            property: "x"
            to: root.x - 20
            easing.type: Easing.InQuad
            duration: 100
        }
        PropertyAnimation {
            target: root
            property: "x"
            to: root.x
            easing.type: Easing.OutQuad
            duration: 100
        }
    }

    Connections {
        target: clientPlayer
        enabled: clientPlayer !== null
        function onPile_changed(name) {
            let pile = clientPlayer.getPile(name)
            let model = specialArea.pileModel
            if (pile.length === 0) {
                for (let i = 0; i < model.length; i++) {
                    if (model[i].str === name) {
                        model.splice(i, 1)
                        break
                    }
                }
            } else {
                let createNew = true
                for (let j = 0; j < model.length; j++) {
                    if (model[j].str === name) {
                        model[j].cids = pile
                        createNew = false
                        break
                    }
                }
                if (createNew) model.push({ str: name, cids: pile})
            }
            specialArea.pileModel = model
        }
    }

    function add(inputs)
    {
        defaultArea.add(inputs);
    }

    function remove(outputs)
    {
        return defaultArea.remove(outputs);
    }

    function updateCardPosition(animated)
    {
        defaultArea.updateCardPosition(animated);
    }

    function tremble() {
        trembleAnimation.start()
    }
}
