import QtQuick 2.15


Item {
    property var skills: []

    width: childrenRect.width
    height: childrenRect.height

    Repeater {
        model: skills

        SkillButton {
            columns: (skills.length % 2 == 0 || index < skills.length - 1) ? 2 : 1
            x: (index % 2 == 1) ? width + 1 : 0
            y: Math.floor(index / 2) * (height + 1)

            name: modelData//.name
            //type: modelData.type
            //enabled: modelData.enabled

            onPressedChanged: {
                if (enabled)
                    ; //roomScene.onSkillActivated(modelData.sid, pressed);
            }
        }
    }
}
