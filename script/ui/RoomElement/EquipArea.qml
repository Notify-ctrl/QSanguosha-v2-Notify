import QtQuick 2.15
import QtGraphicalEffects 1.0
import "../Util/util.js" as Utility

Rectangle {
    signal cardSelected(int cardId, bool selected)
    property var subtypes: ["weapon", "armor", "defensive_horse", "offensive_horse", "treasure"]

    id: root
    color: Qt.rgba(0, 0, 0, 0.65)

    ListModel {
        id: cards

        ListElement {cid: 0; name: ""; suit: ""; number: 0; subtype: "weapon"}
        ListElement {cid: 0; name: ""; suit: ""; number: 0; subtype: "armor"}
        ListElement {cid: 0; name: ""; suit: ""; number: 0; subtype: "defensive_horse"}
        ListElement {cid: 0; name: ""; suit: ""; number: 0; subtype: "offensive_horse"}
        ListElement {cid: 0; name: ""; suit: ""; number: 0; subtype: "treasure"}
    }

    InvisibleCardArea {
        id: area
        anchors.centerIn: parent
    }

    Column {
        x: 7
        y: 13
        spacing: 6

        Repeater {
            model: cards.count

            Rectangle {
                width: 145
                height: 22

                LinearGradient {
                    anchors.fill: parent
                    start: Qt.point(0, 0)
                    end: Qt.point(parent.width, 0)
                    gradient: Gradient {
                        GradientStop {position: 0; color: "#444343"}
                        GradientStop {position: 1; color: "#2A2925"}
                    }
                }

                Rectangle {
                    width: parent.width - 1
                    height: parent.height - 2
                    y: 1

                    LinearGradient {
                        anchors.fill: parent
                        start: Qt.point(0, 0)
                        end: Qt.point(parent.width, 0)
                        gradient: Gradient {
                            GradientStop {position: 0; color: "#252525"}
                            GradientStop {position: 1; color: "#1B1B1A"}
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: parent.height - 2
                        y: 1

                        LinearGradient {
                            anchors.fill: parent
                            start: Qt.point(0, 0)
                            end: Qt.point(parent.width, 0)
                            gradient: Gradient {
                                GradientStop {position: 0; color: "#2C2C2C"}
                                GradientStop {position: 1; color: "#181818"}
                            }
                        }
                    }
                }
            }
        }
    }

    Column {
        x: 7
        y: 13
        spacing: 6

        Repeater {
            id: equipItems
            model: cards

            EquipItem {
                name: model.name
                suit: model.suit
                number: model.number

                onSelectedChanged: cardSelected(model.cid, selected);
            }
        }
    }

    function add(inputs)
    {
        var data;
        if (inputs instanceof Array) {
            for (var i = 0; i < inputs.length; i++) {
                data = inputs[i].toData();
                cards.set(subtypes.indexOf(data.subtype), data);
                equipItems.itemAt(subtypes.indexOf(data.subtype)).show();
            }
        } else {
            data = inputs.toData();
            cards.set(subtypes.indexOf(data.subtype), data);
            equipItems.itemAt(subtypes.indexOf(data.subtype)).show();
        }
        area.add(inputs);
    }

    function remove(outputs)
    {
        var result = area.remove(outputs);
        for (var i = 0; i < result.length; i++) {
            for (var j = 0; j < cards.count; j++) {
                if (result[i].cid === cards.get(j).cid) {
                    cards.set(j, {cid: 0, name: "", suit: "", number: 0});
                    equipItems.itemAt(j).hide();
                }
            }
        }
        return result;
    }

    function updateCardPosition(animated)
    {
        area.updateCardPosition(animated);
    }

    function enableCards(cardIds)
    {
        var card, i, item;
        for (i = 0; i < cards.count; i++) {
            card = cards.get(i);
            item = equipItems.itemAt(i);
            item.selectable = cardIds.contains(card.cid);
            if (!item.selectable)
                item.selected = false;
        }
    }
}
