import QtQuick 2.4
import QtQuick.Layouts 1.2

import Sanguosha.Dialogs 1.0

PcConsoleStartDialog {

    id: root

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
                text: qsTr("Screen Name")
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
                    id: screenNameInput
                    anchors.fill: parent
                    anchors.margins: 11
                    font.pixelSize: 20
                    color: "white"
                    clip: true

                    Component.onCompleted: forceActiveFocus();
                }
            }
        }

        ColumnLayout {
            spacing: 20

            MetroButton {
                width: 160
                height: 50
                text: qsTr("Create")
                onClicked: start(screenNameInput.text, "");
            }
            MetroButton {
                width: 160
                height: 50
                text: qsTr("Cancel")
                onClicked: dialogLoader.source = "";
            }
        }
    }

    onEnterLobby: dialogLoader.setSource("../Lobby.qml");
}
