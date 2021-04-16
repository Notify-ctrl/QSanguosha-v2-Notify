#include "qmlrouter.h"
#include "client.h"
#include "roomscene.h"

QmlRouter *Router = NULL;

QmlRouter::QmlRouter(QObject *parent) : QObject(parent)
{

}

bool QmlRouter::card_isAvailable(int card, QString player_name)
{
    return Sanguosha->getCard(card)->isAvailable(ClientInstance->getPlayer(player_name));
}

QString QmlRouter::get_skill_details(QString skill_name)
{
    static QMap<Skill::Frequency, QString> frequencyMap;
    frequencyMap[Skill::Frequent] = "frequent";
    frequencyMap[Skill::NotFrequent] = "frequent";
    frequencyMap[Skill::Compulsory] = "compulsory";
    frequencyMap[Skill::NotCompulsory] = "compulsory";
    frequencyMap[Skill::Limited] = "oneoff";
    frequencyMap[Skill::Wake] = "awaken";

    const Skill *skill = Sanguosha->getSkill(skill_name);
    if (!skill || !skill->isVisible()) return QString();

    QJsonObject obj;
    QJsonDocument doc;
    obj.insert("name", skill->objectName());

    if (qml_getViewAsSkill(skill_name)) {
        obj.insert("enabled", false);
        obj.insert("type", "proactive");
        obj.insert("pressed", false);
    } else {
        obj.insert("enabled", skill->getFrequency() != Skill::Wake);
        obj.insert("type", frequencyMap.value(skill->getFrequency()));
        obj.insert("pressed", skill->getFrequency() == Skill::Frequent);
    }

    doc.setObject(obj);
    return doc.toJson();
}

bool QmlRouter::vs_view_filter(QString skill_name, QList<int> ids, int id)
{
    const ViewAsSkill *skill = qml_getViewAsSkill(skill_name);
    QList<const Card *> selected;
    foreach (int cid, ids)
        selected << Sanguosha->getCard(cid);
    return (skill && skill->viewFilter(selected, Sanguosha->getCard(id)));
}

bool QmlRouter::vs_can_view_as(QString skill_name, QList<int> ids)
{
    const ViewAsSkill *skill = qml_getViewAsSkill(skill_name);
    if (!skill) return false;
    QList<const Card *> selected;
    foreach (int cid, ids)
        selected << Sanguosha->getCard(cid);

    const Card *vs_card = skill->viewAs(selected);
    if (vs_card != NULL) {
        delete vs_card;
        return true;
    }
    return false;
}

void QmlRouter::update_discard_skill()
{
    DiscardSkill *discard_skill = ClientInstance->discard_skill;
    discard_skill->setNum(ClientInstance->discard_num);
    discard_skill->setMinNum(ClientInstance->min_num);
    discard_skill->setIncludeEquip(ClientInstance->m_canDiscardEquip);
    discard_skill->setIsDiscard(ClientInstance->getStatus() != Client::Exchanging);
    discard_skill->setPattern(ClientInstance->m_cardDiscardPattern);
}

QString QmlRouter::update_response_skill()
{
    ResponseSkill *response_skill = (ResponseSkill *)ClientInstance->aux_skill_map->value("response-skill");
    QString pattern = Sanguosha->currentRoomState()->getCurrentCardUsePattern();
    QRegExp rx("@@?([_A-Za-z]+)(\\d+)?!?");
    if (rx.exactMatch(pattern)) {
        QString skill_name = rx.capturedTexts().at(1);
        const ViewAsSkill *skill = Sanguosha->getViewAsSkill(skill_name);
        if (skill) {
            CardUseStruct::CardUseReason reason = CardUseStruct::CARD_USE_REASON_RESPONSE;
            if (ClientInstance->getStatus() == Client::RespondingUse)
                reason = CardUseStruct::CARD_USE_REASON_RESPONSE_USE;
            QString tempUseFlag = "RoomScene_" + skill_name + "TempUse";
            if (!Self->hasFlag(tempUseFlag))
                Self->setFlags(tempUseFlag);
            bool available = skill->isAvailable(Self, reason, pattern);
            Self->setFlags("-" + tempUseFlag);
            if (!available) {
                ClientInstance->onPlayerResponseCard(NULL);
                return QString();
            }
            return skill_name;
        }
    } else {
        if (pattern.endsWith("!"))
            pattern = pattern.mid(0, pattern.length() - 1);
        response_skill->setPattern(pattern);
        if (ClientInstance->getStatus() == Client::RespondingForDiscard)
            response_skill->setRequest(Card::MethodDiscard);
        else if (ClientInstance->getStatus() == Client::RespondingNonTrigger)
            response_skill->setRequest(Card::MethodNone);
        else if (ClientInstance->getStatus() == Client::RespondingUse)
            response_skill->setRequest(Card::MethodUse);
        else
            response_skill->setRequest(Card::MethodResponse);
        return "response-skill";
    }
    return QString();
}

QStringList QmlRouter::roomscene_get_enable_skills(QStringList skill_names, int newStatus)
{
    QStringList ret;
    foreach (QString str, skill_names) {
        const Skill *skill = Sanguosha->getSkill(str);
        if (skill->inherits("ViewAsSkill")) {
            QString pattern = Sanguosha->currentRoomState()->getCurrentCardUsePattern();
            QRegExp rx("@@?([_A-Za-z]+)(\\d+)?!?");
            CardUseStruct::CardUseReason reason = CardUseStruct::CARD_USE_REASON_UNKNOWN;
            if ((newStatus & Client::ClientStatusBasicMask) == Client::Responding) {
                if (newStatus == Client::RespondingUse)
                    reason = CardUseStruct::CARD_USE_REASON_RESPONSE_USE;
                else if (newStatus == Client::Responding || rx.exactMatch(pattern))
                    reason = CardUseStruct::CARD_USE_REASON_RESPONSE;
            } else if (newStatus == Client::Playing)
                reason = CardUseStruct::CARD_USE_REASON_PLAY;
            if (qobject_cast<const ViewAsSkill *>(skill)->isAvailable(Self, reason, pattern) && !pattern.endsWith("!"))
                ret << str;
        } else {
            const ViewAsSkill *vs_skill = Sanguosha->getViewAsSkill(str);
            if (vs_skill) {
                QString pattern = Sanguosha->currentRoomState()->getCurrentCardUsePattern();
                QRegExp rx("@@?([_A-Za-z]+)(\\d+)?!?");
                CardUseStruct::CardUseReason reason = CardUseStruct::CARD_USE_REASON_UNKNOWN;
                if ((newStatus & Client::ClientStatusBasicMask) == Client::Responding) {
                    if (newStatus == Client::RespondingUse)
                        reason = CardUseStruct::CARD_USE_REASON_RESPONSE_USE;
                    else if (newStatus == Client::Responding || rx.exactMatch(pattern))
                        reason = CardUseStruct::CARD_USE_REASON_RESPONSE;
                } else if (newStatus == Client::Playing)
                    reason = CardUseStruct::CARD_USE_REASON_PLAY;
                if (vs_skill->isAvailable(Self, reason, pattern) && !pattern.endsWith("!"))
                    ret << str;
            } else {
            if (skill->getFrequency(Self) == Skill::Wake) {
                if (Self->getMark(skill->objectName()) > 0)
                    ret << str;
            }
            else
                ret << str;
            }
        }
    }

    return ret;
}

QString QmlRouter::roomscene_enable_targets(int id, QStringList selected_targets)
{
    return enable_targets(Sanguosha->getCard(id), selected_targets);
}

QString QmlRouter::roomscene_enable_targets(QString json_data, QStringList selected_targets)
{
    const Card *card = qml_getCard(json_data);
    QString ret = enable_targets(card, selected_targets);
    if (card && card->isVirtualCard()) delete card;
    return ret;
}

QStringList QmlRouter::roomscene_update_targets_enablity(int id, QStringList selected_targets)
{
    return updateTargetsEnablity(Sanguosha->getCard(id), selected_targets);
}

QStringList QmlRouter::roomscene_update_targets_enablity(QString json_data, QStringList selected_targets)
{
    const Card *card = qml_getCard(json_data);
    QStringList ret = updateTargetsEnablity(card, selected_targets);
    if (card && card->isVirtualCard()) delete card;
    return ret;
}

QString QmlRouter::roomscene_update_selected_targets(int id, QString player_name, bool selected, QStringList targets)
{
    return updateSelectedTargets(Sanguosha->getCard(id), player_name, selected, targets);
}

QString QmlRouter::roomscene_update_selected_targets(QString json_data, QString player_name, bool selected, QStringList targets)
{
    const Card *card = qml_getCard(json_data);
    QString ret = updateSelectedTargets(card, player_name, selected, targets);
    if (card && card->isVirtualCard()) delete card;
    return ret;
}

void QmlRouter::roomscene_use_card(int id, QStringList selected_targets)
{
    useCard(Sanguosha->getCard(id), selected_targets);
}

void QmlRouter::roomscene_use_card(QString json_data, QStringList selected_targets)
{
    useCard(qml_getCard(json_data), selected_targets);
}

void QmlRouter::roomscene_finish()
{
    if (ClientInstance->getStatus() == Client::Playing)
        ClientInstance->onPlayerResponseCard(NULL);
}

void QmlRouter::on_player_response_card(int id, QStringList targets)
{
    const Card *card = Sanguosha->getCard(id);
    QList<const Player *> selected_targets;
    foreach (QString str, targets) {
        selected_targets << ClientInstance->getPlayer(str);
    }
    ClientInstance->onPlayerResponseCard(card, selected_targets);
}

void QmlRouter::on_player_response_card(QString json_data, QStringList targets)
{
    const Card *card = qml_getCard(json_data);
    QList<const Player *> selected_targets;
    foreach (QString str, targets) {
        selected_targets << ClientInstance->getPlayer(str);
    }
    ClientInstance->onPlayerResponseCard(card, selected_targets);
}

const Card *QmlRouter::qml_getCard(QString json_data)
{
    QJsonObject json_obj = QJsonDocument::fromJson(json_data.toLatin1()).object();
    QString skill_name = json_obj.value("skill").toString();
    QList<const Card *> subcards;
    foreach (QVariant d, json_obj.value("subcards").toArray().toVariantList()) {
        subcards << Sanguosha->getCard(d.toInt());
    }

    const ViewAsSkill *skill = qml_getViewAsSkill(skill_name);
    if (!skill) return NULL;
    return skill->viewAs(subcards);
}

QString QmlRouter::enable_targets(const Card *card, QStringList selected)
{
    bool enabled = true;
    QJsonObject obj;
    QJsonDocument doc;
    QList<const Player *> selected_targets;
    foreach (QString str, selected) {
        selected_targets << ClientInstance->getPlayer(str);
    }
    if (card != NULL) {
        Client::Status status = ClientInstance->getStatus();
        if (status == Client::Playing && !card->isAvailable(Self))
            enabled = false;
        if (status == Client::Responding || status == Client::RespondingUse) {
            Card::HandlingMethod method = card->getHandlingMethod();
            if (status == Client::Responding && method == Card::MethodUse)
                method = Card::MethodResponse;
            if (Self->isCardLimited(card, method))
                enabled = false;
        }
        if (status == Client::RespondingUse && ClientInstance->m_respondingUseFixedTarget
            && Sanguosha->isProhibited(Self, ClientInstance->m_respondingUseFixedTarget, card))
            enabled = false;
        if (status == Client::RespondingForDiscard && Self->isCardLimited(card, Card::MethodDiscard))
            enabled = false;
    }
    if (!enabled) {
        obj.insert("ok_enabled", false);
        obj.insert("enabled_targets", QJsonArray::fromStringList(QStringList()));
        doc.setObject(obj);
        return doc.toJson();
    }

    if (card == NULL) {
        obj.insert("ok_enabled", false);
        obj.insert("enabled_targets", QJsonArray::fromStringList(QStringList()));
        doc.setObject(obj);
        return doc.toJson();
    }

    Client::Status status = ClientInstance->getStatus();
    if (card->targetFixed()
        || ((status & Client::ClientStatusBasicMask) == Client::Responding
        && (status == Client::Responding || (card->getTypeId() != Card::TypeSkill && status != Client::RespondingUse)))
        || ClientInstance->getStatus() == Client::AskForShowOrPindian) {
        obj.insert("ok_enabled", true);
        obj.insert("enabled_targets", QJsonArray::fromStringList(QStringList()));
        doc.setObject(obj);
        return doc.toJson();
    }

    obj.insert("ok_enabled", card->targetsFeasible(selected_targets, Self));
    obj.insert("enabled_targets", QJsonArray::fromStringList(updateTargetsEnablity(card, selected)));
    doc.setObject(obj);
    return doc.toJson();
}

QStringList QmlRouter::updateTargetsEnablity(const Card *card, QStringList selected)
{
    QStringList ret;
    QList<const Player *> selected_targets;
    foreach (QString str, selected) {
        selected_targets << ClientInstance->getPlayer(str);
    }
    foreach (const ClientPlayer *player, ClientInstance->getPlayers()) {
        int maxVotes = 0;
        if (card) {
            card->targetFilter(selected_targets, player, Self, maxVotes);
        }

        if (selected.contains(player->objectName())) continue;

        bool enabled = (card == NULL) || ((!Sanguosha->isProhibited(Self, player, card, selected_targets)) && maxVotes > 0);

        if (card && enabled) ret << player->objectName();
    }
    return ret;
}

QString QmlRouter::updateSelectedTargets(const Card *card, QString player_name, bool selected, QStringList targets)
{
    QJsonObject obj;
    QJsonDocument doc;
    QList<const Player *> selected_targets;
    foreach (QString str, targets) {
        selected_targets << ClientInstance->getPlayer(str);
    }
    if (card) {
        ClientPlayer *player = ClientInstance->getPlayer(player_name);
        if (selected) {
            targets << player->objectName();
        } else {
            targets.removeAll(player_name);
            foreach (const Player *cp, selected_targets) {
                QList<const Player *> tempPlayers = QList<const Player *>(selected_targets);
                tempPlayers.removeAll(cp);
                if (!card->targetFilter(tempPlayers, cp, Self) || Sanguosha->isProhibited(Self, cp, card, selected_targets)) {
                    targets = QStringList();
                    break;
                }
            }
        }
    } else {
        targets = QStringList();
    }
    obj.insert("selected_targets", QJsonArray::fromStringList(targets));
    obj.insert("enabled_targets", QJsonArray::fromStringList(updateTargetsEnablity(card, targets)));
    selected_targets.clear();
    foreach (QString str, targets) {
        selected_targets << ClientInstance->getPlayer(str);
    }
    obj.insert("ok_enabled", card && card->targetsFeasible(selected_targets, Self));
    doc.setObject(obj);
    return doc.toJson();
}

void QmlRouter::useCard(const Card *card, QStringList targets)
{
    if (!card) return;
    QList<const Player *> selected_targets;
    foreach (QString str, targets) {
        selected_targets << ClientInstance->getPlayer(str);
    }
    if (card->targetFixed() || card->targetsFeasible(selected_targets, Self))
        ClientInstance->onPlayerResponseCard(card, selected_targets);
}

const ViewAsSkill *QmlRouter::qml_getViewAsSkill(QString skill_name)
{
    QMap<QString, void *> *map = ClientInstance->aux_skill_map;
    const ViewAsSkill *skill = Sanguosha->getViewAsSkill(skill_name);
    if (map->value(skill_name, NULL) != NULL) skill = (const ViewAsSkill *)(map->value(skill_name));
    return skill;
}

