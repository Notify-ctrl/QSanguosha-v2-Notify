import QtQuick 2.4


Rectangle {
    property bool included: true
    property color originalColor: "white"

    id: root
    width: parent.width / 2
    height: parent.height / 2
    border.color: "black"
    border.width: 1
    color: included ? originalColor : "gray"
}
