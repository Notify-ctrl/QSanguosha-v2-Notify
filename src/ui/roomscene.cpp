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
}

REGISTER_QMLTYPE("Sanguosha", 1, 0, RoomScene)
