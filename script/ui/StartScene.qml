import QtQuick 2.15
import "Util"

Image {
    id: startScene
    source: "../../image/background/bg"
    anchors.fill: parent

    FitInView {
        minWidth: 870
        minHeight: 700

        Image {
            id: logo
            source: "../../image/logo/logo"
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: -parent.width / 4
            opacity: 0

            ToolTipArea {
                text: qsTr("QSanguosha")
            }
        }

        GridView {
            id: grid
            interactive: false
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: parent.width / 4
            flow: GridView.FlowTopToBottom
            cellHeight: 162; cellWidth: 162
            height: cellHeight * 4; width: cellWidth * 2
            delegate: buttonDelegate
        }
    }

    NumberAnimation {
        id: logoAni
        target: logo
        property: "opacity"
        duration: 1000
        to: 1
        onStopped: grid.model = buttons;
    }

    Component {
        id: buttonDelegate

        TileButton {
            text: name
            iconSource: "../../image/system/tileicon/" + icon
            onClicked: {
                var dialog = icon.substr(0, 1).toUpperCase() + icon.substr(1);
                dialog = dialog.replace(/\_([a-z])/g, function(str, group1){
                    return group1.toUpperCase();
                });
                dialogLoader.setSource("Dialog/" + dialog + "Dialog.qml");
            }
        }
    }

    ListModel {
        id: buttons
        ListElement { name: qsTr("Start Game"); icon: "start_game" }
        ListElement { name: qsTr("Start Server"); icon: "start_server" }
        ListElement { name: qsTr("PC Console Start"); icon: "pc_console_start" }
        ListElement { name: qsTr("Replay"); icon: "replay" }
        ListElement { name: qsTr("Configure"); icon: "configure" }
        ListElement { name: qsTr("General Overview"); icon: "general_overview" }
        ListElement { name: qsTr("Card Overview"); icon: "card_overview" }
        ListElement { name: qsTr("About"); icon: "about" }
    }

    Timer {
        interval: 1000
        running: true
        onTriggered: logoAni.start()
    }
}

