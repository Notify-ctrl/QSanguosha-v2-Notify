import QtQuick 2.15

// cards: CardItem[]
// add(inputs: CardItem[] | CardItem)
// remove(outputs: number[] | number)
Item {
    property var cards: []
    property int length: 0

    id: root

    function add(inputs)
    {
        if (inputs instanceof Array) {
            cards.push(...inputs);
        } else {
            cards.push(inputs);
        }
        length = cards.length;
    }

    function remove(outputs)
    {
        var result = [];
        for (var i = 0; i < cards.length; i++) {
            for (var j = 0; j < outputs.length; j++) {
                if (outputs[j] === cards[i].cid) {
                    result.push(cards[i]);
                    cards.splice(i, 1);
                    i--;
                    break;
                }
            }
        }
        length = cards.length;
        return result;
    }

    function updateCardPosition(animated)
    {
        var i, card;

        var overflow = false;
        for (i = 0; i < cards.length; i++) {
            card = cards[i];
            card.homeX = i * card.width;
            if (card.homeX + card.width >= root.width) {
                overflow = true;
                break;
            }
            card.homeY = 0;
        }

        if (overflow) {
            //@to-do: Adjust cards in multiple lines if there are too many cards
            var xLimit = root.width - card.width;
            var spacing = xLimit / (cards.length - 1);
            for (i = 0; i < cards.length; i++) {
                card = cards[i];
                card.homeX = i * spacing;
                card.homeY = 0;
            }
        }

        var parentPos = roomScene.mapFromItem(root, 0, 0);
        for (i = 0; i < cards.length; i++) {
            card = cards[i];
            card.homeX += parentPos.x;
            card.homeY += parentPos.y;
        }

        if (animated) {
            for (i = 0; i < cards.length; i++)
                // cards[i].goBack(true);
                roomScene.cardItemGoBack(cards[i], true)
        }
    }
}
