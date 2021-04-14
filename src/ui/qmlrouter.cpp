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

    if (Sanguosha->getViewAsSkill(skill_name)) {
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
            if (skill->getFrequency(Self) == Skill::Wake) {
                if (Self->getMark(skill->objectName()) > 0)
                    ret << str;
            }
            else
                ret << str;
        }
    }

    return ret;
}

bool QmlRouter::roomscene_card_idenabled(int id, int index) // index 表示第几段代码 = =b
{
    bool enabled = true;
    const Card *card = Sanguosha->getCard(id);
    Client::Status status = ClientInstance->getStatus();
    switch (index) {
    case 0:
        if (card != NULL) {
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
        return enabled;
    case 1:
        return (card->targetFixed()
                || ((status & Client::ClientStatusBasicMask) == Client::Responding
                && (status == Client::Responding || (card->getTypeId() != Card::TypeSkill && status != Client::RespondingUse)))
                || ClientInstance->getStatus() == Client::AskForShowOrPindian);
    }
}

QString QmlRouter::roomscene_enable_targets(int id, QStringList selected_targets)
{
    return enable_targets(Sanguosha->getCard(id), selected_targets);
}

QStringList QmlRouter::roomscene_update_targets_enablity(int id, QStringList selected_targets)
{
    return updateTargetsEnablity(Sanguosha->getCard(id), selected_targets);
}

QString QmlRouter::roomscene_update_selected_targets(int id, QString player_name, bool selected, QStringList targets)
{
    return updateSelectedTargets(Sanguosha->getCard(id), player_name, selected, targets);
}

void QmlRouter::roomscene_use_card(int id, QStringList selected_targets)
{
    useCard(Sanguosha->getCard(id), selected_targets);
}

const Card *QmlRouter::qml_getCard(QVariant data)
{
    if (JsonUtils::isNumber(data)) {
        return Sanguosha->getCard(data.toInt());
    } else {
        QJsonObject json_obj = QJsonDocument::fromJson(data.toByteArray()).object();
        QString skill_name = json_obj.value("skill").toString();
        QList<const Card *> subcards;
        foreach (QVariant d, json_obj.value("subcards").toArray().toVariantList()) {
            subcards << Sanguosha->getCard(d.toInt());
        }
        return Sanguosha->getViewAsSkill(skill_name)->viewAs(subcards);
    }
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

