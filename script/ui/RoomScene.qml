import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtMultimedia 5.12
import "Util"
import Sanguosha 1.0
import "RoomElement"
import "Util/util.js" as Utility

RoomScene {
    property var dashboardModel: null
    property var photoModel: []
    property int playerNum: 0
    property int currentPlayerNum: 1
    property var m_move_cache: ({})
    property var _m_cardsMoveStash: ({})

    function /* class */_MoveCardsClassifier(card_ids) {
        this.ids = card_ids
    }

    id: roomScene
    anchors.fill: parent

    // signal chat
    signal return_to_start

    FontLoader {
        source: "../../font/simli.ttf"
    }

    MediaPlayer {
        id: backgroundMusic
    }

    MediaPlayer {
        id: soundEffect
    }

    Image {
        source: "../../image/background/default.jpg"
        anchors.fill: parent
        focus: true
    }

    MouseArea {
        anchors.fill: parent
        onPressed: parent.forceActiveFocus();
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 3

        RowLayout {
            spacing: 1

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item {
                    id: roomArea
                    anchors.fill: parent
                    anchors.margins: 10

                    Repeater {
                        id: photos
                        model: photoModel
                        Photo {
                            screenName: modelData.screenName
                            clientPlayer: modelData.clientPlayer
                            hp: modelData.hp
                            maxHp: modelData.maxHp
                            headGeneral: modelData.headGeneralName
                            deputyGeneral: modelData.deputyGeneralName
                            phase: modelData.phase
                            seat: modelData.seat
                            chained: modelData.chained
                            dying: modelData.dying
                            alive: modelData.alive
                            drunk: modelData.drunk
                            userRole: modelData.role
                            kingdom : modelData.kingdom

                            onSelectedChanged: roomScene.onPhotoSelected(seat, selected);
                        }
                    }

                    onWidthChanged: arrangePhotos();
                    onHeightChanged: arrangePhotos();

                    InvisibleCardArea {
                        id: drawPile
                        x: parent.width / 2
                        y: parent.height * 0.7
                        isDrawPile: true
                    }

                    TablePile {
                        id: tablePile
                        width: parent.width * 0.6
                        height: 150
                        x: parent.width * 0.2
                        y: parent.height * 0.5
                    }

                    ProgressBar {
                        id: globalProgressBar
                        width: 400
                        height: 15
                        anchors.centerIn: parent
                        visible: false
                    }
                }
            }

            ColumnLayout {
                spacing: 1
                Layout.fillWidth: false
                Layout.preferredWidth: 275

                LogBox {
                    id: logBox
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Connections {
                        target: roomScene
                        function onAddLog(richText) {
                            logBox.append(richText);
                        }
                    }
                }

                ChatBox {
                    id: chatBox
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200

                    Connections {
                        target: roomScene
                        function onAddChatter(chatter) {
                            chatBox.append(chatter);
                        }
                    }
                }
            }
        }

        Dashboard {
            id: dashboard

            onAccepted: {
                closeDialog();
                roomScene.onAccepted();
            }

            onRejected: {
                closeDialog();
                roomScene.onRejected();
            }

            onFinished: {
                closeDialog();
                roomScene.onFinished();
            }

            Connections {
                target: roomScene
                function onDashboardModelChanged() {
                    var model = dashboardModel[0];
                    dashboard.seatNumber = Qt.binding(function(){return model.seat});
                    dashboard.phase = Qt.binding(function(){return model.phase});
                    dashboard.hp = Qt.binding(function(){return model.hp});
                    dashboard.maxHp = Qt.binding(function(){return model.maxHp});
                    dashboard.headGeneralName = Qt.binding(function(){return model.headGeneralName});
                    dashboard.deputyGeneralName = Qt.binding(function(){return model.deputyGeneralName});
                    dashboard.headGeneralKingdom = dashboard.deputyGeneralKingdom = Qt.binding(function(){return model.kingdom});
                    dashboard.chained = Qt.binding(function(){return model.chained;});
                    dashboard.dying = Qt.binding(function(){return model.dying;});
                    dashboard.alive = Qt.binding(function(){return model.alive;});
                    dashboard.drunk = Qt.binding(function(){return model.drunk;});
                    dashboard.headSkills = Qt.binding(function(){return model.headSkills;});
                    dashboard.deputySkills = Qt.binding(function(){return model.deputySkills;});
                    dashboard.userRole = Qt.binding(function(){return model.role;});
                }

                function onSetAcceptEnabled() {
                    dashboard.acceptButton.enabled = enabled;
                }

                function onSetRejectEnabled() {
                    dashboard.rejectButton.enabled = enabled;
                }

                function onSetFinishEnabled() {
                    dashboard.finishButton.enabled = enabled;
                }
            }

            Connections {
                target: dashboard.handcardArea
                function onCardSelected(cardId, selected) {
                    roomScene.onCardSelected(cardId, selected);
                }
            }

            Connections {
                target: dashboard.equipArea
                function onCardSelected(cardId, selected) {
                    roomScene.onCardSelected(cardId, selected);
                }
            }
        }
    }

    Loader {
        id: popupBox
        z: 1000
        onSourceChanged: {
            if (item === null)
                return;
            item.finished.connect(function(){
                source = "";
            });
            item.widthChanged.connect(function(){
                popupBox.moveToCenter();
            });
            item.heightChanged.connect(function(){
                popupBox.moveToCenter();
            });
            moveToCenter();
        }

        function moveToCenter()
        {
            item.x = Math.round((roomArea.width - item.width) / 2);
            item.y = Math.round(roomArea.height * 0.67 - item.height / 2);
        }
    }

    Prompt {
        id: promptBox
        x: Math.round((roomArea.width - width) / 2)
        y: Math.round(roomArea.height * 0.67 - height / 2);
        z: 1000
        visible: false

        onFinished: visible = false;
    }

    ColumnLayout {
        id: buttons
        // visible: !Sanguosha.getServerInfo("DuringGame")
        anchors.verticalCenter: roomScene.verticalCenter
        x: roomScene.width / 2 - width / 2 - logBox.width / 2
        MetroButton {
            id: addRobotButton
            text: qsTr("Add a Robot")
            width: 190
            height: 50
            // visible: Sanguosha.getServerInfo("EnableAI")
            onClicked: {
                roomScene.addRobot(1)
            }
        }
        MetroButton {
            id: returnToStart
            text: qsTr("Return to Start")
            width: 190
            height: 50
            onClicked: {
                roomScene.returnToStart()
            }
        }
    }

    MetroButton {
        text: "托管"
        onClicked: roomScene.trust()
    }

    onAddPlayer: {
        for (let i = 0; i < photoModel.length; i++) {
            if (photoModel[i].clientPlayer === "") {
                photoModel[i].clientPlayer = player.objectName
                photoModel[i] = {
                    screenName: player.screenname,
                    clientPlayer: player.objectName,
                    hp: 0,
                    maxHp: 0,
                    headGeneralName: player.getAvatar(),
                    deputyGeneralName: "",
                    phase: "inactive",
                    seat: i + 2,
                    chained: false,
                    dying: false,
                    alive: true,
                    drunk: false,
                    role: "unknown",
                    kingdom : "qun",
                }

                photos.model = photoModel
                arrangePhotos()

                currentPlayerNum++
                if (currentPlayerNum === playerNum)
                    buttons.visible = false
                else buttons.visible = true

                /*if (!Self.hasFlag("marshalling"))
                    Sanguosha.playSystemAudioEffect("add-player", false);*/

                return;
            }
        }
    }

    onRemovePlayer: {
        let photo = null
        for (var i = 0; i < photoModel.length; i++) {
            if (photoModel[i].clientPlayer === player_name) {
                photoModel[i].clientPlayer = ""
                photoModel[i].headGeneralName = "anjiang"
                photoModel[i].screenName = ""
                photos.model = photoModel
                arrangePhotos()
            }
        }
        currentPlayerNum--
    }

    onReturnToStart: {
        dialogLoader.setSource("")
    }

    onChooseGeneral: {
        popupBox.source = "RoomElement/ChooseGeneralBox.qml";
        var box = popupBox.item;
        // box.choiceNum = num;
        box.choiceNum = 1;
        box.accepted.connect(function(){
            roomScene.onChooseGeneralDone(box.choices[0]);
        });
        for (var i = 0; i < generals.length; i++)
            box.generalList.append({ "name": generals[i] });
        box.updatePosition();
    }

    onUpdateProperty: {
        let player_name = args[0]
        let property_name = args[1]
        if (property_name === "general") property_name = "headGeneralName"
        else if (property_name === "general2") property_name = "deputyGeneralName"
        let value = args[2]
        let setValue = function (target_model, property_name, value) {
            switch (property_name) {
            case "headGeneralName":
                target_model[property_name] = value;
                target_model["kingdom"] = Sanguosha.getGeneralKingdom(value);
                break
            case "maxhp":
                property_name = "maxHp";
            case "seat":
            case "hp":
                target_model[property_name] = parseInt(value);
                break;
            case "chained":
            case "alive":
            case "drunk":
                target_model[property_name] = (value === "true" ? true : false)
                break
            default:
                target_model[property_name] = value;
            }
        }

        if (player_name === Self.objectName || player_name === "MG_SELF") {
            let tmp = dashboardModel
            setValue(tmp[0], property_name, value)
            dashboardModel = tmp
        } else {
            for (let i = 0; i < photoModel.length; i++) {
                if (photoModel[i].clientPlayer === player_name) {
                    setValue(photoModel[i], property_name, value)
                    photos.model = photoModel
                    arrangePhotos()
                    break
                }
            }
        }
    }

    onReceiveLog: {
        logBox.append(log_str)
    }

    onGetCards: {
        for (var i = 0; i < moves.length; i++) {
            var move = moves[i];
            var from = getAreaItem(move.from_place, move.from_player_name);
            var to = getAreaItem(move.to_place, move.to_player_name);
            if (!from || !to || from === to)
                continue;
            //var items = from.remove(move.card_ids);
            //if (to === Player.DrawPile) continue;
            let items = _m_cardsMoveStash[moveId][i]
            if (items.length > 0)
                to.add(items);
            to.updateCardPosition(true);
        }
    }

    onLoseCards: {
        _m_cardsMoveStash[moveId] = [];
        for (var i = 0; i < moves.length; i++) {
            var move = moves[i];
            var from = getAreaItem(move.from_place, move.from_player_name);
            var to = getAreaItem(move.to_place, move.to_player_name);
            if (!from || !to || from === to)
                continue;
            var items = from.remove(move.card_ids);
            //if (items.length > 0)
            //    to.add(items);
            //to.updateCardPosition(true);
            _m_cardsMoveStash[moveId].push(items);
        }
    }

    onSetEmotion: {
        var component = Qt.createComponent("RoomElement/PixmapAnimation.qml");
        if (component.status !== Component.Ready)
            return;

        // var photo = getItemBySeat(seat);
        var photo = getItemByPlayerName(who)
        var animation = component.createObject(roomScene, {source: emotion, x: photo.x, y: photo.y});
        animation.finished.connect(function(){animation.destroy();});
        animation.start();
    }

    onEnableCards: {
        dashboard.equipArea.enableCards(cardIds);
        dashboard.handcardArea.enableCards(cardIds);
    }

    onSetPhotoReady: {
        var i, photo;
        if (ready) {
            for (i = 0; i < photos.count; i++) {
                photo = photos.itemAt(i);
                photo.state = "candidate";
            }
        } else {
            for (i = 0; i < photos.count; i++) {
                photo = photos.itemAt(i);
                photo.state = "normal";
                photo.selectable = photo.selected = false;
            }
        }
    }

    onEnablePhotos: {
        for (var i = 0; i < photos.count; i++) {
            var photo = photos.itemAt(i);
            if (!photo.selected)
                photo.selectable = seats.contains(photo.seat);
        }
        if (!dashboard.selected)
            dashboard.selectable = seats.contains(dashboard.seatNumber);
    }

    onPlayAudio: {
        soundEffect.fileName = "audio/" + path;
        soundEffect.play();
    }

    onShowPrompt: {
        promptBox.text = prompt;
        promptBox.visible = true;
    }

    onHidePrompt: {
        promptBox.visible = false;
    }

    onAskToChooseCards: {
        popupBox.source = "RoomElement/ChooseCardBox.qml";
        popupBox.item.addCards(cards);
        popupBox.moveToCenter();
        popupBox.item.cardSelected.connect(function(cid){
            roomScene.onAmazingGraceTaken(cid);
        });
    }

    onAskToChoosePlayerCard: {
        popupBox.source = "RoomElement/PlayerCardBox.qml";
        popupBox.item.addHandcards(handcards);
        popupBox.item.addEquips(equips);
        popupBox.item.addDelayedTricks(delayedTricks);
        popupBox.moveToCenter();
        popupBox.item.cardSelected.connect(function(cid){
            roomScene.onPlayerCardSelected(cid);
        });
    }

    onShowCard: {
        if (cards.length === 1) {
            var photo = getItemBySeat(fromSeat);
            var items = photo.remove(cards);
            tablePile.add(items);
            tablePile.updateCardPosition(true);
        } else if (cards.length > 1) {
            //@to-do: skills like Gongxin show multiple cards
        }
    }

    onClearPopupBox: popupBox.item.close();

    onShowOptions: {
        popupBox.source = "RoomElement/ChooseOptionBox.qml";
        popupBox.item.options = options;
        popupBox.item.accepted.connect(function(){
            roomScene.onOptionSelected(popupBox.item.result);
        });
    }

    onShowArrangeCardBox: {
        popupBox.source = "RoomElement/ArrangeCardBox.qml";
        popupBox.item.cards = cards;
        popupBox.item.areaCapacities = capacities;
        popupBox.item.areaNames = names;
        popupBox.item.accepted.connect(function(){
            var result = popupBox.item.result;
            var cardIds = new Array(result.length);
            for (var i = 0; i < result.length; i++) {
                cardIds[i] = [];
                for (var j = 0; j < result[i].length; j++)
                    cardIds[i].push(result[i][j].cid);
            }
            roomScene.onArrangeCardDone(cardIds);
        });
        dashboard.acceptButton.enabled = true;
    }

    onShowGameOverBox: {
        popupBox.source = "RoomElement/GameOverBox.qml";
        for (var i = 0; i < winners.length; i++)
            popupBox.item.add(winners[i]);
    }

    onPlayerNumChanged: arrangePhotos();

    function arrangePhotos()
    {
        /*
        Layout:
           col1           col2
        _______________________
        |_2_|______1_______|_0_| row1
        |   |              |   |
        | 4 |    table     | 3 |
        |___|______________|___|
        |      dashboard       |
        ------------------------
        region 5 = 0 + 3, region 6 = 2 + 4, region 7 = 0 + 1 + 2
        */

        var regularSeatIndex = [
            [1],
            [5, 6],
            [5, 1, 6],
            [3, 1, 1, 4],
            [3, 7, 7, 7, 4],
            [5, 5, 1, 1, 6, 6],
            [5, 5, 1, 1, 1, 6, 6],
            [5, 5, 1, 1, 1, 1, 6, 6],
            [3, 3, 7, 7, 7, 7, 7, 4, 4]
        ];
        var seatIndex = regularSeatIndex[playerNum - 2];
        var horizontalBorder = roomArea.height * 0.4;
        var sideWidth = playerNum < 9 ? 0.2 : 0.15;
        var verticalBorders = [roomArea.width * sideWidth, roomArea.width * (1 - sideWidth)];
        var regions = [
            {top: 0, bottom: horizontalBorder, left: verticalBorders[1], right: roomArea.width, players: []},
            {top: 0, bottom: horizontalBorder, left: verticalBorders[0], right: verticalBorders[1], players: []},
            {top: 0, bottom: horizontalBorder, left: 0, right: verticalBorders[0], players: []},
            {top: horizontalBorder, bottom: roomArea.height, left: verticalBorders[1], right: roomArea.width, players: []},
            {top: horizontalBorder, bottom: roomArea.height, left: 0, right: verticalBorders[0], players: []},
            {top: 0, bottom: roomArea.height, left: verticalBorders[1], right: roomArea.width, players: []},
            {top: 0, bottom: roomArea.height, left: 0, right: verticalBorders[0], players: []},
            {top: 0, bottom: horizontalBorder, left: 0, right: roomArea.width, players: []}
        ];

        var roomAreaPadding = 10;
        var item, region, i, subindex, x, y, spacing;

        for (i = 0; i < playerNum - 1; i++)
            regions[seatIndex[i]].players.push(i);

        for (i = 0; i < playerNum - 1; i++) {
            item = photos.itemAt(i);
            if (!item)
                continue;

            region = regions[seatIndex[i]];
            subindex = region.players.indexOf(i);

            //Top Area 1 or 7
            if (seatIndex[i] === 1 || seatIndex[i] === 7) {
                if (playerNum === 6 || playerNum === 10) {
                    spacing = ((region.right - region.left) - region.players.length * item.width) / (region.players.length + 1);
                    x = region.right - (item.width + spacing) * (subindex + 1);
                } else {
                    x = region.right - item.width / 2 - (region.right - region.left) / region.players.length / 2 * (subindex * 2 + 1);
                }
            //Left Area 4 or 6, Right Area 3 or 5
            } else {
                x = (region.left + region.right - item.width) / 2;
            }

            //Top Area 1 or 7
            if (seatIndex[i] === 1 || seatIndex[i] === 7) {
                y = (region.top + region.bottom - item.height) / 2;
            } else {
                spacing = ((region.bottom - region.top) - region.players.length * item.height) / (region.players.length + 1);
                //Right Area 3 or 5
                if (seatIndex[i] === 3 || seatIndex[i] === 5) {
                    y = region.bottom - (spacing + item.height) * (subindex + 1);
                //Left Area 4 or 6
                } else {
                    y = region.top + spacing * (subindex + 1) + item.height * subindex;
                }
            }
            item.x = Math.round(x);
            item.y = Math.round(y);
        }
    }

    onShowIndicatorLine:
    {
        var component = Qt.createComponent("RoomElement/IndicatorLine.qml");
        if (component.status !== Component.Ready)
            return;

        var fromItem = getItemBySeat(from);
        var fromPos = mapFromItem(fromItem, fromItem.width / 2, fromItem.height / 2);

        var end = [];
        for (var i = 0; i < tos.length; i++) {
            if (from === tos[i])
                continue;
            var toItem = getItemBySeat(tos[i]);
            var toPos = mapFromItem(toItem, toItem.width / 2, toItem.height / 2);
            end.push(toPos);
        }

        var color = Engine.kingdomColor[fromItem.userRole];
        var line = component.createObject(roomScene, {start: fromPos, end: end, color: color});
        line.finished.connect(function(){line.destroy();});
        line.running = true;
    }

    function getItemByPlayerName(name)
    {
        if (name === Self.objectName)
            return dashboard;
        for (let i = 0; i < photoModel.length; i++) {
            if (photoModel[i].clientPlayer === name)
                return photos.itemAt(i);
        }
    }

    function getAreaItem(area, name)
    {
        if (area === Player.DrawPile) {
            return drawPile;
        } else if (area === Player.DiscardPile || area === Player.PlaceTable
                   || area === Player.PlaceJudge) {
            return tablePile;
        } else if (area === Player.PlaceWuGu) {
            return popupBox.item;
        }

        var photo = getItemByPlayerName(name);
        if (!photo)
            return null;

        if (area === Player.PlaceHand) {
            return photo.handcardArea;
        } else if (area === Player.PlaceEquip)
            return photo.equipArea;
        else if (area === Player.PlaceDelayedTrick)
            return photo.delayedTrickArea;

        return null;
    }

    //@to-do: hide the latest dialog. We need a better solution
    function closeDialog()
    {
        if (promptBox.visible) {
            promptBox.visible = false;
        } else if (popupBox.item) {
            popupBox.item.close();
        }
    }

    Component.onCompleted: {
        toast.show(qsTr("Sucesessfully entered room."))
        dashboardModel = [{
            seat: 1,
            phase: "start",
            hp: 0,
            maxHp: 0,
            headGeneralName: Self.getAvatar(),
            deputyGeneralName: "",
            chained: false,
            dying: false,
            alive: false,
            drunk: false,
            kingdom: "qun",
            role: "unknown",
            headSkills: [],
            deputySkills: []
        }]

        let player_num = Sanguosha.getPlayerCount(Sanguosha.getServerInfo("GameMode"));

        // create photos
        for (let i = 0; i < player_num - 1; i++) {
            photoModel.push(
                        {
                            screenName: "",
                            clientPlayer: "",
                            hp: 0,
                            maxHp: 0,
                            headGeneralName: "anjiang",
                            deputyGeneralName: "",
                            phase: "inactive",
                            seat: i + 2,
                            chained: false,
                            dying: false,
                            alive: true,
                            drunk: false,
                            role: "unknown",
                            kingdom : "qun",
                        });
        }
        photos.model = photoModel

        playerNum = player_num
        // addRobotButton.visible = Self.owner ? 1 : 0
    }
}
