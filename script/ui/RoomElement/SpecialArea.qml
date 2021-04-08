import QtQuick 2.15
import QtQuick.Controls 2.12
import "../Util"
import "../Util/util.js" as Utility

Item {
    x: 28
    y: 20
    property var pileModel: [] // { str, cids[] }[]

    Column {
    Repeater {
        id: piles
        model: pileModel
        Rectangle {
            height: 20
            width: 80
            color: "green"
            Text {
                anchors.centerIn: parent
                color: "white"
                text: Sanguosha.translate(modelData.str) + "(" + modelData.cids.length + ")"
            }

            ToolTipArea {
                function getToolText() {
                    let tmp = []
                    let ids = modelData.cids
                    for (let i = 0; i < ids.length; i++) {
                        let cardData = JSON.parse(Sanguosha.getCard4Qml(ids[i]))
                        if (cardData.name === "card-back") continue
                        tmp.push(Sanguosha.translate(cardData.name) + "[<img src='../../../image/system/log/"
                                 + cardData.suit + ".png' height = 14/> " + Utility.convertNumber(cardData.number) + "]")
                    }
                    return tmp.toString()
                }
                enabled: this.text !== ""
                text: getToolText()
            }
        }
    }
    }

    InvisibleCardArea {
        id: area
    }

    function add(inputs)
    {
        area.add(inputs);
    }

    function remove(outputs)
    {
        return area.remove(outputs);
    }

    function updateCardPosition(animated)
    {
        area.updateCardPosition(animated);
    }
}
