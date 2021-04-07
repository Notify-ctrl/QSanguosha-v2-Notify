import QtQuick 2.15


GraphicsBox {
    property int spacing: 5
    property string currentPlayerName: ""

    signal cardSelected(int cid)

    id: root
    title.text: qsTr("Please choose cards")
    width: cards.count * 100 + spacing * (cards.count - 1) + 25
    height: 180

    ListModel {
        id: cards
    }

    Row {
        x: 20
        y: 35
        spacing: root.spacing

        Repeater {
            model: cards

            CardItem {
                cid: model.cid
                name: model.name
                suit: model.suit
                number: model.number
                autoBack: false
                selectable: model.selectable
                footnote: model.footnote
                onClicked: root.cardSelected(cid);
            }
        }
    }

    function addCards(inputs)
    {
        for (var i = 0;i < inputs.length; i++)
            cards.append(inputs[i]);
    }

    function add(inputs)
    {
        drawPile.add(inputs);
    }

    function remove(outputs)
    {
        var result = drawPile.remove(outputs);
        for (var i = 0; i < result.length; i++) {
            var removed = result[i];
            for (var j = 0; j < cards.count; j++) {
                var card = cards.get(j);
                if (removed.cid === card.cid) {
                    card.selectable = false;
                    card.footnote = currentPlayerName
                }
            }
        }
        return result;
    }

    function updateCardPosition(animated)
    {
        area.updateCardPosition(animated);
    }
}
