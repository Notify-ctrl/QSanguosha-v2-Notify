import QtQuick 2.4
import "../../engine.js" as Engine

CardItem {
    property int gid: 0
    property string kingdom: "qun"
    suit: ""
    number: 0
    card.source: "image://root/general/card/" + name + ".jpg"
    glow.color: Engine.kingdomColor[kingdom]
}
