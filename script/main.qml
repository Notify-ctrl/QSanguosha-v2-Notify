import QtQuick 2.15
import QtQuick.Controls 2.5
import "ui/Util"

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 1366
    height: 768

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

    Rectangle {
        id: toast
        opacity: 0
        z: 999
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height * 0.8
        radius: 16
        color: "#F2808A87"
        height: toast_text.height + 20
        width: toast_text.width + 40
        Text {
            id: toast_text
            text: "QSanguosha"
            anchors.centerIn: parent
            color: "white"
        }
        Behavior on opacity {
            NumberAnimation {
                duration: 240
                easing.type: Easing.InOutQuad
            }
        }
        SequentialAnimation {
            id: keepAnim
            running: toast.opacity == 1
            PauseAnimation {
                duration: 2800
            }

            ScriptAction {
                script: {
                    toast.opacity = 0
                }
            }
        }

        function show(text) {
            opacity = 1
            toast_text.text = text
        }
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

        if (true) { //(skip_splash) {
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
