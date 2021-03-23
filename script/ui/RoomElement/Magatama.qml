import QtQuick 2.4

Image {
    source: "../../../image/general/magatamas/0"
    state: "3"

    states: [
        State {
            name: "3"
            PropertyChanges {
                target: main
                source: "../../../image/general/magatamas/3"
                opacity: 1
                scale: 1
            }
        },
        State {
            name: "2"
            PropertyChanges {
                target: main
                source: "../../../image/general/magatamas/2"
                opacity: 1
                scale: 1
            }
        },
        State {
            name: "1"
            PropertyChanges {
                target: main
                source: "../../../image/general/magatamas/1"
                opacity: 1
                scale: 1
            }
        },
        State {
            name: "0"
            PropertyChanges {
                target: main
                source: "../../../image/general/magatamas/0"
                opacity: 0
                scale: 4
            }
        }
    ]

    transitions: Transition {
        PropertyAnimation {
            properties: "opacity,scale"
        }
    }

    Image {
        id: main
    }
}
