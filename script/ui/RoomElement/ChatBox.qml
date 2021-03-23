import QtQuick 2.4
import QtQuick.Layouts 1.1
import "../Util"


Rectangle {
    color: Qt.rgba(0, 0, 0, 0.6)

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            LogEdit {
                id: chatLogBox
                anchors.fill: parent
                anchors.margins: 10
                font.pixelSize: 14
                color: "white"
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            color: "#040403"
            radius: 3
            border.width: 1
            border.color: "#A6967A"

            TextInput {
                anchors.fill: parent
                anchors.margins: 6
                color: "white"
                clip: true
                font.pixelSize: 14

                onAccepted: {
                    if (text != "") {
                        chatLogBox.append(text);
                        text = "";
                    }
                }
            }
        }
    }
}
