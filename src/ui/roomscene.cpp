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
    connect(ClientInstance, &Client::log_received, this, &RoomScene::receiveLog);

    //connect(ClientInstance, &Client::move_cards_got, this, &RoomScene::moveCards);
    //connect(ClientInstance, &Client::move_cards_lost, this, &RoomScene::moveCards);
    connect(ClientInstance, &Client::move_cards_got, this, &RoomScene::getCards);
    connect(ClientInstance, &Client::move_cards_lost, this, &RoomScene::loseCards);

    connect(this, &RoomScene::addRobot, ClientInstance, &Client::addRobot);
    connect(this, &RoomScene::trust, ClientInstance, &Client::trust);

    connect(ClientInstance, &Client::emotion_set, this, &RoomScene::setEmotion);
    connect(ClientInstance, &Client::animated, this, &RoomScene::doAnimation);
    connect(ClientInstance, &Client::hp_changed, this, &RoomScene::changeHp);

    connect(ClientInstance, &Client::event_received, this, &RoomScene::handleGameEvent);
    connect(ClientInstance, &Client::status_changed, this, &RoomScene::updateStatus);

    connect(ClientInstance, &Client::ag_filled, this, &RoomScene::fillCards);
    connect(ClientInstance, &Client::ag_taken, this, &RoomScene::takeAmazingGrace);
    connect(ClientInstance, &Client::ag_cleared, this, &RoomScene::clearPopupBox);

    connect(ClientInstance, &Client::game_over, this, &RoomScene::showGameOverBox);
}

REGISTER_QMLTYPE("Sanguosha", 1, 0, RoomScene)
