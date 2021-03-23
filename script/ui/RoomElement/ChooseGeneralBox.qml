import QtQuick 2.4
import "../Util"

GraphicsBox {
    property alias model: generalList
    property int choiceNum: 1
    property var choices: []
    property var selectedItem: []

    ListModel {
        id: generalList
    }

    id: root
    title.text: qsTr("Please choose ") + choiceNum + qsTr(" general(s)")
    width: generalArea.width + body.anchors.leftMargin + body.anchors.rightMargin
    height: body.implicitHeight + body.anchors.topMargin + body.anchors.bottomMargin

    Column {
        id: body
        anchors.fill: parent
        anchors.margins: 40
        anchors.bottomMargin: 20

        Item {
            id: generalArea
            width: generalList.count >= 5 ? Math.ceil(generalList.count / 2) : Math.max(3, generalList.count) * 97
            height: generalList.count >= 5 ? 290 : 150
            z: 1

            Repeater {
                id: generalMagnetList
                model: generalList.count

                Item {
                    width: 93
                    height: 130
                    x: (index % Math.ceil(generalList.count / (generalList.count >= 5 ? 2 : 1))) * 98 + (generalList.count >= 5 && index > generalList.count / 2 && generalList.count % 2 == 1 ? 50 : 0)
                    y: generalList.count < 5 ? 0 : (index < generalList.count / 2 ? 0 : 135)
                }
            }
        }

        Item {
            id: splitLine
            width: parent.width - 80
            height: 6
            anchors.horizontalCenter: parent.horizontalCenter
            clip: true

            Image {
                anchors.centerIn: parent
                source: "../../../image/choosegeneralbox/split-line"
            }
        }

        Item {
            width: parent.width
            height: 165

            Row {
                id: resultArea
                anchors.centerIn: parent
                spacing: 10

                Repeater {
                    id: resultList
                    model: choiceNum

                    Rectangle {
                        color: "#1D1E19"
                        radius: 3
                        width: 93
                        height: 130
                    }
                }
            }
        }

        Item {
            id: buttonArea
            width: parent.width
            height: 40

            MetroButton {
                id: fightButton
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                text: qsTr("Fight")
                width: 120
                height: 35
                enabled: false

                onClicked: close();
            }
        }
    }

    Repeater {
        id: generalCardList
        model: generalList

        GeneralCardItem {
            gid: model.gid
            name: model.name
            kingdom: model.kingdom
            selectable: true
            draggable: true

            onClicked: {
                var toSelect = true;
                for (var i = 0; i < selectedItem.length; i++) {
                    if (selectedItem[i] === this) {
                        toSelect = false;
                        selectedItem.splice(i, 1);
                    }
                }
                if (toSelect && selectedItem.length < choiceNum)
                    selectedItem.push(this);
                updatePosition();
            }

            onReleased: {
                if (!isClicked)
                    arrangeCards();
            }
        }
    }

    function arrangeCards()
    {
        var item, i;

        selectedItem = [];
        for (i = 0; i < generalList.count; i++) {
            item = generalCardList.itemAt(i);
            if (item.y > splitLine.y)
                selectedItem.push(item);
        }

        selectedItem.sort(function(a, b){
            return a.x - b.x;
        });

        if (selectedItem.length > choiceNum)
            selectedItem.splice(choiceNum, selectedItem.length - choiceNum);

        updatePosition();
    }

    function updatePosition()
    {
        choices = [];
        var item, magnet, pos, i;
        for (i = 0; i < selectedItem.length && i < resultList.count; i++) {
            item = selectedItem[i];
            choices.push(item.gid);
            magnet = resultList.itemAt(i);
            pos = root.mapFromItem(resultArea, magnet.x, magnet.y);
            if (item.homeX !== pos.x || item.homeY !== item.y) {
                item.homeX = pos.x;
                item.homeY = pos.y;
                item.goBack(true);
            }
        }

        fightButton.enabled = (choices.length == choiceNum);

        for (i = 0; i < generalCardList.count; i++) {
            item = generalCardList.itemAt(i);
            if (selectedItem.contains(item))
                continue;

            magnet = generalMagnetList.itemAt(i);
            pos = root.mapFromItem(generalMagnetList.parent, magnet.x, magnet.y);
            if (item.homeX !== pos.x || item.homeY !== item.y) {
                item.homeX = pos.x;
                item.homeY = pos.y;
                item.goBack(true);
            }
        }
    }
}
