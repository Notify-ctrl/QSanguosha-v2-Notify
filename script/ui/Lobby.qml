import QtQuick 2.15
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import "Util"

import Sanguosha 1.0

Lobby {
    anchors.fill: parent
    property int roomId: 0
    property alias roomLogo: roomLogoItem.source
    property alias chatLog: chatLogItem.text
    property alias userAvatar: userAvatarItem.source
    property alias userName: userNameItem.text
    property bool isOwner: false

    Item {
        id: config
        property string name: qsTr("QSanguosha Lobby")
        property string mode: "standard"
        property int capacity: 0
        property int timeout: 0
    }

    onSetConfig: config[key] = value;

    onMessageLogged: chatLogItem.append(message);

    onRoomListUpdated: {
        var room, i, item;
        var roomMap = {};
        for (i = 0; i < rooms.length; i++)
            roomMap[rooms[i].id] = rooms[i];

        for (i = 0; i < roomList.count; i++) {
            item = roomList.get(i);
            room = roomMap[item.rid];
            if (room === undefined) {
                roomList.remove(i);
                i--;
            } else {
                item.name = room.name;
                item.userNum = room.userNum;
                item.capacity = room.capacity;
                delete roomMap[item.rid];
            }
        }

        for (i in roomMap) {
            room = roomMap[i];
            var info = {
                rid: room.id,
                name: room.name,
                userNum: room.userNum,
                capacity: room.capacity
            };
            roomList.append(info);
        }
    }
    onRoomIdChanged: {
        if (roomId > 0) {
            roomListShowAnimation.stop();
            roomListHideAnimation.start();
        } else {
            roomListHideAnimation.stop();
            roomListShowAnimation.start();
        }
    }
    onGameStarted: {
        dialogLoader.setSource("RoomScene.qml");
    }

    Rectangle {
        anchors.fill: parent
        color: "#696367"
    }

    ListModel {
        id: roomList
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 32

        ListView {
            id: roomView
            Layout.preferredWidth: 160
            Layout.fillHeight: true
            clip: true
            x: 10
            spacing: 10
            model: roomList
            delegate: Component {
                TileButton {
                    backgroundColor: "#3A5D59"
                    border.color: Qt.rgba(0, 0, 0, 0)
                    text: ""
                    width: 160
                    height: 60

                    Item {
                        anchors.fill: parent
                        anchors.margins: 5

                        Text {
                            text: name
                            color: "#E1DF95"
                            font.pixelSize: 20
                        }

                        Text {
                            text: "<" + rid + ">"
                            color: "#D5D8D1"
                            font.pixelSize: 15
                            y: parent.height - height
                        }

                        Text {
                            text: userNum + "/" + (capacity > 0 ? capacity : qsTr("unlimited"))
                            color: "#82906D"
                            font.pixelSize: 18
                            x: parent.width - width
                            y: parent.height - height
                        }
                    }

                    onClicked: onRoomListItemClicked(rid);
                }
            }

            PropertyAnimation {
                id: roomListHideAnimation
                target: roomView
                property: "Layout.preferredWidth"
                from: 160
                to: 0
                onStarted: roomListUpdater.stop();
            }

            PropertyAnimation {
                id: roomListShowAnimation
                target: roomView
                property: "Layout.preferredWidth"
                from: 0
                to: 160
                onStarted: roomListUpdater.start();
            }

            Timer {
                id: roomListUpdater
                interval: 10000
                repeat: true
                triggeredOnStart: true
                onTriggered: updateRoomList();
            }

            Component.onCompleted: roomListUpdater.start();
        }

        ColumnLayout {
            id: mainArea
            spacing: 10

            Item{
                height: 30
            }

            RowLayout {
                id: roomArea
                spacing: 10

                ColumnLayout {
                    spacing: 0

                    Rectangle {
                        color: "#BFBFBF"
                        Layout.fillWidth: true
                        height: 60

                        Canvas {
                            antialiasing: true
                            width: 30
                            height: 21
                            x: -width
                            y: parent.height - height

                            property color color: "#BFBFBF"
                            property real alpha: 1.0

                            onPaint: {
                                var ctx = getContext("2d");
                                ctx.save();
                                ctx.clearRect(0, 0, width, height);
                                ctx.fillStyle = color;
                                ctx.globalAlpha = alpha;
                                ctx.lineJoin = "round";
                                ctx.beginPath();
                                ctx.moveTo(0, 0);
                                ctx.lineTo(width, 2);
                                ctx.lineTo(width, height);
                                ctx.closePath();
                                ctx.fill();
                                ctx.restore();
                            }
                        }

                        Image{
                            id: roomLogoItem
                            source: "image://system/mogara/logo"
                            width: 100
                            height: 100
                            x: 20
                            y: -height / 2
                        }

                        Text {
                            id: roomNameItem
                            text: config.name
                            anchors.left: roomLogoItem.right
                            anchors.bottom: parent.bottom
                            anchors.leftMargin: 10
                            anchors.bottomMargin: 10
                            font.pixelSize: 26
                            color: "#695F5E"
                            verticalAlignment: Text.AlignBottom
                        }
                    }

                    Rectangle {
                        color: "#DDDDDB"
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        LogEdit {
                            id: chatLogItem
                            anchors.fill: parent
                            anchors.margins: 20
                            font.pixelSize: 20
                        }
                    }

                    Rectangle {
                        color: "#BFBFBF"
                        Layout.fillWidth: true
                        height: 60
                    }
                }

                ColumnLayout {
                    id: roomInfo
                    Layout.fillWidth: false
                    Layout.preferredWidth: 260
                    spacing: 0

                    Rectangle {
                        color: "#BFBFBF"
                        Layout.fillWidth: true
                        height: 60

                        Text {
                            text: qsTr("Information")
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.bottomMargin: 10
                            font.pixelSize: 26
                            color: "#695F5E"
                            verticalAlignment: Text.AlignBottom
                        }
                    }

                    Rectangle {
                        color: "#DDDDDB"
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        GridLayout {
                            anchors.fill: parent
                            anchors.margins: 15

                            columns: 2

                            Text {
                                text: qsTr("Room Name")
                                font.pixelSize: 16
                            }

                            TextField {
                                text: config.name
                                font.pixelSize: 14
                                readOnly: !isOwner
                                onEditingFinished: {
                                    if (isOwner)
                                        updateConfig("name", text);
                                }
                            }

                            Text {
                                text: qsTr("Game Mode")
                                font.pixelSize: 16
                            }

                            Row {
                                ExclusiveGroup { id: gameModeGroup }
                                RadioButton {
                                    text: qsTr("Standard")
                                    checked: config.mode == "standard"
                                    exclusiveGroup: gameModeGroup
                                    enabled: isOwner
                                    onCheckedChanged: {
                                        if (checked)
                                            updateConfig("mode", "standard");
                                    }
                                }
                                RadioButton {
                                    text: qsTr("Hegemony")
                                    checked: config.mode == "hegemony"
                                    exclusiveGroup: gameModeGroup
                                    enabled: isOwner
                                    onCheckedChanged: {
                                        if (checked)
                                            updateConfig("mode", "hegemony");
                                    }
                                }
                            }

                            Text {
                                text: qsTr("Capacity")
                                font.pixelSize: 16
                            }

                            TextField {
                                text: config.capacity
                                font.pixelSize: 14
                                readOnly: !isOwner
                                validator: IntValidator {
                                    top: 10
                                    bottom: 2
                                }
                                onEditingFinished: {
                                    if (isOwner)
                                        updateConfig("capacity", text);
                                }
                            }

                            Text {
                                text: qsTr("Timeout")
                                font.pixelSize: 16
                            }

                            TextField {
                                text: config.timeout
                                font.pixelSize: 14
                                readOnly: !isOwner
                                validator: IntValidator {
                                    top: 30
                                    bottom: 5
                                }
                                onEditingFinished: {
                                    if (isOwner)
                                        updateConfig("timeout", text);
                                }
                            }
                        }
                    }

                    Rectangle {
                        color: "#BFBFBF"
                        Layout.fillWidth: true
                        height: 60

                        Row {
                            spacing: 5
                            anchors.fill: parent
                            anchors.margins: 12

                            MetroButton {
                                height: 36
                                backgroundColor: "#A46061"
                                text: isOwner ? qsTr("Start") : qsTr("Ready")
                                textColor: "#EDC5C5"
                                textFont.pixelSize: 18
                                border.width: 0
                                onClicked: onReadyButtonClicked();
                            }

                            MetroButton {
                                height: 36
                                backgroundColor: "#A46061"
                                text: qsTr("Add Robot")
                                textColor: "#EDC5C5"
                                textFont.pixelSize: 18
                                border.width: 0
                                visible: isOwner

                                onClicked: onAddRobotButtonClicked();
                            }
                        }
                    }
                }
            }

            RowLayout {
                id: controlPanel
                Layout.fillHeight: false
                Layout.preferredHeight: 150
                spacing: 10

                ColumnLayout {
                    id: buttonArea
                    Layout.fillWidth: false
                    Layout.preferredWidth: 140
                    spacing: 10

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        color: "#F6F6F6"

                        TextInput {
                            anchors.fill: parent
                            color: "#000000"
                            font.pixelSize: 16
                            wrapMode: TextInput.Wrap
                            inputMethodHints: Qt.ImhDigitsOnly
                            verticalAlignment: TextInput.AlignVCenter
                            horizontalAlignment: TextInput.AlignHCenter
                            validator: IntValidator {
                                bottom: 1
                            }
                        }
                    }

                    MetroButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45
                        backgroundColor: "#455473"
                        textColor: "#D2BDFF"
                        text: qsTr("Join")
                        textFont.pixelSize: 28
                        border.width: 0
                    }

                    MetroButton {
                        Layout.fillWidth: true
                        width: 140
                        Layout.preferredHeight: 55
                        backgroundColor: "#A46061"
                        textColor: "#EDC5C5"
                        text: roomId <= 0 ? qsTr("Create") : qsTr("Exit")
                        textFont.pixelSize: 28
                        border.width: 0
                        onClicked: onCreateButtonClicked();
                    }
                }

                Rectangle {
                    id: chatBox
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#DDDDDB"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        ColumnLayout {
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 32
                                color: "#F6F6F6"

                                TextInput {
                                    id: chatInput
                                    anchors.fill: parent
                                    anchors.margins: 4
                                    color: "#000000"
                                    font.pixelSize: 16
                                    wrapMode: TextInput.Wrap
                                    verticalAlignment: TextInput.AlignVCenter
                                    clip: true
                                    onAccepted: send();

                                    function send(){
                                        speakToServer(text);
                                        text = "";
                                    }
                                }

                                Image {
                                    source: "image://system/mogara/enter_icon"
                                    width: 34
                                    height: 22
                                    x: parent.width - width
                                    y: parent.height + 5

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: chatInput.send();
                                    }
                                }
                            }

                            Item {
                                Layout.fillHeight: true
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: false
                            Layout.preferredWidth: 110

                            Rectangle {
                                Layout.preferredWidth: 110
                                Layout.preferredHeight: 110
                                color: "white"

                                Image {
                                    id: userAvatarItem
                                    anchors.fill: parent
                                }
                            }

                            Text {
                                id: userNameItem
                                Layout.fillWidth: true
                                Layout.preferredHeight: 16
                                font.pixelSize: 16
                                color: "#535351"
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }
            }
        }
    }
}
