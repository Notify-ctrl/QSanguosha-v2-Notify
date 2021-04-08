import QtQuick 2.15
import "../Util"
import Sanguosha 1.0
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3

PcConsoleStartDialog {
    property bool accepted: false

    Image {
        source: "../../../image/background/bg"
        anchors.fill: parent
    }

    Rectangle {
        color: "#88888888"
        anchors.centerIn: parent
        height: 480
        width: 800
        Flickable {
            width: parent.width - 40
            height: parent.height - 20
            anchors.centerIn: parent
            contentWidth: accepted ? logs.width : dialog.width
            contentHeight: accepted ? logs.height : dialog.height
            ScrollBar.vertical: ScrollBar {}
            clip: true
            flickableDirection: Flickable.VerticalFlick
            Rectangle {
                id: dialog
                visible: !accepted
                color: "transparent"
                height: childrenRect.height + 64
                width: childrenRect.width
                Column {
                    x: 32
                    y: 20
                    spacing: 20

                    RowLayout {
                        anchors.rightMargin: 8
                        spacing: 16
                        Text {
                            text: qsTr("Server Name")
                        }
                        TextField {
                            id: serverName
                            font.pixelSize: 18
                            text: Sanguosha.getConfig("ServerName", "sgsfans's server")
                        }
                    }

                    RowLayout {
                        anchors.rightMargin: 8
                        spacing: 16
                        Text {
                            text: qsTr("Operation Time")
                        }
                        SpinBox {
                            id: operationTimeout
                            from: 15
                            to: 60
                            enabled: !operationNoLimit.checked
                            Component.onCompleted: {
                                value = Sanguosha.getConfig("OperationTimeout", 15)
                            }
                        }
                        CheckBox {
                            id: operationNoLimit
                            text: "OperationNoLimit"
                            Component.onCompleted: {
                                checked = Sanguosha.getConfig("OperationNoLimit", false)
                            }
                        }
                    }

                    Text {
                        text: qsTr("GameMode")
                    }

                    Frame {
                        GridLayout {
                            columns: 2
                            columnSpacing: 16
                            RadioButton {
                                id: commonMode
                                text: qsTr("Common Mode")
                                onCheckedChanged: {
                                    if (checked) {
                                        checkable = false
                                        scenarioMode.checked = false
                                        miniScenario.checked = false
                                    } else checkable = true
                                }
                                Component.onCompleted: {
                                    let mode = Sanguosha.getConfig("GameMode", "05p")
                                    if (mode.indexOf("p") !== -1) {
                                        checked = true
                                        commonModePlayerNum.value = parseInt(mode[1])
                                    }
                                }
                            }
                            SpinBox {
                                id: commonModePlayerNum
                                enabled: commonMode.checked
                                from: 2
                                to: 8
                            }
                            // @TODO: fix them
                            RadioButton {
                                id: scenarioMode
                                text: qsTr("Scenario")
                                checkable: false
                                onCheckedChanged: {
                                    if (checked) {
                                        checkable = false
                                        commonMode.checked = false
                                        miniScenario.checked = false
                                    } else checkable = true
                                }
                            }
                            ComboBox {
                                enabled: scenarioMode.checked
                                function getModel() {
                                    let ret = []
                                    let names = Sanguosha.getModScenarioNames()

                                    for (let i = 0; i < names.length; i++) {
                                        ret.push({text: Sanguosha.translate(names[i])})
                                    }

                                    return ret
                                }
                                model: getModel()
                            }
                            RadioButton {
                                id: miniScenario
                                checkable: false
                                text: qsTr("Mini Scnario")
                                onCheckedChanged: {
                                    if (checked) {
                                        checkable = false
                                        scenarioMode.checked = false
                                        commonMode.checked = false
                                    } else checkable = true
                                }
                            }
                            ComboBox {
                                enabled: miniScenario.checked
                                model: Sanguosha.getMiniScenarioNames()
                            }
                            Rectangle {
                                color: "transparent"
                                width: miniScenario.width
                                height: miniScenario.height
                            }
                            MetroButton {
                                enabled: miniScenario.checked
                                text: qsTr("Custom ...")
                            }
                        }
                    }

                    CheckBox {
                        id: disableLua
                        text: qsTr("Disable Lua")
                        Component.onCompleted: {
                            checked = Sanguosha.getConfig("DisableLua", false)
                        }
                    }

                    RowLayout {
                        anchors.rightMargin: 8
                        spacing: 16
                        MetroButton {
                            text: qsTr("PC Console Start")
                            onClicked: {
                                accepted = true
                                config()
                                consoleStart()
                            }
                        }
                        MetroButton {
                            text: qsTr("Cancel")
                            onClicked: {
                                accepted = false
                                dialogLoader.setSource("")
                            }
                        }
                    }
                }
            }

            TextEdit {
                id: logs
                visible: accepted
                height: parent.contentHeight
                width: parent.width
                wrapMode: Text.WordWrap
                readOnly: true
                selectByMouse: true
                color: "white"
                font.pixelSize: 18
                textFormat: Text.RichText
            }
        }
    }

    onEnterRoom: dialogLoader.setSource("../RoomScene.qml");

    function config() {
        Sanguosha.setConfig("ServerName", serverName.text)
        Sanguosha.setConfig("OperationTimeout", operationTimeout.value)
        Sanguosha.setConfig("OperationNoLimit", operationNoLimit.checked)

        let gameMode = ""
        if (commonMode.checked)
            gameMode = "0" + commonModePlayerNum.value + "p"
        // @TODO: add logic for scenario here

        Sanguosha.setConfig("GameMode", gameMode)
        Sanguosha.setConfig("DisableLua", disableLua.checked)
    }
}
