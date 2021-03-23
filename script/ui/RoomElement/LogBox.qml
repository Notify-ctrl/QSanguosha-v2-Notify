import QtQuick 2.4
import QtQuick.Layouts 1.1
import "../Util"

Rectangle {
    color: Qt.rgba(0, 0, 0, 0.6)

    ColumnLayout {
        anchors.fill: parent

        Text {
            id: basicInfoItem
            Layout.fillWidth: true
            Layout.preferredHeight: 32
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Rectangle {
                width: 1
                height: parent.width
                gradient: Gradient {
                    GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0, 0) }
                    GradientStop { position: 0.5; color: Qt.rgba(166, 150, 122, 1.0) }
                    GradientStop { position: 1.0; color: Qt.rgba(0, 0, 0, 0) }
                }

                transform: Rotation {
                    angle: 270
                }
            }

            LogEdit {
                id: logEdit
                anchors.fill: parent
                anchors.margins: 10

                font.pixelSize: 14
                color: "white"
            }
        }
    }

    function append(log)
    {
        logEdit.append(log);
    }
}

