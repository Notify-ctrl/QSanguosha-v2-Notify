import QtQuick 2.15


Item {
    property string value: "unknown"

    width: 22
    height: 22
    visible: value != "hidden"

    Item {
        anchors.fill: parent
        visible: value == "unknown"

        SimpleKingdomIcon {
            id: wei
            originalColor: "#2779AA"
        }

        SimpleKingdomIcon {
            id: qun
            originalColor: "#C3B154"
            x: parent.width / 2
        }

        SimpleKingdomIcon {
            id: shu
            originalColor: "#AC3120"
            y: parent.height / 2
        }

        SimpleKingdomIcon {
            id: wu
            originalColor: "#55A434"
            x: parent.width / 2
            y: parent.height / 2
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            expandingBox.forceActiveFocus();
            expandingBox.visible = true;
        }
    }

    Image {
        id: expandingBox
        source: "../../../image/kingdom/expanding"
        focus: true
        visible: false

        onActiveFocusChanged: {
            if (!activeFocus)
                visible = false;
        }

        Image {
            source: "../../../image/kingdom/expandingmask/wei"
            x: 3
            y: 3
            visible: !wei.included
        }

        Image {
            source: "../../../image/kingdom/expandingmask/qun"
            x: 23
            y: 3
            visible: !qun.included
        }

        Image {
            source: "../../../image/kingdom/expandingmask/shu"
            x: 3
            y: 23
            visible: !shu.included
        }

        Image {
            source: "../../../image/kingdom/expandingmask/wu"
            x: 23
            y: 23
            visible: !wu.included
        }

        MouseArea {
            width: parent.width / 2
            height: parent.height / 2
            onClicked: wei.included = !wei.included;
        }

        MouseArea {
            width: parent.width / 2
            height: parent.height / 2
            x: parent.width / 2
            onClicked: qun.included = !qun.included;
        }

        MouseArea {
            width: parent.width / 2
            height: parent.height / 2
            y: parent.height / 2
            onClicked: shu.included = !shu.included;
        }

        MouseArea {
            width: parent.width / 2
            height: parent.height / 2
            x: parent.width / 2
            y: parent.height / 2
            onClicked: wu.included = !wu.included;
        }
    }

    Image {
        source: value && value != "hidden" ? "../../../image/kingdom/" + value : ""
        visible: value != "unknown" && value != "hidden"
        anchors.centerIn: parent
    }
}
