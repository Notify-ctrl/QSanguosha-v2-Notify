import QtQuick 2.4
import QtQuick.Layouts 1.1
import "../Util"

RowLayout {
    property string headGeneralName: ""
    property alias headGeneralKingdom: headGeneralItem.kingdom
    property string deputyGeneralName: ""
    property alias deputyGeneralKingdom: deputyGeneralItem.kingdom
    property int seatNumber: 0
    property string userRole: "unknown"
    property string kingdom: "unknown"
    property alias hp: hpBar.value
    property alias maxHp: hpBar.maxValue
    property string phase: "inactive"
    property bool chained: false
    property bool dying: false
    property bool alive: true
    property bool drunk: false
    property bool selectable: false
    property bool selected: false

    property alias acceptButton: acceptButtonItem
    property alias rejectButton: rejectButtonItem
    property alias finishButton: finishButtonItem
    property alias handcardArea: handcardAreaItem
    property alias equipArea: equipAreaItem
    property alias delayedTrickArea: delayedTrickAreaItem
    property alias progressBar: progressBarItem
    property alias headSkills: headSkillPanel.skills
    property alias deputySkills: deputySkillPanel.skills

    signal accepted()
    signal rejected()
    signal finished()

    id: root
    spacing: 0
    Layout.fillHeight: false
    Layout.preferredHeight: 150

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
                    source: seatNumber > 0 ? "../../../image/dashboard/seatnum/" + seatNumber : ""
                    visible: seatNumber > 0
                }
            }
        }
    }

    Item {
        Layout.preferredWidth: deputyGeneralItem.visible ? 283 : 155
        Layout.preferredHeight: 149

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
            avatar: headGeneralName ? headGeneralName: "huangyueying"
            generalName: Sanguosha.translate(headGeneralName)
            generalPosition: "head"

            Rectangle {
                color: Qt.rgba(250, 0, 0, 0.45)
                anchors.fill: parent
                visible: root.drunk
            }

            SkillPanel {
                id: headSkillPanel
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 3
            }
        }

        GeneralAvatar {
            id: deputyGeneralItem
            x: 128
            y: -4
            avatar: deputyGeneralName ? deputyGeneralName : "zhugeliang"
            generalName: Sanguosha.translate(deputyGeneralName)
            generalPosition: "deputy"
            visible: deputyGeneralName ? true : false

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
        }

        Image {
            source: "../../../image/system/chain"
            visible: root.chained
            anchors.horizontalCenter: headGeneralItem.right
            anchors.verticalCenter: headGeneralItem.verticalCenter
        }
    }
}
