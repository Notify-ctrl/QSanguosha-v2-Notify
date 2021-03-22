#include "gamerule.h"
#include "serverplayer.h"
#include "room.h"
#include "standard.h"
#include "maneuvering.h"
#include "engine.h"
#include "settings.h"
#include "json.h"
#include "roomthread.h"

GameRule::GameRule(QObject *)
    : TriggerSkill("game_rule")
{
    //@todo: this setParent is illegitimate in QT and is equivalent to calling
    // setParent(NULL). So taking it off at the moment until we figure out
    // a way to do it.
    //setParent(parent);

    events << GameStart << TurnStart
        << EventPhaseProceeding << EventPhaseEnd << EventPhaseChanging
        << PreCardUsed << CardUsed << CardFinished << CardEffected
        << HpChanged
        << EventLoseSkill << EventAcquireSkill
        << AskForPeaches << AskForPeachesDone << BuryVictim << GameOverJudge
        << SlashHit << SlashEffected << SlashProceed
        << ConfirmDamage << DamageDone << DamageComplete
        << StartJudge << FinishRetrial << FinishJudge
        << ChoiceMade;
}

bool GameRule::triggerable(const ServerPlayer *) const
{
    return true;
}

int GameRule::getPriority(TriggerEvent) const
{
    return 0;
}

void GameRule::onPhaseProceed(ServerPlayer *player) const
{
    Room *room = player->getRoom();
    switch (player->getPhase()) {
    case Player::PhaseNone: {
        Q_ASSERT(false);
    }
    case Player::RoundStart:{
        break;
    }
    case Player::Start: {
        break;
    }
    case Player::Judge: {
        QList<const Card *> tricks = player->getJudgingArea();
        while (!tricks.isEmpty() && player->isAlive()) {
            const Card *trick = tricks.takeLast();
            bool on_effect = room->cardEffect(trick, NULL, player);
            if (!on_effect)
                trick->onNullified(player);
        }
        break;
    }
    case Player::Draw: {
        int num = 2;
        if (player->hasFlag("Global_FirstRound")) {
            room->setPlayerFlag(player, "-Global_FirstRound");
            if (room->getMode() == "02_1v1") num--;
        }

        QVariant data = num;
        room->getThread()->trigger(DrawNCards, room, player, data);
        int n = data.toInt();
        if (n > 0)
            player->drawCards(n, "draw_phase");
        QVariant _n = n;
        room->getThread()->trigger(AfterDrawNCards, room, player, _n);
        break;
    }
    case Player::Play: {
        while (player->isAlive()) {
            CardUseStruct card_use;
            room->activate(player, card_use);
            if (card_use.card != NULL)
                room->useCard(card_use, true);
            else
                break;
        }
        break;
    }
    case Player::Discard: {
        int handcard_num = player->getHandcardNum();
        if (player->hasSkill("olyuhua")) {
            room->sendCompulsoryTriggerLog(player, "olyuhua", true);
            int num = 0;
            foreach (const Card *card, player->getHandcards()) {
                if (!card->isKindOf("BasicCard"))
                    continue;
                num++;
            }
            if (num != handcard_num) {
                room->broadcastSkillInvoke("olyuhua");
                handcard_num = num;
            }
            LogMessage msg;
            msg.type = "#olyuhua-effect";
            msg.from = player;
            msg.arg = QString::number(num);
            room->sendLog(msg);
        }
        int discard_num = handcard_num - player->getMaxCards();
        if (discard_num > 0)
            room->askForDiscard(player, "gamerule", discard_num, discard_num);
        break;
    }
    case Player::Finish: {
        break;
    }
    case Player::NotActive:{
        break;
    }
    }
}

bool GameRule::trigger(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data) const
{
    if (room->getTag("SkipGameRule").toBool()) {
        room->removeTag("SkipGameRule");
        return false;
    }

    // Handle global events
    if (player == NULL) {
        if (triggerEvent == GameStart) {
            foreach (ServerPlayer *player, room->getPlayers()) {
                if (player->getGeneral()->getKingdom() == "god" && player->getGeneralName() != "anjiang"
                    && !player->getGeneralName().startsWith("boss_"))
                    room->setPlayerProperty(player, "kingdom", room->askForKingdom(player));
                foreach (const Skill *skill, player->getVisibleSkillList()) {
                    if (skill->getFrequency() == Skill::Limited && !skill->getLimitMark().isEmpty()
                        && (!skill->isLordSkill() || player->hasLordSkill(skill->objectName())))
                        room->setPlayerMark(player, skill->getLimitMark(), 1);
                }
            }
            room->setTag("FirstRound", true);
            bool kof_mode = room->getMode() == "02_1v1" && Config.value("1v1/Rule", "2013").toString() != "Classical";
            QList<int> n_list;
            foreach (ServerPlayer *p, room->getPlayers()) {
                int n = kof_mode ? p->getMaxHp() : 4;
                QVariant data = n;
                room->getThread()->trigger(DrawInitialCards, room, p, data);
                n_list << data.toInt();
            }
            room->drawCards(room->getPlayers(), n_list, QString());
            if (Config.EnableLuckCard)
                room->askForLuckCard();
            int i = 0;
            foreach (ServerPlayer *p, room->getPlayers()) {
                QVariant _nlistati = n_list.at(i);
                room->getThread()->trigger(AfterDrawInitialCards, room, p, _nlistati);
                i++;
            }
        }
        return false;
    }

    switch (triggerEvent) {
    case TurnStart: {
        player = room->getCurrent();
        if (room->getTag("FirstRound").toBool()) {
            room->setTag("FirstRound", false);
            room->setPlayerFlag(player, "Global_FirstRound");
        }

        LogMessage log;
        log.type = "$AppendSeparator";
        room->sendLog(log);
        room->addPlayerMark(player, "Global_TurnCount");
        room->setPlayerMark(player, "damage_point_round", 0);
        if (!player->faceUp()) {
            room->setPlayerFlag(player, "-Global_FirstRound");
            player->turnOver();
        } else if (player->isAlive())
            player->play();

        break;
    }
    case EventPhaseProceeding: {
        onPhaseProceed(player);
        break;
    }
    case EventPhaseEnd: {
        if (player->getPhase() == Player::Play)
            room->addPlayerHistory(player, ".");
        break;
    }
    case EventPhaseChanging: {
        PhaseChangeStruct change = data.value<PhaseChangeStruct>();
        if (change.to == Player::NotActive) {
            foreach (ServerPlayer *p, room->getAllPlayers()) {
                if (p->getMark("drank") > 0) {
                    LogMessage log;
                    log.type = "#UnsetDrankEndOfTurn";
                    log.from = player;
                    log.to << p;
                    room->sendLog(log);

                    room->setPlayerMark(p, "drank", 0);
                }
            }
            room->setPlayerFlag(player, ".");
            room->clearPlayerCardLimitation(player, true);
        } else if (change.to == Player::Play) {
            room->setPlayerMark(player, "damage_point_play_phase", 0);
            room->addPlayerHistory(player, ".");
        }
        break;
    }
    case PreCardUsed: {
        if (data.canConvert<CardUseStruct>()) {
            CardUseStruct card_use = data.value<CardUseStruct>();
            if (card_use.from->hasFlag("Global_ForbidSurrender")) {
                card_use.from->setFlags("-Global_ForbidSurrender");
                room->doNotify(card_use.from, QSanProtocol::S_COMMAND_ENABLE_SURRENDER, QVariant(true));
            }

            card_use.from->broadcastSkillInvoke(card_use.card);
            if (!card_use.card->getSkillName().isNull() && card_use.card->getSkillName(true) == card_use.card->getSkillName(false)
                && card_use.m_isOwnerUse && card_use.from->hasSkill(card_use.card->getSkillName()))
                room->notifySkillInvoked(card_use.from, card_use.card->getSkillName());
        }
        break;
    }
    case CardUsed: {
        if (data.canConvert<CardUseStruct>()) {
            CardUseStruct card_use = data.value<CardUseStruct>();
            RoomThread *thread = room->getThread();

            if (card_use.card->hasPreAction())
                card_use.card->doPreAction(room, card_use);

            if (card_use.from && !card_use.to.isEmpty()) {
                thread->trigger(TargetSpecifying, room, card_use.from, data);
                CardUseStruct card_use = data.value<CardUseStruct>();
                QList<ServerPlayer *> targets = card_use.to;
                foreach (ServerPlayer *to, card_use.to) {
                    if (targets.contains(to)) {
                        thread->trigger(TargetConfirming, room, to, data);
                        CardUseStruct new_use = data.value<CardUseStruct>();
                        targets = new_use.to;
                    }
                }
            }
            card_use = data.value<CardUseStruct>();

            try {
                QVariantList jink_list_backup;
                if (card_use.card->isKindOf("Slash")) {
                    jink_list_backup = card_use.from->tag["Jink_" + card_use.card->toString()].toList();
                    QVariantList jink_list;
                    for (int i = 0; i < card_use.to.length(); i++)
                        jink_list.append(QVariant(1));
                    card_use.from->tag["Jink_" + card_use.card->toString()] = QVariant::fromValue(jink_list);
                }
                if (card_use.from && !card_use.to.isEmpty()) {
                    thread->trigger(TargetSpecified, room, card_use.from, data);
                    foreach(ServerPlayer *p, room->getAllPlayers())
                        thread->trigger(TargetConfirmed, room, p, data);
                }
                card_use = data.value<CardUseStruct>();
                room->setTag("CardUseNullifiedList", QVariant::fromValue(card_use.nullified_list));
                card_use.card->use(room, card_use.from, card_use.to);
                if (!jink_list_backup.isEmpty())
                    card_use.from->tag["Jink_" + card_use.card->toString()] = QVariant::fromValue(jink_list_backup);
            }
            catch (TriggerEvent triggerEvent) {
                if (triggerEvent == TurnBroken || triggerEvent == StageChange)
                    card_use.from->tag.remove("Jink_" + card_use.card->toString());
                throw triggerEvent;
            }
        }

        break;
    }
    case CardFinished: {
        CardUseStruct use = data.value<CardUseStruct>();
        room->clearCardFlag(use.card);

        if (use.card->isKindOf("AOE") || use.card->isKindOf("GlobalEffect")) {
            foreach(ServerPlayer *p, room->getAlivePlayers())
                room->doNotify(p, QSanProtocol::S_COMMAND_NULLIFICATION_ASKED, QVariant("."));
        }
        if (use.card->isKindOf("Slash"))
            use.from->tag.remove("Jink_" + use.card->toString());

        break;
    }
    case EventAcquireSkill:
    case EventLoseSkill: {
        QString skill_name = data.toString();
        const Skill *skill = Sanguosha->getSkill(skill_name);
        bool refilter = skill->inherits("FilterSkill");

        if (refilter)
            room->filterCards(player, player->getCards("he"), triggerEvent == EventLoseSkill);

        break;
    }
    case HpChanged: {
        if (player->getHp() > 0)
            break;
        if (data.isNull() || data.canConvert<RecoverStruct>())
            break;
        if (data.canConvert<DamageStruct>()) {
            DamageStruct damage = data.value<DamageStruct>();
            room->enterDying(player, &damage);
        } else {
            room->enterDying(player, NULL);
        }

        break;
    }
    case AskForPeaches: {
        DyingStruct dying = data.value<DyingStruct>();
        const Card *peach = NULL;

        while (dying.who->getHp() <= 0) {
            peach = NULL;

            // coupling Wansha here to deal with complicated rule problems
            ServerPlayer *current = room->getCurrent();
            if (current && current->isAlive() && current->getPhase() != Player::NotActive && current->hasSkill("wansha")) {
                if (player != current && player != dying.who) {
                    player->setFlags("wansha");
                    room->addPlayerMark(player, "Global_PreventPeach");
                }
            }

            if (dying.who->isAlive())
                peach = room->askForSinglePeach(player, dying.who);

            if (player->hasFlag("wansha") && player->getMark("Global_PreventPeach") > 0) {
                player->setFlags("-wansha");
                room->removePlayerMark(player, "Global_PreventPeach");
            }

            if (peach == NULL)
                break;
            room->useCard(CardUseStruct(peach, player, dying.who));
        }
        break;
    }
    case AskForPeachesDone: {
        if (player->getHp() <= 0 && player->isAlive()) {
            DyingStruct dying = data.value<DyingStruct>();
            room->killPlayer(player, dying.damage);
        }

        break;
    }
    case ConfirmDamage: {
        DamageStruct damage = data.value<DamageStruct>();
        if (damage.card && damage.to->getMark("SlashIsDrank") > 0) {
            LogMessage log;
            log.type = "#AnalepticBuff";
            log.from = damage.from;
            log.to << damage.to;
            log.arg = QString::number(damage.damage);

            damage.damage += damage.to->getMark("SlashIsDrank");
            damage.to->setMark("SlashIsDrank", 0);

            log.arg2 = QString::number(damage.damage);

            room->sendLog(log);

            data = QVariant::fromValue(damage);
        }

        break;
    }
    case DamageDone: {
        DamageStruct damage = data.value<DamageStruct>();
        if (damage.from && !damage.from->isAlive())
            damage.from = NULL;
        data = QVariant::fromValue(damage);

        LogMessage log;

        if (damage.from) {
            log.type = "#Damage";
            log.from = damage.from;
        } else {
            log.type = "#DamageNoSource";
        }

        log.to << damage.to;
        log.arg = QString::number(damage.damage);

        switch (damage.nature) {
        case DamageStruct::Normal: log.arg2 = "normal_nature"; break;
        case DamageStruct::Fire: log.arg2 = "fire_nature"; break;
        case DamageStruct::Thunder: log.arg2 = "thunder_nature"; break;
        }

        room->sendLog(log);

        int new_hp = damage.to->getHp() - damage.damage;

        QString change_str = QString("%1:%2").arg(damage.to->objectName()).arg(-damage.damage);
        switch (damage.nature) {
        case DamageStruct::Fire: change_str.append("F"); break;
        case DamageStruct::Thunder: change_str.append("T"); break;
        default: break;
        }

        JsonArray arg;
        arg << damage.to->objectName() << -damage.damage << damage.nature;
        room->doBroadcastNotify(QSanProtocol::S_COMMAND_CHANGE_HP, arg);

        room->setTag("HpChangedData", data);

        if (damage.nature != DamageStruct::Normal && player->isChained() && !damage.chain) {
            int n = room->getTag("is_chained").toInt();
            n++;
            room->setTag("is_chained", n);
        }

        room->setPlayerProperty(damage.to, "hp", new_hp);

        break;
    }
    case DamageComplete: {
        DamageStruct damage = data.value<DamageStruct>();
        if (damage.prevented)
            break;
        if (damage.nature != DamageStruct::Normal && player->isChained()) {
            room->setPlayerProperty(player, "chained", false);
            room->setEmotion(player, "chain");
        }
        if (room->getTag("is_chained").toInt() > 0) {
            if (damage.nature != DamageStruct::Normal && !damage.chain) {
                // iron chain effect
                int n = room->getTag("is_chained").toInt();
                n--;
                room->setTag("is_chained", n);
                QList<ServerPlayer *> chained_players;
                if (room->getCurrent()->isDead())
                    chained_players = room->getOtherPlayers(room->getCurrent());
                else
                    chained_players = room->getAllPlayers();
                foreach (ServerPlayer *chained_player, chained_players) {
                    if (chained_player->isChained()) {
                        room->getThread()->delay();
                        LogMessage log;
                        log.type = "#IronChainDamage";
                        log.from = chained_player;
                        room->sendLog(log);

                        DamageStruct chain_damage = damage;
                        chain_damage.to = chained_player;
                        chain_damage.chain = true;
                        chain_damage.transfer = false;
                        chain_damage.transfer_reason = QString();

                        room->damage(chain_damage);
                    }
                }
            }
        }
        if (room->getMode() == "02_1v1" || room->getMode() == "06_XMode") {
            foreach (ServerPlayer *p, room->getAllPlayers()) {
                if (p->hasFlag("Global_DebutFlag")) {
                    p->setFlags("-Global_DebutFlag");
                    if (room->getMode() == "02_1v1")
                        room->getThread()->trigger(Debut, room, p);
                }
            }
        }
        break;
    }
    case CardEffected: {
        if (data.canConvert<CardEffectStruct>()) {
            CardEffectStruct effect = data.value<CardEffectStruct>();
            if (!effect.card->isKindOf("Slash") && effect.nullified) {
                LogMessage log;
                log.type = "#CardNullified";
                log.from = effect.to;
                log.arg = effect.card->objectName();
                room->sendLog(log);

                return true;
            } else if (effect.card->getTypeId() == Card::TypeTrick) {
                if (room->isCanceled(effect)) {
                    effect.to->setFlags("Global_NonSkillNullify");
                    return true;
                } else {
                    room->getThread()->trigger(TrickEffect, room, effect.to, data);
                }
            }
            if (effect.to->isAlive() || effect.card->isKindOf("Slash"))
                effect.card->onEffect(effect);
        }

        break;
    }
    case SlashEffected: {
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        if (effect.nullified) {
            LogMessage log;
            log.type = "#CardNullified";
            log.from = effect.to;
            log.arg = effect.slash->objectName();
            room->sendLog(log);

            return true;
        }
        if (effect.jink_num > 0)
            room->getThread()->trigger(SlashProceed, room, effect.from, data);
        else
            room->slashResult(effect, NULL);
        break;
    }
    case SlashProceed: {
        SlashEffectStruct effect = data.value<SlashEffectStruct>();
        QString slasher = effect.from->objectName();
        if (!effect.to->isAlive())
            break;
        if (effect.jink_num == 1) {
            const Card *jink = room->askForCard(effect.to, "jink", "slash-jink:" + slasher, data, Card::MethodUse, effect.from);
            room->slashResult(effect, room->isJinkEffected(effect.to, jink) ? jink : NULL);
        } else {
            DummyCard *jink = new DummyCard;
            const Card *asked_jink = NULL;
            for (int i = effect.jink_num; i > 0; i--) {
                QString prompt = QString("@multi-jink%1:%2::%3").arg(i == effect.jink_num ? "-start" : QString())
                    .arg(slasher).arg(i);
                asked_jink = room->askForCard(effect.to, "jink", prompt, data, Card::MethodUse, effect.from);
                if (!room->isJinkEffected(effect.to, asked_jink)) {
                    delete jink;
                    room->slashResult(effect, NULL);
                    return false;
                } else {
                    jink->addSubcard(asked_jink->getEffectiveId());
                }
            }
            room->slashResult(effect, jink);
        }

        break;
    }
    case SlashHit: {
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        if (effect.drank > 0) effect.to->setMark("SlashIsDrank", effect.drank);
        room->damage(DamageStruct(effect.slash, effect.from, effect.to, 1, effect.nature));

        break;
    }
    case GameOverJudge: {
        if (room->getMode() == "02_1v1") {
            QStringList list = player->tag["1v1Arrange"].toStringList();
            QString rule = Config.value("1v1/Rule", "2013").toString();
            if (list.length() > ((rule == "2013") ? 3 : 0)) break;
        }

        QString winner = getWinner(player);
        if (!winner.isNull()) {
            room->gameOver(winner);
            return true;
        }

        break;
    }
    case BuryVictim: {
        DeathStruct death = data.value<DeathStruct>();
        player->bury();

        if (room->getTag("SkipNormalDeathProcess").toBool())
            return false;

        ServerPlayer *killer = death.damage ? death.damage->from : NULL;
        if (killer)
            rewardAndPunish(killer, player);

        break;
    }
    case StartJudge: {
        int card_id = room->drawCard();

        JudgeStruct *judge = data.value<JudgeStruct *>();
        judge->card = Sanguosha->getCard(card_id);

        LogMessage log;
        log.type = "$InitialJudge";
        log.from = player;
        log.card_str = QString::number(judge->card->getEffectiveId());
        room->sendLog(log);

        room->moveCardTo(judge->card, NULL, judge->who, Player::PlaceJudge,
            CardMoveReason(CardMoveReason::S_REASON_JUDGE,
            judge->who->objectName(),
            QString(), QString(), judge->reason), true);
        judge->updateResult();
        break;
    }
    case FinishRetrial: {
        JudgeStruct *judge = data.value<JudgeStruct *>();

        LogMessage log;
        log.type = "$JudgeResult";
        log.from = player;
        log.card_str = QString::number(judge->card->getEffectiveId());
        room->sendLog(log);

        int delay = Config.AIDelay;
        if (judge->time_consuming) delay /= 1.25;
        room->getThread()->delay(delay);
        if (judge->play_animation) {
            room->sendJudgeResult(judge);
            room->getThread()->delay(Config.S_JUDGE_LONG_DELAY);
        }

        break;
    }
    case FinishJudge: {
        JudgeStruct *judge = data.value<JudgeStruct *>();

        if (room->getCardPlace(judge->card->getEffectiveId()) == Player::PlaceJudge) {
            CardMoveReason reason(CardMoveReason::S_REASON_JUDGEDONE, judge->who->objectName(),
                judge->reason, QString());
            if (judge->retrial_by_response)
                reason.m_extraData = QVariant::fromValue(judge->retrial_by_response);
            room->moveCardTo(judge->card, judge->who, NULL, Player::DiscardPile, reason, true);
        }

        break;
    }
    case ChoiceMade: {
        foreach (ServerPlayer *p, room->getAlivePlayers()) {
            foreach (QString flag, p->getFlagList()) {
                if (flag.startsWith("Global_") && flag.endsWith("Failed"))
                    room->setPlayerFlag(p, "-" + flag);
            }
        }
        break;
    }
    default:
        break;
    }

    return false;
}

void GameRule::rewardAndPunish(ServerPlayer *killer, ServerPlayer *victim) const
{
    if (killer->isDead())
        return;

    if (victim->getRole() == "rebel" && killer != victim)
        killer->drawCards(3, "kill");
    else if (victim->getRole() == "loyalist" && killer->getRole() == "lord")
        killer->throwAllHandCardsAndEquips();
}

QString GameRule::getWinner(ServerPlayer *victim) const
{
    Room *room = victim->getRoom();
    QString winner;

    QStringList alive_roles = room->aliveRoles(victim);
    switch (victim->getRoleEnum()) {
    case Player::Lord: {
        if (alive_roles.length() == 1 && alive_roles.first() == "renegade")
            winner = room->getAlivePlayers().first()->objectName();
        else
            winner = "rebel";
        break;
    }
    case Player::Rebel:
    case Player::Renegade: {
        if (!alive_roles.contains("rebel") && !alive_roles.contains("renegade"))
            winner = "lord+loyalist";
        break;
    }
    default:
        break;
    }

    return winner;
}
