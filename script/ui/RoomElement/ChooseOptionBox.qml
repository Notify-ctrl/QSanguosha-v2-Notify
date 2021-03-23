import QtQuick 2.4
import "../Util"

GraphicsBox {
    property var options: []
    property int result

    id: root
    title.text: qsTr("Please choose")
    width: Math.max(140, body.width + 20)
    height: body.height + title.height + 20

    Column {
        id: body
        x: 10
        y: title.height + 5
        spacing: 10

        Repeater {
            model: options

            MetroButton {
                text: qsTr(modelData)
                anchors.horizontalCenter: parent.horizontalCenter

                onClicked: {
                    result = index;
                    root.close();
                }
            }
        }
    }
}
