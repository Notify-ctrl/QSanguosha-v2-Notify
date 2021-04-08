#include "roomscene.h"
#include "engine.h"

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

QString RoomScene::_translateMovement(const CardsMoveStruct &move)
{
    CardMoveReason reason = move.reason;
    if (reason.m_reason == CardMoveReason::S_REASON_UNKNOWN) return QString();
    // ============================================
    if (move.from && move.card_ids.length() == 1 && move.to_place == Player::DrawPile
        && move.from->property("zongxuan_move").toString() == QString::number(move.card_ids.first()))
        reason = CardMoveReason(CardMoveReason::S_REASON_PUT, move.from_player_name, QString(), "zongxuan", QString());
    // ============================================
    ClientPlayer *src = ClientInstance->getPlayer(reason.m_playerId);
    ClientPlayer *dst = ClientInstance->getPlayer(reason.m_targetId);
    QString playerName, targetName;

    if (src != NULL)
        playerName = Sanguosha->translate(src->getGeneralName());
    else if (reason.m_playerId == Self->objectName())
        playerName = QString("%1(%2)").arg(Sanguosha->translate(Self->getGeneralName())).arg(Sanguosha->translate("yourself"));

    if (dst != NULL)
        targetName = Sanguosha->translate("use upon").append(Sanguosha->translate(dst->getGeneralName()));
    else if (reason.m_targetId == Self->objectName())
        targetName = QString("%1%2(%3)").arg(Sanguosha->translate("use upon"))
        .arg(Sanguosha->translate(Self->getGeneralName()))
        .arg(Sanguosha->translate("yourself"));

    QString result(playerName + targetName);
    result.append(Sanguosha->translate(reason.m_eventName));
    result.append(Sanguosha->translate(reason.m_skillName));
    if ((reason.m_reason & CardMoveReason::S_MASK_BASIC_REASON) == CardMoveReason::S_REASON_USE && reason.m_skillName.isEmpty()) {
        result.append(Sanguosha->translate("use"));
    } else if ((reason.m_reason & CardMoveReason::S_MASK_BASIC_REASON) == CardMoveReason::S_REASON_RESPONSE) {
        if (reason.m_reason == CardMoveReason::S_REASON_RETRIAL)
            result.append(Sanguosha->translate("retrial"));
        else if (reason.m_skillName.isEmpty())
            result.append(Sanguosha->translate("response"));
    } else if ((reason.m_reason & CardMoveReason::S_MASK_BASIC_REASON) == CardMoveReason::S_REASON_DISCARD) {
        if (reason.m_reason == CardMoveReason::S_REASON_RULEDISCARD)
            result.append(Sanguosha->translate("discard"));
        else if (reason.m_reason == CardMoveReason::S_REASON_THROW)
            result.append(Sanguosha->translate("throw"));
        else if (reason.m_reason == CardMoveReason::S_REASON_CHANGE_EQUIP)
            result.append(Sanguosha->translate("change equip"));
        else if (reason.m_reason == CardMoveReason::S_REASON_DISMANTLE)
            result.append(Sanguosha->translate("throw"));
    } else if (reason.m_reason == CardMoveReason::S_REASON_RECAST) {
        result.append(Sanguosha->translate("recast"));
    } else if (reason.m_reason == CardMoveReason::S_REASON_PINDIAN) {
        result.append(Sanguosha->translate("pindian"));
    } else if ((reason.m_reason & CardMoveReason::S_MASK_BASIC_REASON) == CardMoveReason::S_REASON_SHOW) {
        if (reason.m_reason == CardMoveReason::S_REASON_JUDGE)
            result.append(Sanguosha->translate("judge"));
        else if (reason.m_reason == CardMoveReason::S_REASON_TURNOVER)
            result.append(Sanguosha->translate("turnover"));
        else if (reason.m_reason == CardMoveReason::S_REASON_DEMONSTRATE)
            result.append(Sanguosha->translate("show"));
        else if (reason.m_reason == CardMoveReason::S_REASON_PREVIEW)
            result.append(Sanguosha->translate("preview"));
    } else if ((reason.m_reason & CardMoveReason::S_MASK_BASIC_REASON) == CardMoveReason::S_REASON_PUT) {
        if (reason.m_reason == CardMoveReason::S_REASON_PUT) {
            result.append(Sanguosha->translate("put"));
            if (move.to_place == Player::DiscardPile)
                result.append(Sanguosha->translate("discardPile"));
            else if (move.to_place == Player::DrawPile)
                result.append(Sanguosha->translate("drawPileTop"));
        } else if (reason.m_reason == CardMoveReason::S_REASON_NATURAL_ENTER) {
            result.append(Sanguosha->translate("enter"));
            if (move.to_place == Player::DiscardPile)
                result.append(Sanguosha->translate("discardPile"));
            else if (move.to_place == Player::DrawPile)
                result.append(Sanguosha->translate("drawPileTop"));
        } else if (reason.m_reason == CardMoveReason::S_REASON_JUDGEDONE) {
            result.append(Sanguosha->translate("judgedone"));
        } else if (reason.m_reason == CardMoveReason::S_REASON_REMOVE_FROM_PILE) {
            result.append(Sanguosha->translate("backinto"));
        }
    }
    return result;
}
