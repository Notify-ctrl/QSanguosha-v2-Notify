#include "roomscene.h"
#include "clientplayer.h"
#include "client.h"

using namespace QSanProtocol;

//RoomScene *RoomSceneInstance = NULL;
RoomScene *RoomSceneInstance;

RoomScene::RoomScene(QQuickItem *parent)
    : QQuickItem(parent)
{
    connect(Self, SIGNAL(general_changed()), dashboard, SLOT(updateAvatar()));
    connect(Self, SIGNAL(general2_changed()), dashboard, SLOT(updateSmallAvatar()));
    connect(Self, SIGNAL(pile_changed(QString)), dashboard, SLOT(updatePile(QString)));

    // add role ComboBox
    connect(Self, SIGNAL(role_changed(QString)), dashboard, SLOT(updateRole(QString)));
    connect(ClientInstance, SIGNAL(player_added(ClientPlayer *)), SLOT(addPlayer(ClientPlayer *)));
    connect(ClientInstance, SIGNAL(player_removed(QString)), SLOT(removePlayer(QString)));
    connect(ClientInstance, SIGNAL(generals_got(QStringList)), this, SLOT(chooseGeneral(QStringList)));
    connect(ClientInstance, SIGNAL(generals_viewed(QString, QStringList)), this, SLOT(viewGenerals(QString, QStringList)));
    connect(ClientInstance, SIGNAL(suits_got(QStringList)), this, SLOT(chooseSuit(QStringList)));
    connect(ClientInstance, SIGNAL(options_got(QString, QStringList)), this, SLOT(chooseOption(QString, QStringList)));
    connect(ClientInstance, SIGNAL(cards_got(const ClientPlayer *, QString, QString, bool, Card::HandlingMethod, QList<int>)),
        this, SLOT(chooseCard(const ClientPlayer *, QString, QString, bool, Card::HandlingMethod, QList<int>)));
    connect(ClientInstance, SIGNAL(roles_got(QString, QStringList)), this, SLOT(chooseRole(QString, QStringList)));
    connect(ClientInstance, SIGNAL(directions_got()), this, SLOT(chooseDirection()));
    connect(ClientInstance, SIGNAL(orders_got(QSanProtocol::Game3v3ChooseOrderCommand)), this, SLOT(chooseOrder(QSanProtocol::Game3v3ChooseOrderCommand)));
    connect(ClientInstance, SIGNAL(kingdoms_got(QStringList)), this, SLOT(chooseKingdom(QStringList)));
    connect(ClientInstance, SIGNAL(seats_arranged(QList<const ClientPlayer *>)), SLOT(arrangeSeats(QList<const ClientPlayer *>)));
    connect(ClientInstance, SIGNAL(status_changed(Client::Status, Client::Status)), this, SLOT(updateStatus(Client::Status, Client::Status)));
    connect(ClientInstance, SIGNAL(avatars_hiden()), this, SLOT(hideAvatars()));
    connect(ClientInstance, SIGNAL(hp_changed(QString, int, DamageStruct::Nature, bool)), SLOT(changeHp(QString, int, DamageStruct::Nature, bool)));
    connect(ClientInstance, SIGNAL(maxhp_changed(QString, int)), SLOT(changeMaxHp(QString, int)));
    connect(ClientInstance, SIGNAL(pile_reset()), this, SLOT(resetPiles()));
    connect(ClientInstance, SIGNAL(player_killed(QString)), this, SLOT(killPlayer(QString)));
    connect(ClientInstance, SIGNAL(player_revived(QString)), this, SLOT(revivePlayer(QString)));
    connect(ClientInstance, SIGNAL(card_shown(QString, int)), this, SLOT(showCard(QString, int)));
    connect(ClientInstance, SIGNAL(gongxin(QList<int>, bool, QList<int>)), this, SLOT(doGongxin(QList<int>, bool, QList<int>)));
    connect(ClientInstance, SIGNAL(focus_moved(QStringList, QSanProtocol::Countdown)), this, SLOT(moveFocus(QStringList, QSanProtocol::Countdown)));
    connect(ClientInstance, SIGNAL(emotion_set(QString, QString)), this, SLOT(setEmotion(QString, QString)));
    connect(ClientInstance, SIGNAL(skill_invoked(QString, QString)), this, SLOT(showSkillInvocation(QString, QString)));
    connect(ClientInstance, SIGNAL(skill_acquired(const ClientPlayer *, QString)), this, SLOT(acquireSkill(const ClientPlayer *, QString)));
    connect(ClientInstance, SIGNAL(animated(int, QStringList)), this, SLOT(doAnimation(int, QStringList)));
    connect(ClientInstance, SIGNAL(role_state_changed(QString)), this, SLOT(updateRoles(QString)));
    connect(ClientInstance, SIGNAL(event_received(const QVariant)), this, SLOT(handleGameEvent(const QVariant)));

    connect(ClientInstance, SIGNAL(game_started()), this, SLOT(onGameStart()));
    connect(ClientInstance, SIGNAL(game_over()), this, SLOT(onGameOver()));
    connect(ClientInstance, SIGNAL(standoff()), this, SLOT(onStandoff()));

    connect(ClientInstance, SIGNAL(move_cards_lost(int, QList<CardsMoveStruct>)), this, SLOT(loseCards(int, QList<CardsMoveStruct>)));
    connect(ClientInstance, SIGNAL(move_cards_got(int, QList<CardsMoveStruct>)), this, SLOT(getCards(int, QList<CardsMoveStruct>)));

    connect(ClientInstance, SIGNAL(nullification_asked(bool)), dashboard, SLOT(controlNullificationButton(bool)));

    connect(ClientInstance, SIGNAL(assign_asked()), this, SLOT(startAssign()));
    connect(ClientInstance, SIGNAL(start_in_xs()), this, SLOT(startInXs()));

    connect(ClientInstance, &Client::skill_updated, this, &RoomScene::updateSkill);
    connect(ClientInstance, SIGNAL(guanxing(QList<int>, bool)), guanxing_box, SLOT(doGuanxing(QList<int>, bool)));
    connect(card_container, SIGNAL(item_chosen(int)), ClientInstance, SLOT(onPlayerChooseAG(int)));
    connect(card_container, SIGNAL(item_gongxined(int)), ClientInstance, SLOT(onPlayerReplyGongxin(int)));
    connect(ClientInstance, SIGNAL(ag_filled(QList<int>, QList<int>)), this, SLOT(fillCards(QList<int>, QList<int>)));
    connect(ClientInstance, SIGNAL(ag_taken(ClientPlayer *, int, bool)), this, SLOT(takeAmazingGrace(ClientPlayer *, int, bool)));
    connect(ClientInstance, SIGNAL(ag_cleared()), card_container, SLOT(clear()));
    connect(ClientInstance, SIGNAL(skill_attached(QString)), this, SLOT(attachSkill(QString)));
    connect(ClientInstance, SIGNAL(skill_detached(QString)), this, SLOT(detachSkill(QString)));
    connect(ClientInstance, SIGNAL(line_spoken(QString)), chat_box, SLOT(append(QString)));
    connect(ClientInstance, SIGNAL(player_speak(const QString &, const QString &)),
        this, SLOT(showBubbleChatBox(const QString &, const QString &)));
    connect(chat_widget, SIGNAL(return_button_click()), this, SLOT(speak()));
    connect(chat_widget, SIGNAL(chat_widget_msg(QString)), this, SLOT(appendChatEdit(QString)));
    connect(ClientInstance, SIGNAL(log_received(QStringList)), log_box, SLOT(appendLog(QStringList)));
    connect(return_to_main_menu, SIGNAL(clicked()), this, SIGNAL(return_to_start()));
}

bool RoomScene::checkTargetFeasibility()
{
    const Card *card = nullptr;
    const ClientPlayer *self = ClientInstance->selfPlayer();

    if (m_viewAsSkill) {
        const ClientPlayer *self = ClientInstance->selfPlayer();
        QList<Card *> copy;
        foreach (const Card *card, m_selectedCard)
            copy << card->clone();
        Card *result = m_viewAsSkill->viewAs(copy, self);
        foreach (Card *card, copy)
            delete card;
        card = result;
    } else if (m_selectedCard.length() == 1) {
        card = m_selectedCard.first();
    }

    if (card) {
        setPhotoReady(true);

        bool acceptable = card->isTargetFixed() || card->targetFeasible(m_selectedPlayer, self);
        if (acceptable && !m_assignedTargets.isEmpty()) {
            foreach (const Player *target, m_assignedTargets) {
                if (!m_selectedPlayer.contains(target)) {
                    acceptable = false;
                    break;
                }
            }
        }

        if (m_selectedPlayer.isEmpty() && card->canRecast())
            acceptable = true;
        setAcceptEnabled(acceptable);

        QVariantList seats;
        if (!card->isTargetFixed()) {
            const ClientPlayer *self = ClientInstance->selfPlayer();
            QList<const ClientPlayer *> players = ClientInstance->players();
            foreach (const ClientPlayer *player, players) {
                if (card->targetFilter(m_selectedPlayer, player, self))
                    seats << player->seat();
            }
        }
        enablePhotos(seats);

        if (card->isVirtual())
            delete card;
        return acceptable;
    } else {
        setPhotoReady(false);
        setAcceptEnabled(false);
        return false;
    }
}

void RoomScene::resetDashboard()
{
    m_respondingState = InactiveState;

    m_selectedCard.clear();
    enableCards(QVariantList());
    setPhotoReady(false);
    m_selectedPlayer.clear();
    enablePhotos(QVariantList());

    setAcceptEnabled(false);
    setRejectEnabled(false);
    setFinishEnabled(false);

    hidePrompt();

    m_viewAsSkill = nullptr;
    const ClientPlayer *self = ClientInstance->selfPlayer();
    QList<const Skill *> skills = self->skills();
    foreach (const Skill *skill, skills) {
        ClientSkill *model = self->getSkill(skill);
        model->setEnabled(false);
    }
}

void RoomScene::enableCards(const QString &pattern)
{
    QVariantList cardIds;
    const ClientPlayer *self = ClientInstance->selfPlayer();
    QList<Card *> cards = self->handcardArea()->cards();
    if (pattern.isEmpty()) {
        foreach (Card *card, cards) {
            if (card->isAvailable(self))
                cardIds << card->id();
        }
    } else {
        cards << self->equipArea()->cards();

        CardPattern exp(pattern);
        foreach (const Card *card, cards) {
            if (exp.match(self, card))
                cardIds << card->id();
        }
    }
    enableCards(cardIds);

    QList<const Skill *> skills = self->skills();
    foreach (const Skill *skill, skills) {
        if (skill->type() != Skill::ViewAsType)
            continue;
        const ViewAsSkill *viewAsSkill = static_cast<const ViewAsSkill *>(skill);
        ClientSkill *model = self->getSkill(skill);
        model->setEnabled(viewAsSkill->isAvailable(self, pattern));
    }
}

void RoomScene::enableCards(const QList<const Card *> &cards)
{
    QVariantList cardIds;
    foreach (const Card *card, cards)
        cardIds << card->getId();
    enableCards(cardIds);
}

void RoomScene::onSeatArranged()
{
    QList<const ClientPlayer *> players = ClientInstance->getPlayers();
    std::sort(players.begin(), players.end(), [](const ClientPlayer *p1, const ClientPlayer *p2){
        return p1->getSeat() < p2->getSeat();
    });

    const ClientPlayer *self = Self;
    int selfIndex = players.indexOf(self);
    players = players.mid(selfIndex + 1) + players.mid(0, selfIndex);
    setProperty("dashboardModel", qConvertToModel(self));
    setProperty("photoModel", qConvertToModel(players));
    setProperty("playerNum", players.length() + 1);
}

void RoomScene::onChooseGeneralRequested(const QList<const General *> &candidates, int num)
{
    QVariantList generals;
    foreach (const General *general, candidates) {
        QVariantMap generalData;
        generalData["gid"] = Sanguosha->getAllGenerals().indexOf(general);
        generalData["name"] = general->objectName();
        generalData["kingdom"] = general->getKingdom();
        generals << generalData;
    }
    chooseGeneral(generals, num);
}

void RoomScene::onChooseGeneralFinished(const QVariantList &choices)
{
    QVariantList data;
    foreach (const QVariant &choice, choices)
        data << choice.toUInt();
    ClientInstance->replyToServer(S_COMMAND_CHOOSE_GENERAL, data);
}

void RoomScene::onUsingCard(const QString &pattern, const QList<const Player *> &assignedTargets)
{
    m_respondingState = UsingCardState;
    m_assignedTargets = assignedTargets;
    m_respondingPattern = pattern;

    enableCards(pattern);

    setAcceptEnabled(false);
    setRejectEnabled(false);
    setFinishEnabled(true);
}

void RoomScene::onCardSelected(uint cardId, bool selected)
{
    if (selected) {
        const Card *card = ClientInstance->getCard(cardId);
        if (card)
            m_selectedCard << card;
    } else {
        for (int i = 0, max = m_selectedCard.length(); i < max; i++) {
            const Card *card = m_selectedCard.at(i);
            if (card->getId() == cardId) {
                m_selectedCard.removeAt(i);
                break;
            }
        }
    }

    switch (m_respondingState) {
    case UsingCardState:{
        if (m_selectedCard.isEmpty()) {
            if (m_viewAsSkill)
                onSkillActivated(Sanguosha->getSkillNames().indexOf(m_viewAsSkill->objectName()), true);
            else
                onUsingCard(m_respondingPattern);
        } else {
            if (m_viewAsSkill) {
                QList<const Card *> cards = Self->getHandcards();
                cards << Self->getEquips();

                QVariantList enabled;
                foreach (const Card *card, m_selectedCard)
                    enabled << card->getId();

                foreach (const Card *card, cards) {
                    if (m_selectedCard.contains(card))
                        continue;
                    if (m_viewAsSkill->viewFilter(m_selectedCard, card))
                        enabled << card->getId();
                }
                enableCards(enabled);
            } else {
                const Card *card = m_selectedCard.first();
                uint id = card->getId();
                enableCards(QVariantList() << id);
            }
            m_selectedPlayer.clear();
            checkTargetFeasibility();
        }
        break;
    }
    case RespondingCardState:{
        int selectedNum = m_selectedCard.length();
        setAcceptEnabled(m_minRespondingCardNum <= selectedNum && selectedNum <= m_maxRespondingCardNum);
        if (selectedNum >= m_maxRespondingCardNum) {
            enableCards(m_selectedCard);
        } else {
            enableCards(m_respondingPattern);
        }
        break;
    }
    default:;
    }
}

void RoomScene::onPhotoSelected(int seat, bool selected)
{
    QList<const ClientPlayer *> players = ClientInstance->getPlayers();
    foreach (const ClientPlayer *player, players) {
        if (player->getSeat() == seat) {
            if (selected)
                m_selectedPlayer.append(player);
            else
                m_selectedPlayer.removeOne(player);
            break;
        }
    }

    checkTargetFeasibility();
}

void RoomScene::onAccepted()
{
    switch (m_respondingState) {
    case UsingCardState:{
        ClientInstance->act(m_selectedCard, m_selectedPlayer, m_viewAsSkill);
        break;
    }
    case RespondingCardState:{
        ClientInstance->respondCard(m_selectedCard, m_viewAsSkill);
        break;
    }
    default:;
    }

    resetDashboard();
}

void RoomScene::onRejected()
{
    if (m_respondingState == RespondingCardState)
        ClientInstance->replyToServer(S_COMMAND_ASK_FOR_CARD);

    resetDashboard();
}

void RoomScene::onFinished()
{
    if (m_respondingState == UsingCardState)
        ClientInstance->replyToServer(S_COMMAND_ACT);

    resetDashboard();
}

void RoomScene::onAmazingGraceTaken(uint cid)
{
    ClientInstance->replyToServer(S_COMMAND_TAKE_AMAZING_GRACE, cid);
}

void RoomScene::onPlayerCardSelected(uint cid)
{
    ClientInstance->replyToServer(S_COMMAND_CHOOSE_PLAYER_CARD, cid);
}

void RoomScene::onSkillActivated(uint skillId, bool activated)
{
    const Skill *originalSkill = Sanguosha->getSkill(Sanguosha->getSkillNames().at(skillI));
    if (!qobject_cast<ViewAsSkill *>(originalSkill))
        return;

    if (activated) {
        const ViewAsSkill *skill = static_cast<const ViewAsSkill *>(originalSkill);
        m_viewAsSkill = skill;

        QList<const Card *> cards = Self->getHandcards() + Self->getEquips();
        QVariantList enabled;
        foreach (const Card *card, cards) {
            if (skill->viewFilter(m_selectedCard, card))
                enabled << card->getId();
        }
        enableCards(enabled);

        if (checkTargetFeasibility()) {
            onAccepted();
            onSkillActivated(skillId, false);
        }
    } else {
        m_viewAsSkill = nullptr;

        if (m_respondingState == UsingCardState)
            onUsingCard(m_respondingPattern);
        else if (m_respondingState == RespondingCardState)
            enableCards(m_respondingPattern);
        else if (m_respondingState == InactiveState)
            enableCards(QVariantList());
    }
}

void RoomScene::onOptionSelected(int selected)
{
    ClientInstance->replyToServer(S_COMMAND_TRIGGER_ORDER, selected);
}

void RoomScene::onArrangeCardDone(const QVariantList &results)
{
    QVariantList data;
    foreach (const QVariant &result, results) {
        QVariantList row;
        const QVariantList resultList = result.toList();
        foreach (const QVariant &cid, resultList)
            row << cid.toUInt();
        data << QVariant(row);
    }

    ClientInstance->replyToServer(S_COMMAND_ARRANGE_CARD, data);
}

void RoomScene::onDamageDone(const ClientPlayer *, const ClientPlayer *to, DataValue::DamageValue::Nature, int damage)
{
    if (damage <= 0 || to == nullptr)
        return;

    int seat = to->getSeat();
    startEmotion("damage", seat);

    playAudio(QString("system/injure%1.ogg").arg(qMin(damage, 3)));
}

void RoomScene::onRecoverDone(const ClientPlayer *, const ClientPlayer *, int)
{
    playAudio("card/common/peach.ogg");
}

void RoomScene::onCardUsed(const QVariantMap &, const ClientPlayer *from, const QList<const ClientPlayer *> &tos)
{
    QVariantList toSeats;
    foreach (const ClientPlayer *to, tos)
        toSeats << to->getSeat();
    showIndicatorLine(from->getSeat(), toSeats);
}

void RoomScene::onCardAsked(const QString &pattern)
{
    m_respondingState = RespondingCardState;
    m_respondingPattern = pattern;
    m_minRespondingCardNum = m_maxRespondingCardNum = 1;
    m_respondingOptional = true;
    enableCards(pattern);
    setRejectEnabled(true);
}

void RoomScene::onCardsAsked(const QString &pattern, int minNum, int maxNum, bool optional)
{
    m_respondingState = RespondingCardState;
    m_respondingPattern = pattern;
    m_minRespondingCardNum = minNum;
    m_maxRespondingCardNum = maxNum;
    m_respondingOptional = optional;
    enableCards(pattern);
    setRejectEnabled(optional);
}
/*
void RoomScene::onAmazingGraceStarted()
{
    const CardArea *wugu = ClientInstance->wugu();
    QList<Card *> cards = wugu->cards();
    QVariantList cardList;
    foreach (Card *card, cards) {
        QVariantMap data = convertToMap(card);
        data["selectable"] = true;
        cardList << data;
    }
    askToChooseCards(cardList);
}
*/
void RoomScene::onChoosePlayerCardRequested(const QList<Card *> &handcards, const QList<Card *> &equips, const QList<Card *> &delayedTricks)
{
    QVariantList h;
    foreach (Card *card, handcards)
        h << convertToMap(card);
    QVariantList e;
    foreach (Card *card, equips)
        e << convertToMap(card);
    QVariantList j;
    foreach (Card *card, delayedTricks)
        j << convertToMap(card);
    askToChoosePlayerCard(h, e, j);
}

void RoomScene::onCardShown(const ClientPlayer *from, const QList<const Card *> &cards)
{
    QVariantList data;
    foreach (const Card *card, cards)
        data << convertToMap(card);
    showCard(from->getSeat(), data);
}

void RoomScene::onArrangeCardRequested(const QList<Card *> &cards, const QList<int> &capacities, const QStringList &areaNames)
{
    QVariantList cardData;
    foreach (const Card *card, cards)
        cardData << convertToMap(card);

    QVariantList capacityData;
    foreach (int capacity, capacities)
        capacityData << capacity;

    showArrangeCardBox(cardData, capacityData, areaNames);
}

void RoomScene::onGameOver(const QList<const ClientPlayer *> &winners)
{
    QVariantList winnerList;
    foreach (const ClientPlayer *winner, winners) {
        QVariantMap info;
        info["role"] = winner->getRole();
        info["general"] = winner->getGeneralName();
        info["userAvatar"] = winner->getAvatarGeneral();
        info["userName"] = winner->screenName();
        winnerList << info;
    }
    showGameOverBox(winnerList);
}

QVariantMap RoomScene::convertToMap(const Card *card) const
{
    QVariantMap data;
    if (card) {
        data["cid"] = card->getId();
        data["name"] = card->objectName();
        data["suit"] = card->getSuitString();
        data["number"] = card->getNumber();
    } else {
        data["cid"] = 0;
        data["name"] = "hegback";
        data["number"] = 0;
        data["suit"] = "";
    }
    return data;
}
