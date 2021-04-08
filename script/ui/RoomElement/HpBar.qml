import QtQuick 2.15


Column {
    id: root
    property int maxValue: 5
    property int value: 5

    Repeater {
        id: repeater
        model: maxValue <= 5 ? maxValue : 0
        Magatama {
            state: (maxValue - 1 - index) >= value ? 0 : (value >= 3 || value >= maxValue ? 3 : (value <= 0 ? 0 : value))
        }
    }

    Column {
        visible: maxValue > 5

        Magatama {
            state: (value >= 3 || value >= maxValue) ? 3 : (value <= 0 ? 0 : value)
        }

        GlowText {
            id: hpItem
            width: root.width
            text: value
            color: "#C3C322"
            font.family: "Arial"
            font.pixelSize: 24
            font.bold: true
            horizontalAlignment: Text.AlignHCenter

            glow.color: "black"
            glow.spread: 0.8
            glow.radius: 2
            glow.samples: 12
        }

        GlowText {
            id: splitter
            width: root.width
            text: "/"
            color: hpItem.color
            font: hpItem.font
            horizontalAlignment: hpItem.horizontalAlignment

            glow.color: hpItem.glow.color
            glow.spread: hpItem.glow.spread
            glow.radius: hpItem.glow.radius
            glow.samples: hpItem.glow.samples
        }

        GlowText {
            id: maxHpItem
            width: root.width
            text: maxValue
            color: hpItem.color
            font: hpItem.font
            horizontalAlignment: hpItem.horizontalAlignment

            glow.color: hpItem.glow.color
            glow.spread: hpItem.glow.spread
            glow.radius: hpItem.glow.radius
            glow.samples: hpItem.glow.samples
        }
    }
}
