import QtQuick 2.4


Image {
    property string value: "unknown"
    property var options: ["unknown", "loyalist", "rebel", "renegade"]

    id: root
    source: visible ? "image://root/role/" + value : ""
    visible: value != "hidden"

    Image {
        property string value: "unknown"

        id: assumptionBox
        source: "image://root/role/" + value
        visible: root.value == "unknown"

        MouseArea {
            anchors.fill: parent
            onClicked: optionPopupBox.visible = true;
        }
    }

    Column {
        id: optionPopupBox
        visible: false
        spacing: 2

        Repeater {
            model: options

            Image {
                source: "image://root/role/" + modelData

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        optionPopupBox.visible = false;
                        assumptionBox.value = modelData;
                    }
                }
            }
        }
    }
}
