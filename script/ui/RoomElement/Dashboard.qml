import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Sanguosha 1.0
import "../Util"

RowLayout {
    property string headGeneral: ""
    property alias headGeneralKingdom: headGeneralItem.kingdom
    property string deputyGeneral: ""
    property alias deputyGeneralKingdom: deputyGeneralItem.kingdom
    property ClientPlayer clientPlayer: Self
    property int seat: 0
    property string userRole: "unknown"
    property string kingdom: "unknown"
    property alias hp: hpBar.value
    property alias maxHp: hpBar.maxValue
    property string phase: "inactive"
    property bool chained: false
    property bool dying: false
    property bool alive: true
    property bool drunk: Self === null || Self === undefined ? false : Self.getMark("drank") > 0
    property bool faceup: true
    property bool selectable: false
    property bool selected: false
    property string view_as_skill: "" // for pending card
    property bool is_pending: false
    property var pending_card
    property var pendings: [] // int[]
    property int selected_card: -1

    property alias acceptButton: acceptButtonItem
    property alias rejectButton: rejectButtonItem
    property alias finishButton: finishButtonItem
    property alias handcardArea: handcardAreaItem
    property alias equipArea: equipAreaItem
    property alias delayedTrickArea: delayedTrickAreaItem
    property alias specialArea: specialArea
    property alias progressBar: progressBarItem
    property alias headSkills: headSkillPanel.skills
    property alias headSkillButtons: headSkillPanel.skill_buttons
    property alias deputySkills: deputySkillPanel.skills

    signal accepted()
    signal rejected()
    signal finished()
    signal card_selected(var card)

    id: root
    spacing: 0
    Layout.fillHeight: false
    Layout.preferredHeight: 150

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
        }
    ]
    state: "normal"


    EquipArea {
        id: equipAreaItem
        Layout.preferredWidth: 164
        Layout.fillHeight: true
    }

    Rectangle {
        color: Qt.rgba(0, 0, 0, 0.65)
        Layout.fillWidth: true
        Layout.fillHeight: true

        RowLayout {
            anchors.fill: parent

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                HandcardArea {
                    id: handcardAreaItem
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: 15

                    Colorize {
                        anchors.fill: parent
                        source: parent
                        hue: 0
                        saturation: 0
                        lightness: 0
                        visible: !parent.alive
                    }

                    DelayedTrickArea {
                        id: delayedTrickAreaItem
                        width: parent.width
                        height: 30
                        rows: 1
                        y: -height
                    }

                    ProgressBar {
                        id: progressBarItem
                        width: parent.width * 0.4
                        height: 15
                        y: -height - 10
                        x: (parent.width - width) / 2
                        visible: false
                    }

                    Image {
                        source: root.phase != "inactive" ? "../../../image/system/phase/" + root.phase + ".png" : ""
                        y: -height - 5
                        x: parent.width - width
                        visible: root.phase != "inactive"
                    }

                    Image {
                        anchors.centerIn: parent
                        source: "../../../image/system/death/" + userRole
                        visible: !root.alive
                    }

                    Text {
                        id: marks
                        height: 32
                        anchors.right: parent.right
                        font.pixelSize: 14
                        color: "white"
                        // wrapMode: Text.WrapAnywhere
                        text: Self === null ? "" : Self.mark_doc
                    }

                    Connections {
                        target: root
                        function onPhaseChanged() {
                            handcardArea.enableCards([]);
                        }
                    }
                }

                Item {
                    Layout.preferredWidth: 20
                    Layout.fillHeight: true

                    HandcardNumber {
                        id: handcardNumItem
                        kingdom: headGeneralKingdom
                        value: handcardArea.length
                        y: parent.height - height - 5
                    }
                }
            }

            Image {
                id: platter
                source: "../../../image/dashboard/platter"

                Colorize {
                    anchors.fill: parent
                    source: parent
                    hue: 0
                    saturation: 0
                    lightness: 0
                    visible: !parent.alive
                }

                IrregularButton {
                    id: acceptButtonItem
                    name: "platter/confirm"
                    enabled: false
                    x: 6
                    y: 3

                    onClicked: root.accepted();
                }

                IrregularButton {
                    id: rejectButtonItem
                    name: "platter/cancel"
                    enabled: false
                    x: 6
                    y: 79

                    onClicked: root.rejected();
                }

                IrregularButton {
                    id: finishButtonItem
                    name: "platter/discard"
                    enabled: false
                    x: 67
                    y: 37

                    onClicked: root.finished();
                }

                Image {
                    source: "../../../image/system/role/" + userRole
                    x: 70
                    y: 3
                }

                Image {
                    x: 71
                    y: 117
                    source: seat > 0 ? "../../../image/dashboard/seatnum/" + seat : ""
                    visible: seat > 0
                }
            }
        }
    }

    Item {
        Layout.preferredWidth: deputyGeneralItem.visible ? 283 : 155
        Layout.preferredHeight: 149

        Colorize {
            anchors.fill: parent
            source: parent
            hue: 0
            saturation: 0
            lightness: 0
            visible: !parent.alive
        }

        Image {
            source: "../../../image/dashboard/base"
        }

        Image {
            source: "../../../image/dashboard/hpbase"
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: 3

            HpBar {
                id: hpBar
                visible: maxHp > 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: -3
                transform: Scale {
                    xScale: hpBar.parent.width / hpBar.width
                    yScale: xScale
                }
            }
        }

        Image {
            source: "../../../image/dashboard/avatarbg"
        }

        GeneralAvatar {
            id: headGeneralItem
            y: -4
            avatar: headGeneral ? headGeneral: "huangyueying"
            generalName: Sanguosha.translate(headGeneral)
            generalPosition: "head"

            Rectangle {
                color: Qt.rgba(250, 0, 0, 0.45)
                anchors.fill: parent
                visible: root.drunk
            }

            ToolTipArea {
                enabled: clientPlayer !== null
                text: clientPlayer === null ? "" : clientPlayer.getSkillDescription()
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (root.state != "candidate" || !root.selectable)
                        return;
                    root.selected = !root.selected;
                }
            }

            SkillPanel {
                id: headSkillPanel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 3
            }

            Image {
                anchors.fill: parent
                source: "../../../image/general/faceturned"
                visible: !faceup
            }

            RectangularGlow {
                id: outerGlow
                anchors.fill: parent
                visible: true
                glowRadius: 8
                spread: 0.4
                cornerRadius: 8
                z: -1
            }

            Rectangle {
                id: disableMask
                anchors.fill: parent
                color: "black"
                opacity: root.state === "candidate" && !root.selectable ? 0.3 : 0
                Behavior on opacity {
                    NumberAnimation {
                        duration: 200
                        easing.type: Easing.InOutQuad
                    }
                }
            }

            SpecialArea {
                id: specialArea
            }

            Connections {
                target: Self
                enabled: Self !== null
                function onPile_changed(name) {
                    let pile = Self.getPile(name)
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
        }

        GeneralAvatar {
            id: deputyGeneralItem
            x: 128
            y: -4
            avatar: deputyGeneral ? deputyGeneral : "zhugeliang"
            generalName: Sanguosha.translate(deputyGeneral)
            generalPosition: "deputy"
            visible: deputyGeneral ? true : false

            Rectangle {
                color: Qt.rgba(250, 0, 0, 0.45)
                anchors.fill: parent
                visible: root.drunk
            }

            SkillPanel {
                id: deputySkillPanel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 3
            }

            Image {
                anchors.fill: parent
                source: "../../../image/general/faceturned"
                visible: !faceup
            }
        }

        Image {
            source: "../../../image/system/chain"
            visible: root.chained
            anchors.horizontalCenter: headGeneralItem.horizontalCenter
            anchors.verticalCenter: headGeneralItem.verticalCenter
        }
    }

    function disableAllCards() {
        handcardAreaItem.enableCards([])
    }

    function unSelectAll(expectId) {
        handcardAreaItem.unselectAll(expectId);
    }

    function enableCards() {
        // @TODO: expand pile
        let ids = [], cards = handcardAreaItem.cards;
        for (let i = 0; i < cards.length; i++) {
            if (Router.card_isAvailable(cards[i].cid, Self.objectName))
                ids.push(cards[i].cid);
        }
        handcardAreaItem.enableCards(ids)
    }

    function cardSelected(cardId, selected) {
        if (view_as_skill !== "") {
            if (selected) {
                pendings.push(cardId);
            } else {
                pendings.splice(pendings.indexOf(cardId), 1);
            }

            updatePending();
        } else {
            if (selected) {
                handcardAreaItem.unselectAll(cardId);
                selected_card = cardId;
                card_selected(cardId);
            } else {
                handcardAreaItem.unselectAll();
                selected_card = -1;
                card_selected(-1);
            }
        }
    }

    function getSelectedCard() {
        if (view_as_skill !== "") {
            console.log(JSON.stringify({
                skill: view_as_skill,
                subcards: pendings
            }));
            return JSON.stringify({
                skill: view_as_skill,
                subcards: pendings
            });
        } else {
            return selected_card;
        }
    }

    function updatePending() {
        if (view_as_skill === "") return;

        let enabled_cards = [];

        handcardAreaItem.cards.forEach(function(card) {
            if (card.selected || Router.vs_view_filter(view_as_skill, pendings, card.cid))
                enabled_cards.push(card.cid);
        });
        handcardAreaItem.enableCards(enabled_cards);

        // @TODO: equipment

        if (Router.vs_can_view_as(view_as_skill, pendings)) {
            pending_card = {
                skill: view_as_skill,
                subcards: pendings
            };
            card_selected(JSON.stringify(pending_card));
        } else {
            pending_card = -1;
            card_selected(pending_card);
        }
    }

    function startPending(skill_name) {
        view_as_skill = skill_name;
        pendings = [];
        handcardAreaItem.unselectAll();

        // @TODO: expand pile

        // @TODO: equipment

        updatePending();
    }

    function deactivateSkillButton() {
        for (let i = 0; i < headSkills.length; i++) {
            headSkillButtons.itemAt(i).pressed = false;
        }
    }

    function stopPending() {
        view_as_skill = "";
        pendings = [];
        pending_card = -1;

        // @TODO: expand pile

        // @TODO: equipment

        handcardAreaItem.adjustCards();
        card_selected(-1);
    }
}
