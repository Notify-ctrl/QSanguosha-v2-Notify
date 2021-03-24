#include "roomscene.h"

RoomScene::RoomScene(QQuickItem *parent) : QQuickItem(parent)
{
    connect(ClientInstance, &Client::line_spoken, this, &RoomScene::addChatter);
    connect(this, &RoomScene::chat, ClientInstance, &Client::speakToServer);

    connect(ClientInstance, &Client::player_added, this, &RoomScene::addPlayer);
    connect(ClientInstance, &Client::player_removed, this, &RoomScene::removePlayer);

    connect(this, &RoomScene::returnToStart, Self, &ClientPlayer::deleteLater);
    connect(this, &RoomScene::returnToStart, ClientInstance, &Client::deleteLater);
    connect(this, &RoomScene::returnToStart, ClientInstance, &Client::disconnectFromHost);

    connect(ClientInstance, &Client::generals_got, this, &RoomScene::chooseGeneral);
    connect(this, &RoomScene::chooseGeneralDone, ClientInstance, &Client::onPlayerChooseGeneral);

    connect(ClientInstance, &Client::property_updated, this, &RoomScene::updateProperty);
}

REGISTER_QMLTYPE("Sanguosha", 1, 0, RoomScene)
