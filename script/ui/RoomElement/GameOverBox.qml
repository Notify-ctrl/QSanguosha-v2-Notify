import QtQuick 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import "../Util"

GraphicsBox {
    title.text: qsTr("WINNERS")
    width: 600
    height: 420

    ListModel {
        id: winnerList
    }

    TableView {
        anchors.fill: parent
        anchors.margins: 40
        anchors.bottomMargin: 20
        backgroundVisible: false

        model: winnerList
        style: TableViewStyle {
            backgroundColor: Qt.rgba(0, 0, 0, 0)
            alternateBackgroundColor: Qt.rgba(0, 0, 0, 0)
        }

        TableViewColumn {
            role: "gameRole"
            title: qsTr("Role")
            width: 100
        }
        TableViewColumn {
            role: "userName"
            title: qsTr("User Name")
            width: 150
        }

        TableViewColumn {
            role: "general"
            title: qsTr("General")
            width: 250
        }

        headerDelegate: Item {
            height: 30
            Text {
                text: styleData.value
                font.pixelSize: 24
                color: Qt.rgba(255, 255, 255, 1)
            }
        }

        rowDelegate: Item {
            height: 30
        }

        itemDelegate: Text {
            text: styleData.value
            font.pixelSize: 20
            color: Qt.rgba(255, 255, 255, 1)
        }
    }

    Row {
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 4
        MetroButton {
            text: qsTr("Restart")
            onClicked: {
                dialogLoader.setSource("../Dialog/StartGameDialog.qml")
                dialogLoader.item.connectToServer()
            }
        }
        MetroButton {
            text: qsTr("Main Menu")
            onClicked: {
                close()
                dialogLoader.setSource("")
            }
        }
    }

    function add(item)
    {
        winnerList.append(item);
    }
}
