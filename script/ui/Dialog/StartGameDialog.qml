import QtQuick 2.4
import QtQuick.Layouts 1.2
import "../Util"

StartGameDialog {
    id: startGameDialog

    signal accepted
    signal rejected

    property alias serverAddress: serverAddressInput.text
    property alias screenName: screenNameInput.text

    onLobbyEntered: dialogLoader.setSource("../Lobby.qml");

    onAccepted: {
        signup(screenName, "");
        connectToServer(serverAddress);
    }

    onRejected: dialogLoader.source = "";

    Image {
        source: config.backgroundImage
        anchors.fill: parent
    }

    RowLayout {
        width: 600
        height: 270
        anchors.centerIn: parent
        spacing: 22

        GridLayout {
            columns: 2

            Text {
                text: qsTr("Server Address")
                color: "white"
                font.pixelSize: 20
                width: 50
                height: 22
            }

            Rectangle {
                color: Qt.rgba(0xFF, 0xFF, 0xFF, 0.3)
                Layout.fillWidth: true
                height: 42
                TextInput {
                    id: serverAddressInput
                    anchors.fill: parent
                    anchors.margins: 11
                    font.pixelSize: 20
                    color: "white"
                    clip: true
                    text: "127.0.0.1:5927"
                }
            }

            Text {
                text: qsTr("Screen Name")
                color: "white"
                font.pixelSize: 20
                width: 50
                height: 22

                Component.onCompleted: forceActiveFocus();
            }

            Rectangle {
                color: Qt.rgba(0xFF, 0xFF, 0xFF, 0.3)
                Layout.fillWidth: true
                height: 42
                TextInput {
                    id: screenNameInput
                    anchors.fill: parent
                    anchors.margins: 11
                    font.pixelSize: 20
                    color: "white"
                    clip: true
                }
            }
        }

        ColumnLayout {
            spacing: 20

            MetroButton {
                width: 160
                height: 50
                text: qsTr("Connect")
                onClicked: startGameDialog.accepted();
            }
            MetroButton {
                width: 160
                height: 50
                text: qsTr("Cancel")
                onClicked: startGameDialog.rejected();
            }
        }
    }
}
