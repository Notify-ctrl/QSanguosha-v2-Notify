import QtQuick 2.15
import "../Util"
import Sanguosha.Dialogs 1.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

StartServerDialog {
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
            contentWidth: logs.width
            contentHeight: logs.height
            ScrollBar.vertical: ScrollBar {}
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            Rectangle {
                visible: !accepted
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
                        TextEdit {
                            font.pixelSize: 18
                            text: Sanguosha.getConfig("ServerName")
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
                            }
                            SpinBox {
                                id: commonModePlayerNum
                                enabled: commonMode.checked
                                from: 2
                                to: 8
                                value: 5
                            }
                            RadioButton {
                                id: scenarioMode
                                text: qsTr("Scenario")
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
                            }
                            RadioButton {
                                id: miniScenario
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

                    RowLayout {
                        anchors.rightMargin: 8
                        spacing: 16
                        MetroButton {
                            text: qsTr("Start Server")
                            onClicked: {
                                accepted = true
                                createServer()
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

    onMessageLogged: {
        logs.append(message);
    }

    function config() {}
}
