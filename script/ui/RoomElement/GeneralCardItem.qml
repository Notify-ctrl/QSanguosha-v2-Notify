import QtQuick 2.15

CardItem {
    property int gid: 0
    property string kingdom: "qun"
    description: Sanguosha.getGeneralDescription(name)
    suit: ""
    number: 0
    card.source: "../../../image/general/card/" + name + ".jpg"
    glow.color: "white" //Engine.kingdomColor[kingdom]
}
