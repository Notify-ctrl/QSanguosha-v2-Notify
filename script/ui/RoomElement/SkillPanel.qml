import QtQuick 2.15


Item {
    property var skills: []
    property alias skill_buttons: skill_buttons

    width: childrenRect.width
    height: childrenRect.height

    Repeater {
        id: skill_buttons
        model: skills

        SkillButton {
            columns: (skills.length % 2 == 0 || index < skills.length - 1) ? 2 : 1
            x: (index % 2 == 1) ? width + 1 : 0
            y: Math.floor(index / 2) * (height + 1)

            name: modelData.name
            type: modelData.type
            enabled: modelData.enabled
            pressed: modelData.pressed

            onPressedChanged: {
                if (enabled && type === "proactive")
                    roomScene.activateSkill(modelData.name, pressed);
            }
        }
    }
}
