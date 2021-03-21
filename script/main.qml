import QtQuick 2.15

Item {
    id: root

    Loader {
        id: startSceneLoader
        anchors.fill: parent
    }

    Loader {
        id: dialogLoader
        z: 100
        anchors.fill: parent
        onSourceChanged: startSceneLoader.visible = (source == "");
    }

    Loader {
        id: splashLoader
        anchors.fill: parent
        focus: true
    }

    Component.onCompleted: {
        var skip_splash = false;
        for (var i = 0; i < Qt.application.arguments.length; i++) {
            var schema = "qsanguosha://";
            var arg = Qt.application.arguments[i];
            if (arg.substr(0, schema.length).toLowerCase() === schema) {
                dialogLoader.setSource("ui/Dialog/StartGameDialog.qml");
                arg = arg.substr(schema.length, arg.indexOf("/", schema.length) - schema.length);
                dialogLoader.item.serverAddress = arg;
                dialogLoader.item.accepted();
                skip_splash = true;
                break;
            }
        }

        if (skip_splash) {
            startSceneLoader.source = "ui/StartScene.qml";
        } else {
            splashLoader.source = "ui/Splash.qml";
            splashLoader.item.disappearing.connect(function(){
                startSceneLoader.source = "ui/StartScene.qml";
            });
            splashLoader.item.disappeared.connect(function(){
                splashLoader.source = "";
            });
        }
    }
}
