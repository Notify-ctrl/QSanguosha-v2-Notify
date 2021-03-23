import QtQuick 2.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.12
import Sanguosha.Dialogs 1.0
import "../Util"

StartGameDialog {
    id: startGameDialog

    signal accepted
    signal rejected

    onEnterRoom: dialogLoader.setSource("../RoomScene.qml");

    onAccepted: {
        config()
        connectToServer();
    }

    onRejected: dialogLoader.source = "";

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
            contentWidth: dialog.width
            contentHeight: dialog.height
            ScrollBar.vertical: ScrollBar {}
            clip: true
            flickableDirection: Flickable.VerticalFlick
            Rectangle {
                id: dialog
                color: "transparent"
                height: childrenRect.height + 64
                width: childrenRect.width
                Column {
                    x: 32
                    y: 20
                    spacing: 20

                    GridLayout {
                        columns: 2
                        rowSpacing: 8
                        columnSpacing: 16
                        Text {
                            text: qsTr("Name")
                        }

                        TextField {
                            id: userName
                            selectByMouse: true
                            text: Sanguosha.getConfig("UserName", "sgsfans")
                        }

                        Text {
                            text: qsTr("Host Address")
                        }

                        ComboBox {
                            id: hostAddress
                            editable: true
                            width: 128 /*
                            function getModel() {
                                let ret = []
                                let history = Sanguosha.getConfig("HistoryIPs", "")
                                if (history instanceof Array)
                                    for (let i = 0; i < history.length; i++)
                                        ret.push(history[i])
                                else
                                    ret.push(history)
                                if (ret.length === 0)
                                    ret.push("localhost")
                                return ret
                            }

                            model: getModel()*/
                            model: ["localhost"]
                        }
                    }

                    RowLayout {
                        TextField {
                            id: userAvatar
                            selectByMouse: true
                            text: Sanguosha.getConfig("UserAvatar", "nos_zhangliao")

                            onAccepted: {
                                avatarImg.source = "../../../image/general/full/" + text;
                            }
                        }

                        MetroButton {
                            text: qsTr("Apply")
                            onClicked: avatarImg.source = "../../../image/general/full/" + userAvatar.text;
                        }
                    }

                    Image {
                        id: avatarImg
                        Component.onCompleted: {
                            source = "../../../image/general/full/" + userAvatar.text;
                        }
                    }

                    RowLayout {
                        anchors.rightMargin: 8
                        spacing: 16
                        CheckBox {
                            id: reconnect
                            text: "Reconnect"
                            checked: Sanguosha.getConfig("EnableReconnection", false)
                        }

                        MetroButton {
                            text: qsTr("Start Game")
                            enabled: avatarImg.height == 292 && userName.length > 0
                            onClicked: {
                                startGameDialog.accepted()
                            }
                        }
                        MetroButton {
                            text: qsTr("Cancel")
                            onClicked: {
                                startGameDialog.rejected()
                            }
                        }
                    }
                }
            }
        }
    }

    function config() {
        toast.show("Trying to connect to the host...")
        Sanguosha.setConfig("UserName", userName.text)
        Sanguosha.setConfig("HostAddress", hostAddress.editText)
        Sanguosha.setConfig("UserAvatar", userAvatar.text)
        Sanguosha.setConfig("EnableReconnection", reconnect.checked)
    }

    onErrorGet: {
        toast.show(error_msg)
    }

}
