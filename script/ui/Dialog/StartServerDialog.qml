import QtQuick 2.4

import Sanguosha.Dialogs 1.0

StartServerDialog {
    Component.onCompleted: {
        createServer();
    }

    Image {
        source: config.backgroundImage
        anchors.fill: parent
    }

    TextEdit {
        id: logs
        anchors.fill: parent
        anchors.margins: 20
        readOnly: true
        color: "white"
        font.pixelSize: 18
    }

    onMessageLogged: logs.append(message);
}
