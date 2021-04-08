#include "client.h"
#include "settings.h"
#include "engine.h"
// #include "choosegeneraldialog.h"
#include "nativesocket.h"
#include "recorder.h"
#include "json.h"
#include "clientplayer.h"
#include "clientstruct.h"
#include "util.h"
#include "wrapped-card.h"
#include "src/main.h"
// @TODO
#include "server.h"

using namespace std;
using namespace QSanProtocol;

Client *ClientInstance = NULL;

Client::Client(QObject *parent, const QString &filename)
    : QObject(parent), m_isDiscardActionRefusable(true), m_bossLevel(0),
    status(NotActive), alive_count(1), swap_pile(0), _m_roomState(true),
    player_count(1) // Self is not included!! Be care!!!
{
    ClientInstance = this;
    main_window->rootContext()->setContextProperty("ClientInstance", this);
    m_isGameOver = false;
    m_isDisconnected = true;

    m_callbacks[S_COMMAND_CHECK_VERSION] = &Client::checkVersion;
    m_callbacks[S_COMMAND_SETUP] = &Client::setup;
    m_callbacks[S_COMMAND_NETWORK_DELAY_TEST] = &Client::networkDelayTest;
    m_callbacks[S_COMMAND_ADD_PLAYER] = &Client::addPlayer;
    m_callbacks[S_COMMAND_REMOVE_PLAYER] = &Client::removePlayer;
    m_callbacks[S_COMMAND_START_IN_X_SECONDS] = &Client::startInXs;
    m_callbacks[S_COMMAND_ARRANGE_SEATS] = &Client::arrangeSeats;
    m_callbacks[S_COMMAND_WARN] = &Client::warn;
    m_callbacks[S_COMMAND_SPEAK] = &Client::speak;

    m_callbacks[S_COMMAND_GAME_START] = &Client::startGame;
    m_callbacks[S_COMMAND_GAME_OVER] = &Client::gameOver;

    m_callbacks[S_COMMAND_CHANGE_HP] = &Client::hpChange;
    m_callbacks[S_COMMAND_CHANGE_MAXHP] = &Client::maxhpChange;
    m_callbacks[S_COMMAND_KILL_PLAYER] = &Client::killPlayer;
    m_callbacks[S_COMMAND_REVIVE_PLAYER] = &Client::revivePlayer;
    m_callbacks[S_COMMAND_SHOW_CARD] = &Client::showCard;
    m_callbacks[S_COMMAND_UPDATE_CARD] = &Client::updateCard;
    m_callbacks[S_COMMAND_SET_MARK] = &Client::setMark;
    m_callbacks[S_COMMAND_LOG_SKILL] = &Client::log;
    m_callbacks[S_COMMAND_ATTACH_SKILL] = &Client::attachSkill;
    m_callbacks[S_COMMAND_MOVE_FOCUS] = &Client::moveFocus;
    m_callbacks[S_COMMAND_SET_EMOTION] = &Client::setEmotion;
    m_callbacks[S_COMMAND_INVOKE_SKILL] = &Client::skillInvoked;
    m_callbacks[S_COMMAND_SHOW_ALL_CARDS] = &Client::showAllCards;
    m_callbacks[S_COMMAND_SKILL_GONGXIN] = &Client::askForGongxin;
    m_callbacks[S_COMMAND_LOG_EVENT] = &Client::handleGameEvent;
    m_callbacks[S_COMMAND_ADD_HISTORY] = &Client::addHistory;
    m_callbacks[S_COMMAND_ANIMATE] = &Client::animate;
    m_callbacks[S_COMMAND_FIXED_DISTANCE] = &Client::setFixedDistance;
    m_callbacks[S_COMMAND_ATTACK_RANGE] = &Client::setAttackRangePair;
    m_callbacks[S_COMMAND_CARD_LIMITATION] = &Client::cardLimitation;
    m_callbacks[S_COMMAND_NULLIFICATION_ASKED] = &Client::setNullification;
    m_callbacks[S_COMMAND_ENABLE_SURRENDER] = &Client::enableSurrender;
    m_callbacks[S_COMMAND_EXCHANGE_KNOWN_CARDS] = &Client::exchangeKnownCards;
    m_callbacks[S_COMMAND_SET_KNOWN_CARDS] = &Client::setKnownCards;
    m_callbacks[S_COMMAND_VIEW_GENERALS] = &Client::viewGenerals;

    m_callbacks[S_COMMAND_UPDATE_STATE_ITEM] = &Client::updateStateItem;
    m_callbacks[S_COMMAND_AVAILABLE_CARDS] = &Client::setAvailableCards;

    m_callbacks[S_COMMAND_GET_CARD] = &Client::getCards;
    m_callbacks[S_COMMAND_LOSE_CARD] = &Client::loseCards;
    m_callbacks[S_COMMAND_SET_PROPERTY] = &Client::updateProperty;
    m_callbacks[S_COMMAND_RESET_PILE] = &Client::resetPiles;
    m_callbacks[S_COMMAND_UPDATE_PILE] = &Client::setPileNumber;
    m_callbacks[S_COMMAND_SYNCHRONIZE_DISCARD_PILE] = &Client::synchronizeDiscardPile;
    m_callbacks[S_COMMAND_CARD_FLAG] = &Client::setCardFlag;

    // interactive methods
    m_interactions[S_COMMAND_CHOOSE_GENERAL] = &Client::askForGeneral;
    m_interactions[S_COMMAND_CHOOSE_PLAYER] = &Client::askForPlayerChosen;
    m_interactions[S_COMMAND_CHOOSE_ROLE] = &Client::askForAssign;
    m_interactions[S_COMMAND_EXCHANGE_CARD] = &Client::askForExchange;
    m_interactions[S_COMMAND_ASK_PEACH] = &Client::askForSinglePeach;
    m_interactions[S_COMMAND_SKILL_GUANXING] = &Client::askForGuanxing;
    m_interactions[S_COMMAND_SKILL_GONGXIN] = &Client::askForGongxin;
    m_interactions[S_COMMAND_SKILL_YIJI] = &Client::askForYiji;
    m_interactions[S_COMMAND_PLAY_CARD] = &Client::activate;
    m_interactions[S_COMMAND_DISCARD_CARD] = &Client::askForDiscard;
    m_interactions[S_COMMAND_CHOOSE_SUIT] = &Client::askForSuit;
    m_interactions[S_COMMAND_CHOOSE_KINGDOM] = &Client::askForKingdom;
    m_interactions[S_COMMAND_RESPONSE_CARD] = &Client::askForCardOrUseCard;
    m_interactions[S_COMMAND_INVOKE_SKILL] = &Client::askForSkillInvoke;
    m_interactions[S_COMMAND_MULTIPLE_CHOICE] = &Client::askForChoice;
    m_interactions[S_COMMAND_NULLIFICATION] = &Client::askForNullification;
    m_interactions[S_COMMAND_SHOW_CARD] = &Client::askForCardShow;
    m_interactions[S_COMMAND_AMAZING_GRACE] = &Client::askForAG;
    m_interactions[S_COMMAND_PINDIAN] = &Client::askForPindian;
    m_interactions[S_COMMAND_CHOOSE_CARD] = &Client::askForCardChosen;
    m_interactions[S_COMMAND_SURRENDER] = &Client::askForSurrender;
    m_interactions[S_COMMAND_LUCK_CARD] = &Client::askForLuckCard;

    m_callbacks[S_COMMAND_FILL_AMAZING_GRACE] = &Client::fillAG;
    m_callbacks[S_COMMAND_TAKE_AMAZING_GRACE] = &Client::takeAG;
    m_callbacks[S_COMMAND_CLEAR_AMAZING_GRACE] = &Client::clearAG;
    m_callbacks[S_COMMAND_UPDATE_SKILL] = &Client::updateSkill;

    m_noNullificationThisTime = false;
    m_noNullificationTrickName = ".";
    m_respondingUseFixedTarget = NULL;

    Self = new ClientPlayer(this);
    main_window->rootContext()->setContextProperty("Self", Self);
    Self->setScreenName(Config.value("UserName").toString());
    Self->setProperty("avatar", Config.value("UserAvatar").toString());
    connect(Self, SIGNAL(phase_changed()), this, SLOT(alertFocus()));
    connect(Self, SIGNAL(role_changed(QString)), this, SLOT(notifyRoleChange(QString)));

    players << Self;

    lines_doc = new QTextDocument(this);

    prompt_doc = new QTextDocument(this);
    prompt_doc->setTextWidth(350);
#ifdef Q_OS_LINUX
    prompt_doc->setDefaultFont(QFont("DroidSansFallback"));
#else
    prompt_doc->setDefaultFont(QFont("SimHei"));
#endif

    if (!filename.isEmpty()) {
        socket = NULL;
        recorder = NULL;

        replayer = new Replayer(this, filename);
        connect(replayer, SIGNAL(command_parsed(QString)), this, SLOT(processServerPacket(QString)));
    } else {
        socket = new NativeClientSocket;
        recorder = new Recorder(this);
        m_isDisconnected = false;

        replayer = NULL;

        connect(socket, SIGNAL(message_got(const char *)), recorder, SLOT(record(const char *)));
        connect(socket, SIGNAL(message_got(const char *)), this, SLOT(processServerPacket(const char *)));
        connect(socket, SIGNAL(error_message(QString)), this, SIGNAL(error_message(QString)));
        socket->connectToHost();
    }
}

Client::~Client()
{
    ClientInstance = NULL;
}

void Client::updateCard(const QVariant &val)
{
    if (JsonUtils::isNumber(val.type())) {
        // reset card
        int cardId = val.toInt();
        Card *card = _m_roomState.getCard(cardId);
        if (!card->isModified()) return;
        _m_roomState.resetCard(cardId);
    } else {
        // update card
        JsonArray args = val.value<JsonArray>();
        Q_ASSERT(args.size() >= 5);
        int cardId = args[0].toInt();
        Card::Suit suit = (Card::Suit) args[1].toInt();
        int number = args[2].toInt();
        QString cardName = args[3].toString();
        QString skillName = args[4].toString();
        QString objectName = args[5].toString();
        QStringList flags;
        JsonUtils::tryParse(args[6], flags);

        Card *card = Sanguosha->cloneCard(cardName, suit, number, flags);
        card->setId(cardId);
        card->setSkillName(skillName);
        card->setObjectName(objectName);
        WrappedCard *wrapped = Sanguosha->getWrappedCard(cardId);
        Q_ASSERT(wrapped != NULL);
        wrapped->copyEverythingFrom(card);
    }
}

void Client::signup()
{
    if (replayer)
        replayer->start();
    else {
        JsonArray arg;
        arg << Config.value("EnableReconnection", false).toBool();
        arg << QString(Config.value("UserName").toString().toUtf8().toBase64());
        arg << Config.value("UserAvatar").toString();
        notifyServer(S_COMMAND_SIGNUP, arg);
    }
}
void Client::networkDelayTest(const QVariant &)
{
    notifyServer(S_COMMAND_NETWORK_DELAY_TEST);
}

void Client::replyToServer(CommandType command, const QVariant &arg)
{
    if (socket) {
        Packet packet(S_SRC_CLIENT | S_TYPE_REPLY | S_DEST_ROOM, command);
        packet.localSerial = _m_lastServerSerial;
        packet.setMessageBody(arg);
        socket->send(packet.toJson());
    }
}

void Client::handleGameEvent(const QVariant &arg)
{
    emit event_received(arg.value<QVariantList>());
}

void Client::requestServer(CommandType command, const QVariant &arg)
{
    if (socket) {
        Packet packet(S_SRC_CLIENT | S_TYPE_REQUEST | S_DEST_ROOM, command);
        packet.setMessageBody(arg);
        socket->send(packet.toJson());
    }
}

void Client::notifyServer(CommandType command, const QVariant &arg)
{
    if (socket) {
        Packet packet(S_SRC_CLIENT | S_TYPE_NOTIFICATION | S_DEST_ROOM, command);
        packet.setMessageBody(arg);
        socket->send(packet.toJson());
    }
}

void Client::checkVersion(const QVariant &server_version)
{
    QString version = server_version.toString();
    QString version_number, mod_name;
    if (version.contains(QChar(':'))) {
        QStringList texts = version.split(QChar(':'));
        version_number = texts.value(0);
        mod_name = texts.value(1);
    } else {
        version_number = version;
        mod_name = "official";
    }

    emit version_checked(version_number, mod_name);
}

void Client::setup(const QVariant &setup_json)
{
    if (socket && !socket->isConnected())
        return;

    QString setup_str = setup_json.toString();

    if (ServerInfo.parse(setup_str)) {
        emit server_connected();
        notifyServer(S_COMMAND_TOGGLE_READY);
    } else {
        QMessageBox::warning(NULL, tr("Warning"), tr("Setup string can not be parsed: %1").arg(setup_str));
    }
}

void Client::disconnectFromHost()
{
    if (!m_isDisconnected) {
        socket->disconnectFromHost();
        socket->deleteLater();
        m_isDisconnected = true;
    }
    // @TODO: Dirty code here. Remember to remove them.
    Server *server = qApp->findChild<Server *>();
    if (server) server->deleteLater();
}

void Client::processServerPacket(const QString &cmd)
{
    processServerPacket(cmd.toLatin1().data());
}

void Client::processServerPacket(const char *cmd)
{
    if (m_isGameOver) return;
    Packet packet;
    if (packet.parse(cmd)) {
        if (packet.getPacketType() == S_TYPE_NOTIFICATION) {
            Callback callback = m_callbacks[packet.getCommandType()];
            if (callback) {
                (this->*callback)(packet.getMessageBody());
            }
        } else if (packet.getPacketType() == S_TYPE_REQUEST) {
            if (!replayer)
                processServerRequest(packet);
        }
    }
}

bool Client::processServerRequest(const Packet &packet)
{
    setStatus(NotActive);
    _m_lastServerSerial = packet.globalSerial;
    CommandType command = packet.getCommandType();
    QVariant msg = packet.getMessageBody();

    if (!replayer) {
        Countdown countdown;
        countdown.current = 0;
        countdown.type = Countdown::S_COUNTDOWN_USE_DEFAULT;
        countdown.max = ServerInfo.getCommandTimeout(command, S_CLIENT_INSTANCE);
        setCountdown(countdown);
    }

    Callback callback = m_interactions[command];
    if (!callback) return false;
    (this->*callback)(msg);
    return true;
}

void Client::addPlayer(const QVariant &player_info)
{
    if (!player_info.canConvert<JsonArray>())
        return;

    JsonArray info = player_info.value<JsonArray>();
    if (info.size() < 3)
        return;

    QString name = info[0].toString();
    QString screen_name = QString::fromUtf8(QByteArray::fromBase64(info[1].toString().toLatin1()));
    QString avatar = info[2].toString();

    ClientPlayer *player = new ClientPlayer(this);
    player->setObjectName(name);
    player->setScreenName(screen_name);
    player->setProperty("avatar", avatar);

    players << player;
    alive_count++;
    player_count++;
    emit player_added(player);
}

void Client::updateProperty(const QVariant &arg)
{
    JsonArray args = arg.value<JsonArray>();
    if (!JsonUtils::isStringArray(args, 0, 2)) return;
    QString object_name = args[0].toString();
    ClientPlayer *player = getPlayer(object_name);
    if (!player) return;
    player->setProperty(args[1].toString().toLatin1().constData(), args[2].toString());
    QVariantList sig_args;
    sig_args << object_name << args[1].toString() << args[2];
    emit property_updated(sig_args);
}

void Client::removePlayer(const QVariant &player_name)
{
    QString name = player_name.toString();
    ClientPlayer *player = findChild<ClientPlayer *>(name);
    if (player) {
        player->setParent(NULL);
        alive_count--;
        player_count--;
        emit player_removed(name);
    }
}

bool Client::_loseSingleCard(int card_id, CardsMoveStruct move)
{
    const Card *card = Sanguosha->getCard(card_id);
    if (move.from)
        move.from->removeCard(card, move.from_place);
    else {
        if (move.from_place == Player::DiscardPile)
            discarded_list.removeOne(card);
        else if (move.from_place == Player::DrawPile && !Self->hasFlag("marshalling"))
            pile_num--;
    }
    return true;
}

bool Client::_getSingleCard(int card_id, CardsMoveStruct move)
{
    const Card *card = Sanguosha->getCard(card_id);
    if (move.to)
        move.to->addCard(card, move.to_place);
    else {
        if (move.to_place == Player::DrawPile)
            pile_num++;
        else if (move.to_place == Player::DiscardPile)
            discarded_list.prepend(card);
    }
    return true;
}

void Client::getCards(const QVariant &arg)
{
    JsonArray args = arg.value<JsonArray>();
    Q_ASSERT(args.size() >= 1);
    int moveId = args[0].toInt();
    QList<CardsMoveStruct> moves;
    for (int i = 1; i < args.length(); i++) {
        CardsMoveStruct move;
        if (!move.tryParse(args[i])) return;
        move.from = getPlayer(move.from_player_name);
        move.to = getPlayer(move.to_player_name);
        Player::Place dstPlace = move.to_place;

        if (dstPlace == Player::PlaceSpecial)
            ((ClientPlayer *)move.to)->changePile(move.to_pile_name, true, move.card_ids);
        else {
            foreach(int card_id, move.card_ids)
                _getSingleCard(card_id, move); // DDHEJ->DDHEJ, DDH/EJ->EJ
        }
        moves.append(move);
    }
    updatePileNum();
    emit move_cards_got(moveId, moves);
    // @TODO
    for (int i = 0; i < moves.length(); i++) {
        QString log = getCardLog(moves[i]);
        if (log != QString()) emit log_received(log);
    }
}

// @TODO: remove it
QString Client::getCardLog(CardsMoveStruct &move)
{
    if (move.card_ids.isEmpty()) return QString();
    if (move.to
        && (move.to_place == Player::PlaceHand
        || move.to_place == Player::PlaceEquip
        || move.to_place == Player::PlaceSpecial)
        && move.from_place != Player::DrawPile) {
        foreach(QString flag, move.to->getFlagList())
            if (flag.endsWith("_InTempMoving"))
                return QString();
    }

    // private pile
    if (move.to_place == Player::PlaceSpecial && !move.to_pile_name.isNull() && !move.to_pile_name.startsWith('#')) {
        bool hidden = (move.card_ids.contains(Card::S_UNKNOWN_CARD_ID));
        if (hidden)
            return appendLog("#RemoveFromGame", QString(), QStringList(), QString(), move.to_pile_name, QString::number(move.card_ids.length()));
        else
            return appendLog("$AddToPile", QString(), QStringList(), IntList2StringList(move.card_ids).join("+"), move.to_pile_name);
    }
    if (move.from_place == Player::PlaceSpecial && move.to
        && move.reason.m_reason == CardMoveReason::S_REASON_EXCHANGE_FROM_PILE) {
        bool hidden = (move.card_ids.contains(Card::S_UNKNOWN_CARD_ID));
        if (!hidden)
            return appendLog("$GotCardFromPile", move.to->objectName(), QStringList(), IntList2StringList(move.card_ids).join("+"), move.from_pile_name);
        else
            return appendLog("#GotNCardFromPile", move.to->objectName(), QStringList(), QString(), move.from_pile_name, QString::number(move.card_ids.length()));
    }
    //DrawNCards
    if (move.from_place == Player::DrawPile && move.to_place == Player::PlaceHand) {
        QString to_general = move.to->objectName();
        bool hidden = (move.card_ids.contains(Card::S_UNKNOWN_CARD_ID));
        if (!hidden)
            return appendLog("$DrawCards", to_general, QStringList(), IntList2StringList(move.card_ids).join("+"),
            QString::number(move.card_ids.length()));
        else
            return appendLog("#DrawNCards", to_general, QStringList(), QString(),
            QString::number(move.card_ids.length()));

    }
    if ((move.from_place == Player::PlaceTable || move.from_place == Player::PlaceJudge)
        && move.to_place == Player::PlaceHand
        && move.reason.m_reason != CardMoveReason::S_REASON_PREVIEW) {
        QString to_general = move.to->objectName();
        QList<int> ids = move.card_ids;
        ids.removeAll(Card::S_UNKNOWN_CARD_ID);
        if (!ids.isEmpty()) {
            QString card_str = IntList2StringList(ids).join("+");
            return appendLog("$GotCardBack", to_general, QStringList(), card_str);
        }
    }
    if (move.from_place == Player::DiscardPile && move.to_place == Player::PlaceHand) {
        QString to_general = move.to->objectName();
        QString card_str = IntList2StringList(move.card_ids).join("+");
        return appendLog("$RecycleCard", to_general, QStringList(), card_str);
    }
    if (move.from && move.from_place != Player::PlaceHand && move.from_place != Player::PlaceJudge
        && move.to_place != Player::PlaceDelayedTrick && move.to_place != Player::PlaceJudge
        && move.to && move.from != move.to) {
        QString from_general = move.from->objectName();
        QStringList tos;
        tos << move.to->objectName();
        QList<int> ids = move.card_ids;
        ids.removeAll(Card::S_UNKNOWN_CARD_ID);
        int hide = move.card_ids.length() - ids.length();
        if (!ids.isEmpty()) {
            QString card_str = IntList2StringList(ids).join("+");
            return appendLog("$MoveCard", from_general, tos, card_str);
        }
        if (hide > 0)
            return appendLog("#MoveNCards", from_general, tos, QString(), QString::number(hide));
    }
    if (move.from_place == Player::PlaceHand && move.to_place == Player::PlaceHand) {
        QString from_general = move.from->objectName();
        QStringList tos;
        tos << move.to->objectName();
        bool hidden = (move.card_ids.contains(Card::S_UNKNOWN_CARD_ID));
        if (hidden)
            return appendLog("#MoveNCards", from_general, tos, QString(), QString::number(move.card_ids.length()));
        else
            return appendLog("$MoveCard", from_general, tos, IntList2StringList(move.card_ids).join("+"));
    }
    if (move.from && move.to) {
        // both src and dest are player
        QString type;
        if (move.to_place == Player::PlaceDelayedTrick) {
            if (move.from_place == Player::PlaceDelayedTrick && move.from != move.to)
                type = "$LightningMove";
            else
                type = "$PasteCard";
        }
        if (!type.isNull()) {
            QString from_general = move.from->objectName();
            QStringList tos;
            tos << move.to->objectName();
            return appendLog(type, from_general, tos, QString::number(move.card_ids.first()));
        }
    }
    if (move.from && move.to && move.from_place == Player::PlaceEquip && move.to_place == Player::PlaceEquip) {
        QString type = "$Install";
        QString to_general = move.to->objectName();
        foreach(int card_id, move.card_ids)
            return appendLog(type, to_general, QStringList(), QString::number(card_id));
    }
    if (move.reason.m_reason == CardMoveReason::S_REASON_TURNOVER)
        return appendLog("$TurnOver", move.reason.m_playerId, QStringList(), IntList2StringList(move.card_ids).join("+"));

    return QString();
}


void Client::loseCards(const QVariant &arg)
{
    JsonArray args = arg.value<JsonArray>();
    Q_ASSERT(args.size() >= 1);
    int moveId = args[0].toInt();
    QList<CardsMoveStruct> moves;
    for (int i = 1; i < args.length(); i++) {
        CardsMoveStruct move;
        if (!move.tryParse(args[i])) return;
        move.from = getPlayer(move.from_player_name);
        move.to = getPlayer(move.to_player_name);
        Player::Place srcPlace = move.from_place;

        if (srcPlace == Player::PlaceSpecial)
            ((ClientPlayer *)move.from)->changePile(move.from_pile_name, false, move.card_ids);
        else {
            foreach (int card_id, move.card_ids)
                _loseSingleCard(card_id, move); // DDHEJ->DDHEJ, DDH/EJ->EJ
        }
        moves.append(move);
    }
    updatePileNum();
    emit move_cards_lost(moveId, moves);
}

void Client::onPlayerChooseGeneral(const QString &item_name)
{
    setStatus(NotActive);
    if (!item_name.isEmpty()) {
        replyToServer(S_COMMAND_CHOOSE_GENERAL, item_name);
        Sanguosha->playSystemAudioEffect("choose-item");
    }
}

void Client::requestCheatRunScript(const QString &script)
{
    JsonArray cheatReq;
    cheatReq << (int)S_CHEAT_RUN_SCRIPT;
    cheatReq << script;
    requestServer(S_COMMAND_CHEAT, cheatReq);
}

void Client::requestCheatRevive(const QString &name)
{
    JsonArray cheatReq;
    cheatReq << (int)S_CHEAT_REVIVE_PLAYER;
    cheatReq << name;
    requestServer(S_COMMAND_CHEAT, cheatReq);
}

void Client::requestCheatDamage(const QString &source, const QString &target, DamageStruct::Nature nature, int points)
{
    JsonArray cheatReq, cheatArg;
    cheatArg << source;
    cheatArg << target;
    cheatArg << (int)nature;
    cheatArg << points;

    cheatReq << (int)S_CHEAT_MAKE_DAMAGE;
    cheatReq << QVariant(cheatArg);
    requestServer(S_COMMAND_CHEAT, cheatReq);
}

void Client::requestCheatKill(const QString &killer, const QString &victim)
{
    JsonArray cheatArg;
    cheatArg << (int)S_CHEAT_KILL_PLAYER;
    cheatArg << QVariant(JsonArray() << killer << victim);
    requestServer(S_COMMAND_CHEAT, cheatArg);
}

void Client::requestCheatGetOneCard(int card_id)
{
    JsonArray cheatArg;
    cheatArg << (int)S_CHEAT_GET_ONE_CARD;
    cheatArg << card_id;
    requestServer(S_COMMAND_CHEAT, cheatArg);
}

void Client::requestCheatChangeGeneral(const QString &name, bool isSecondaryHero)
{
    JsonArray cheatArg;
    cheatArg << (int)S_CHEAT_CHANGE_GENERAL;
    cheatArg << name;
    cheatArg << isSecondaryHero;
    requestServer(S_COMMAND_CHEAT, cheatArg);
}

void Client::addRobot(int num)
{
    notifyServer(S_COMMAND_ADD_ROBOT, num);
}

void Client::onPlayerResponseCard(const Card *card, const QList<const Player *> &targets)
{
    if (Self->hasFlag("Client_PreventPeach")) {
        Self->setFlags("-Client_PreventPeach");
        Self->removeCardLimitation("use", "Peach$0");
    }
    if ((status & ClientStatusBasicMask) == Responding)
        _m_roomState.setCurrentCardUsePattern(QString());
    if (card == NULL) {
        replyToServer(S_COMMAND_RESPONSE_CARD);
    } else {
        JsonArray targetNames;
        if (!card->targetFixed()) {
            foreach (const Player *target, targets)
                targetNames << target->objectName();
        }

        replyToServer(S_COMMAND_RESPONSE_CARD, JsonArray() << card->toString() << QVariant::fromValue(targetNames));

        if (card->isVirtualCard() && !card->parent())
            delete card;
    }

    setStatus(NotActive);
}

void Client::startInXs(const QVariant &left_seconds)
{
    int seconds = left_seconds.toInt();
    if (seconds > 0)
        lines_doc->setHtml(tr("<p align = \"center\">Game will start in <b>%1</b> seconds...</p>").arg(seconds));
    else
        lines_doc->setHtml(QString());

    emit start_in_xs();
    if (seconds == 0 && Sanguosha->getScenario(ServerInfo.GameMode) == NULL) {
        emit avatars_hiden();
    }
}

void Client::arrangeSeats(const QVariant &seats_arr)
{
    QStringList player_names;
    if (seats_arr.canConvert<JsonArray>()) {
        JsonArray seats = seats_arr.value<JsonArray>();
        foreach (const QVariant &seat, seats) {
            player_names << seat.toString();
        }
    }
    players.clear();

    for (int i = 0; i < player_names.length(); i++) {
        ClientPlayer *player = findChild<ClientPlayer *>(player_names.at(i));

        Q_ASSERT(player != NULL);

        player->setSeat(i + 1);
        players << player;
    }

    QList<const ClientPlayer *> seats;
    int self_index = players.indexOf(Self);

    Q_ASSERT(self_index != -1);

    for (int i = self_index + 1; i < players.length(); i++)
        seats.append(players.at(i));
    for (int i = 0; i < self_index; i++)
        seats.append(players.at(i));

    Q_ASSERT(seats.length() == players.length() - 1);

    emit seats_arranged(seats);
}

void Client::notifyRoleChange(const QString &new_role)
{
    if (isNormalGameMode(ServerInfo.GameMode) && !new_role.isEmpty()) {
        QString prompt_str = tr("Your role is %1").arg(Sanguosha->translate(new_role));
        if (new_role != "lord")
            prompt_str += tr("\n wait for the lord player choosing general, please");
        lines_doc->setHtml(QString("<p align = \"center\">%1</p>").arg(prompt_str));
    }
}

void Client::activate(const QVariant &playerId)
{
    setStatus(playerId.toString() == Self->objectName() ? Playing : NotActive);
}

void Client::startGame(const QVariant &)
{
    Sanguosha->registerRoom(this);
    _m_roomState.reset();

    QList<ClientPlayer *> players = findChildren<ClientPlayer *>();
    alive_count = players.count();

    emit game_started();
}

void Client::hpChange(const QVariant &change_str)
{
    JsonArray change = change_str.value<JsonArray>();
    if (change.size() != 3) return;
    if (!JsonUtils::isString(change[0]) || !JsonUtils::isNumber(change[1]) || !JsonUtils::isNumber(change[2])) return;

    QString who = change[0].toString();
    ClientPlayer *player = getPlayer(who);
    int delta = change[1].toInt();

    int nature_index = change[2].toInt();
    int nature = (int)DamageStruct::Normal;
    if (nature_index > 0) nature = nature_index;

    emit hp_changed(who, delta, nature, nature_index == -1);
    if (delta > 0)
        emit log_received(appendLog("#Recover", who, QStringList(), QString(), QString::number(delta)));
    emit log_received(appendLog("#GetHp", who, QStringList(), QString(),
                                QString::number(player->getHp() + delta), QString::number(player->getMaxHp())));
}

void Client::maxhpChange(const QVariant &change_str)
{
    JsonArray change = change_str.value<JsonArray>();
    if (change.size() != 2) return;
    if (!JsonUtils::isString(change[0]) || !JsonUtils::isNumber(change[1])) return;

    QString who = change[0].toString();
    int delta = change[1].toInt();
    emit maxhp_changed(who, delta);
}

void Client::setStatus(Status status)
{
    Status old_status = this->status;
    this->status = status;
    if (status == Client::Playing)
        _m_roomState.setCurrentCardUseReason(CardUseStruct::CARD_USE_REASON_PLAY);
    else if (status == Responding)
        _m_roomState.setCurrentCardUseReason(CardUseStruct::CARD_USE_REASON_RESPONSE);
    else if (status == RespondingUse)
        _m_roomState.setCurrentCardUseReason(CardUseStruct::CARD_USE_REASON_RESPONSE_USE);
    else
        _m_roomState.setCurrentCardUseReason(CardUseStruct::CARD_USE_REASON_UNKNOWN);
    emit status_changed(old_status, status);
}

Client::Status Client::getStatus() const
{
    return status;
}

void Client::cardLimitation(const QVariant &limit)
{
    JsonArray args = limit.value<JsonArray>();
    if (args.size() != 4) return;

    bool set = args[0].toBool();
    bool single_turn = args[3].toBool();
    if (args[1].isNull() && args[2].isNull()) {
        Self->clearCardLimitation(single_turn);
    } else {
        if (!JsonUtils::isString(args[1]) || !JsonUtils::isString(args[2])) return;
        QString limit_list = args[1].toString();
        QString pattern = args[2].toString();
        if (set)
            Self->setCardLimitation(limit_list, pattern, single_turn);
        else
            Self->removeCardLimitation(limit_list, pattern);
    }
}

void Client::setNullification(const QVariant &str)
{
    if (!JsonUtils::isString(str)) return;
    QString astr = str.toString();
    if (astr != ".") {
        if (m_noNullificationTrickName == ".") {
            m_noNullificationThisTime = false;
            m_noNullificationTrickName = astr;
            emit nullification_asked(true);
        }
    } else {
        m_noNullificationThisTime = false;
        m_noNullificationTrickName = ".";
        emit nullification_asked(false);
    }
}

void Client::enableSurrender(const QVariant &enabled)
{
    if (!JsonUtils::isBool(enabled)) return;
    bool en = enabled.toBool();
    emit surrender_enabled(en);
}

void Client::exchangeKnownCards(const QVariant &players)
{
    JsonArray args = players.value<JsonArray>();
    if (args.size() != 2 || !JsonUtils::isString(args[0]) || !JsonUtils::isString(args[1])) return;
    ClientPlayer *a = getPlayer(args[0].toString()), *b = getPlayer(args[1].toString());
    QList<int> a_known, b_known;
    foreach (const Card *card, a->getHandcards())
        a_known << card->getId();
    foreach (const Card *card, b->getHandcards())
        b_known << card->getId();
    a->setCards(b_known);
    b->setCards(a_known);
}

void Client::setKnownCards(const QVariant &set_str)
{
    JsonArray set = set_str.value<JsonArray>();
    if (set.size() != 2) return;
    QString name = set[0].toString();
    ClientPlayer *player = getPlayer(name);
    if (player == NULL) return;
    QList<int> ids;
    JsonUtils::tryParse(set[1], ids);
    player->setCards(ids);
}

void Client::viewGenerals(const QVariant &arg)
{
    JsonArray args = arg.value<JsonArray>();
    if (args.size() != 2 || !JsonUtils::isString(args[0])) return;
    QString reason = args[0].toString();
    QStringList names;
    if (!JsonUtils::tryParse(args[1], names)) return;
    emit generals_viewed(reason, names);
}

Replayer *Client::getReplayer() const
{
    return replayer;
}

QString Client::getPlayerName(const QString &str)
{
    QRegExp rx("sgs\\d+");
    QString general_name;
    if (rx.exactMatch(str)) {
        ClientPlayer *player = getPlayer(str);
        if (!player) return QString();
        general_name = player->getGeneralName();
        general_name = Sanguosha->translate(general_name);
        if (player->getGeneral2())
            general_name.append("/" + Sanguosha->translate(player->getGeneral2Name()));
        if (ServerInfo.EnableSame || player->getGeneralName() == "anjiang")
            general_name = QString("%1[%2]").arg(general_name).arg(player->getSeat());
        return general_name;
    } else
        return Sanguosha->translate(str);
}

QString Client::getSkillNameToInvoke() const
{
    return skill_to_invoke;
}

QString Client::getSkillNameToInvokeData() const
{
    return skill_to_invoke_data;
}

void Client::onPlayerInvokeSkill(bool invoke)
{
    if (skill_name == "surrender")
        replyToServer(S_COMMAND_SURRENDER, invoke);
    else
        replyToServer(S_COMMAND_INVOKE_SKILL, invoke);
    setStatus(NotActive);
}

QString Client::setPromptList(const QStringList &texts)
{
    QString prompt = Sanguosha->translate(texts.at(0));
    if (texts.length() >= 2)
        prompt.replace("%src", getPlayerName(texts.at(1)));

    if (texts.length() >= 3)
        prompt.replace("%dest", getPlayerName(texts.at(2)));

    if (texts.length() >= 5) {
        QString arg2 = Sanguosha->translate(texts.at(4));
        prompt.replace("%arg2", arg2);
    }

    if (texts.length() >= 4) {
        QString arg = Sanguosha->translate(texts.at(3));
        prompt.replace("%arg", arg);
    }

    prompt_doc->setHtml(prompt);
    return prompt;
}

void Client::commandFormatWarning(const QString &str, const QRegExp &rx, const char *command)
{
    QString text = tr("The argument (%1) of command %2 does not conform the format %3")
        .arg(str).arg(command).arg(rx.pattern());
    QMessageBox::warning(NULL, tr("Command format warning"), text);
}

QString Client::_processCardPattern(const QString &pattern)
{
    const QChar c = pattern.at(pattern.length() - 1);
    if (c == '!' || c.isNumber())
        return pattern.left(pattern.length() - 1);

    return pattern;
}

void Client::askForCardOrUseCard(const QVariant &cardUsage)
{
    JsonArray usage = cardUsage.value<JsonArray>();
    if (usage.size() < 2 || !JsonUtils::isString(usage[0]) || !JsonUtils::isString(usage[1]))
        return;
    QString card_pattern = usage[0].toString();
    _m_roomState.setCurrentCardUsePattern(card_pattern);
    QString textsString = usage[1].toString();
    QStringList texts = textsString.split(":");
    int index = -1;
    if (usage.size() >= 4 && JsonUtils::isNumber(usage[3]) && usage[3].toInt() > 0)
        index = usage[3].toInt();

    if (texts.isEmpty())
        return;
    else
        setPromptList(texts);

    if (card_pattern.endsWith("!"))
        m_isDiscardActionRefusable = false;
    else
        m_isDiscardActionRefusable = true;

    QString temp_pattern = _processCardPattern(card_pattern);
    QRegExp rx("^@@?(\\w+)(-card)?$");
    if (rx.exactMatch(temp_pattern)) {
        QString skill_name = rx.capturedTexts().at(1);
        const Skill *skill = Sanguosha->getSkill(skill_name);
        if (skill) {
            QString text = prompt_doc->toHtml();
            text.append(tr("<br/> <b>Notice</b>: %1<br/>").arg(skill->getNotice(index)));
            prompt_doc->setHtml(text);
        }
    }

    Status status = Responding;
    m_respondingUseFixedTarget = NULL;
    if (usage.size() >= 3 && JsonUtils::isNumber(usage[2])) {
        Card::HandlingMethod method = (Card::HandlingMethod)(usage[2].toInt());
        switch (method) {
        case Card::MethodDiscard: status = RespondingForDiscard; break;
        case Card::MethodUse: status = RespondingUse; break;
        case Card::MethodResponse: status = Responding; break;
        default: status = RespondingNonTrigger; break;
        }
    }
    setStatus(status);
}

void Client::askForSkillInvoke(const QVariant &arg)
{
    JsonArray args = arg.value<JsonArray>();
    if (!JsonUtils::isStringArray(args, 0, 1)) return;

    QString skill_name = args[0].toString();
    QString data = args[1].toString();

    skill_to_invoke = skill_name;
    skill_to_invoke_data = data;

    QString text;
    if (data.isEmpty()) {
        text = tr("Do you want to invoke skill [%1] ?").arg(Sanguosha->translate(skill_name));
        prompt_doc->setHtml(text);
    } else if (data.startsWith("playerdata:")) {
        QString name = getPlayerName(data.split(":").last());
        text = tr("Do you want to invoke skill [%1] to %2 ?").arg(Sanguosha->translate(skill_name)).arg(name);
        prompt_doc->setHtml(text);
    } else if (skill_name.startsWith("cv_")) {
        setPromptList(QStringList() << "@sp_convert" << QString() << QString() << data);
    } else {
        QStringList texts = data.split(":");
        text = QString("%1:%2").arg(skill_name).arg(texts.first());
        texts.replace(0, text);
        setPromptList(texts);
    }

    setStatus(AskForSkillInvoke);
}

void Client::onPlayerMakeChoice()
{
    QString option = sender()->objectName();
    replyToServer(S_COMMAND_MULTIPLE_CHOICE, option);
    setStatus(NotActive);
}

void Client::askForSurrender(const QVariant &initiator)
{
    if (!JsonUtils::isString(initiator)) return;

    QString text = tr("%1 initiated a vote for disadvataged side to claim "
        "capitulation. Click \"OK\" to surrender or \"Cancel\" to resist.")
        .arg(Sanguosha->translate(initiator.toString()));
    text.append(tr("<br/> <b>Notice</b>: if all people on your side decides to surrender. "
        "You'll lose this game."));
    skill_name = "surrender";

    prompt_doc->setHtml(text);
    setStatus(AskForSkillInvoke);
}

void Client::askForLuckCard(const QVariant &)
{
    skill_to_invoke = "luck_card";
    skill_to_invoke_data = QString();
    prompt_doc->setHtml(tr("Do you want to use the luck card?"));
    setStatus(AskForSkillInvoke);
}

void Client::askForNullification(const QVariant &arg)
{
    JsonArray args = arg.value<JsonArray>();
    if (args.size() != 3 || !JsonUtils::isString(args[0])
        || !(args[1].isNull() || JsonUtils::isString(args[1]))
        || !JsonUtils::isString(args[2]))
        return;

    QString trick_name = args[0].toString();
    const QVariant &source_name = args[1];
    ClientPlayer *target_player = getPlayer(args[2].toString());

    if (!target_player || !target_player->getGeneral()) return;

    ClientPlayer *source = NULL;
    if (!source_name.isNull())
        source = getPlayer(source_name.toString());

    const Card *trick_card = Sanguosha->findChild<const Card *>(trick_name);
    if (Config.value("NeverNullifyMyTrick").toBool() && source == Self) {
        if (trick_card->isKindOf("SingleTargetTrick") || trick_card->isKindOf("IronChain")) {
            onPlayerResponseCard(NULL);
            return;
        }
    }
    if (m_noNullificationThisTime && m_noNullificationTrickName == trick_name) {
        if (trick_card->isKindOf("AOE") || trick_card->isKindOf("GlobalEffect")) {
            onPlayerResponseCard(NULL);
            return;
        }
    }

    if (source == NULL) {
        prompt_doc->setHtml(tr("Do you want to use nullification to trick card %1 from %2?")
            .arg(Sanguosha->translate(trick_card->objectName()))
            .arg(getPlayerName(target_player->objectName())));
    } else {
        prompt_doc->setHtml(tr("%1 used trick card %2 to %3 <br>Do you want to use nullification?")
            .arg(getPlayerName(source->objectName()))
            .arg(Sanguosha->translate(trick_name))
            .arg(getPlayerName(target_player->objectName())));
    }

    _m_roomState.setCurrentCardUsePattern("nullification");
    m_isDiscardActionRefusable = true;
    m_respondingUseFixedTarget = NULL;
    setStatus(RespondingUse);
}

void Client::onPlayerChooseCard(int card_id)
{
    QVariant reply;
    if (card_id != -2)
        reply = card_id;
    replyToServer(S_COMMAND_CHOOSE_CARD, reply);
    setStatus(NotActive);
}

void Client::onPlayerChoosePlayer(const Player *player)
{
    if (player == NULL && !m_isDiscardActionRefusable)
        player = findChild<const Player *>(players_to_choose.first());

    replyToServer(S_COMMAND_CHOOSE_PLAYER, (player == NULL) ? QVariant() : player->objectName());
    setStatus(NotActive);
}

void Client::trust()
{
    notifyServer(S_COMMAND_TRUST);

    if (Self->getState() == "trust")
        Sanguosha->playSystemAudioEffect("untrust");
    else
        Sanguosha->playSystemAudioEffect("trust");

    setStatus(NotActive);
}

void Client::requestSurrender()
{
    requestServer(S_COMMAND_SURRENDER);
    setStatus(NotActive);
}

void Client::speakToServer(const QString &text)
{
    if (text.isEmpty())
        return;

    QByteArray data = text.toUtf8().toBase64();
    notifyServer(S_COMMAND_SPEAK, QString(data));
}

void Client::addHistory(const QVariant &history)
{
    JsonArray args = history.value<JsonArray>();
    if (args.size() != 2 || !JsonUtils::isString(args[0]) || !JsonUtils::isNumber(args[1])) return;

    QString add_str = args[0].toString();
    int times = args[1].toInt();
    if (add_str == "pushPile") {
        emit card_used();
        return;
    } else if (add_str == ".") {
        Self->clearHistory();
        return;
    }

    if (times == 0)
        Self->clearHistory(add_str);
    else
        Self->addHistory(add_str, times);
}

int Client::alivePlayerCount() const
{
    return alive_count;
}

ClientPlayer *Client::getPlayer(const QString &name)
{
    if (name == Self->objectName() ||
        name == QSanProtocol::S_PLAYER_SELF_REFERENCE_ID)
        return Self;
    else
        return findChild<ClientPlayer *>(name);
}

bool Client::save(const QString &filename) const
{
    if (recorder)
        return recorder->save(filename);
    else
        return false;
}

QList<QByteArray> Client::getRecords() const
{
    if (recorder)
        return recorder->getRecords();
    else
        return QList<QByteArray>();
}

QString Client::getReplayPath() const
{
    if (replayer)
        return replayer->getPath();
    else
        return QString();
}

QTextDocument *Client::getLinesDoc() const
{
    return lines_doc;
}

QTextDocument *Client::getPromptDoc() const
{
    return prompt_doc;
}

void Client::resetPiles(const QVariant &)
{
    discarded_list.clear();
    swap_pile++;
    updatePileNum();
    emit pile_reset();
}

void Client::setPileNumber(const QVariant &pile_str)
{
    if (!pile_str.canConvert<int>()) return;
    pile_num = pile_str.toInt();
    updatePileNum();
}

void Client::synchronizeDiscardPile(const QVariant &discard_pile)
{
    if (!discard_pile.canConvert<JsonArray>())
        return;

    if (JsonUtils::isNumberArray(discard_pile, 0, discard_pile.value<JsonArray>().length() - 1))
        return;

    QList<int> discard;
    if (JsonUtils::tryParse(discard_pile, discard)) {
        foreach (int id, discard) {
            const Card *card = Sanguosha->getCard(id);
            discarded_list.append(card);
        }
        updatePileNum();
    }
}

void Client::setCardFlag(const QVariant &pattern_str)
{
    JsonArray pattern = pattern_str.value<JsonArray>();
    if (pattern.size() != 2 || !JsonUtils::isNumber(pattern[0]) || !JsonUtils::isString(pattern[1])) return;

    int id = pattern[0].toInt();
    QString flag = pattern[1].toString();

    Card *card = Sanguosha->getCard(id);
    if (card != NULL)
        card->setFlags(flag);
}

void Client::updatePileNum()
{
    QString pile_str = tr("Draw pile: <b>%1</b>, discard pile: <b>%2</b>, swap times: <b>%3</b>")
        .arg(pile_num).arg(discarded_list.length()).arg(swap_pile);
    lines_doc->setHtml(QString("<font color='%1'><p align = \"center\">" + pile_str + "</p></font>").arg("white"));
}

void Client::askForDiscard(const QVariant &reqvar)
{
    JsonArray req = reqvar.value<JsonArray>();
    if (req.size() != 6 || !JsonUtils::isNumber(req[0]) || !JsonUtils::isNumber(req[1]) || !JsonUtils::isBool(req[2])
        || !JsonUtils::isBool(req[3]) || !JsonUtils::isString(req[4]) || !JsonUtils::isString(req[5]))
        return;


    discard_num = req[0].toInt();
    min_num = req[1].toInt();
    m_isDiscardActionRefusable = req[2].toBool();
    m_canDiscardEquip = req[3].toBool();
    QString prompt = req[4].toString();
    QString pattern = req[5].toString();
    if (pattern.isEmpty()) pattern = ".";
    m_cardDiscardPattern = pattern;

    if (prompt.isEmpty()) {
        if (m_canDiscardEquip)
            prompt = tr("Please discard %1 card(s), include equip").arg(discard_num);
        else
            prompt = tr("Please discard %1 card(s), only hand cards is allowed").arg(discard_num);
        if (min_num < discard_num) {
            prompt.append("<br/>");
            prompt.append(tr("%1 %2 cards(s) are required at least").arg(min_num).arg(m_canDiscardEquip ? "" : tr("hand")));
        }
        prompt_doc->setHtml(prompt);
    } else {
        QStringList texts = prompt.split(":");
        if (texts.length() < 4) {
            while (texts.length() < 3)
                texts.append(QString());
            texts.append(QString::number(discard_num));
        }
        setPromptList(texts);
    }

    setStatus(Discarding);
}

void Client::askForExchange(const QVariant &exchange)
{
    JsonArray args = exchange.value<JsonArray>();
    if (args.size() != 6 || !JsonUtils::isNumber(args[0]) || !JsonUtils::isNumber(args[1]) || !JsonUtils::isBool(args[2])
        || !JsonUtils::isString(args[3]) || !JsonUtils::isBool(args[4]) || !JsonUtils::isString(args[5]))
        return;

    discard_num = args[0].toInt();
    min_num = args[1].toInt();
    m_canDiscardEquip = args[2].toBool();
    QString prompt = args[3].toString();
    m_isDiscardActionRefusable = args[4].toBool();

    QString pattern = args[5].toString();

    if (pattern.isEmpty()) pattern = ".";
    m_cardDiscardPattern = pattern;

    if (prompt.isEmpty()) {
        prompt = tr("Please give %1 cards to exchange").arg(discard_num);
        prompt_doc->setHtml(prompt);
    } else {
        QStringList texts = prompt.split(":");
        if (texts.length() < 4) {
            while (texts.length() < 3)
                texts.append(QString());
            texts.append(QString::number(discard_num));
        }
        setPromptList(texts);
    }
    setStatus(Exchanging);
}

void Client::gameOver(const QVariant &arg)
{
    disconnectFromHost();
    m_isGameOver = true;
    setStatus(Client::NotActive);

    JsonArray args = arg.value<JsonArray>();
    if (args.size() < 2)
        return;

    QString winner = args[0].toString();
    QStringList roles;
    foreach (const QVariant &role, args[1].value<JsonArray>())
        roles << role.toString();


    Q_ASSERT(roles.length() == players.length());

    for (int i = 0; i < roles.length(); i++) {
        QString name = players.at(i)->objectName();
        getPlayer(name)->setRole(roles.at(i));
    }

    if (winner == ".") {
        emit standoff();
        Sanguosha->unregisterRoom();
        return;
    }

    QT_WARNING_DISABLE_DEPRECATED
    QSet<QString> winners = winner.split("+").toSet();
    foreach (const ClientPlayer *player, players) {
        QString role = player->getRole();
        bool win = winners.contains(player->objectName()) || winners.contains(role);

        ClientPlayer *p = const_cast<ClientPlayer *>(player);
        p->setProperty("win", win);
    }

    Sanguosha->unregisterRoom();
    emit game_over();
}

void Client::killPlayer(const QVariant &player_name)
{
    if (!JsonUtils::isString(player_name)) return;
    QString name = player_name.toString();

    alive_count--;
    ClientPlayer *player = getPlayer(name);
    if (player == Self) {
        foreach (const Skill *skill, Self->getVisibleSkills())
            emit skill_detached(skill->objectName());
    }
    player->detachAllSkills();

    if (!Self->hasFlag("marshalling")) {
        QString general_name = player->getGeneralName();
        QString last_word = Sanguosha->translate(QString("~%1").arg(general_name));
        if (last_word.startsWith("~")) {
            QStringList origin_generals = general_name.split("_");
            if (origin_generals.length() > 1)
                last_word = Sanguosha->translate(("~") + origin_generals.at(1));
        }

        if (last_word.startsWith("~") && general_name.endsWith("f")) {
            QString origin_general = general_name;
            origin_general.chop(1);
            if (Sanguosha->getGeneral(origin_general))
                last_word = Sanguosha->translate(("~") + origin_general);
        }
        updatePileNum();
    }

    emit player_killed(name);
}

void Client::revivePlayer(const QVariant &player_arg)
{
    if (!JsonUtils::isString(player_arg)) return;

    QString player_name = player_arg.toString();

    alive_count++;
    updatePileNum();
    emit player_revived(player_name);
}


void Client::warn(const QVariant &reason_var)
{
    QString reason = reason_var.toString();
    QString msg;
    if (reason == "GAME_OVER")
        msg = tr("Game is over now");
    else if (reason == "INVALID_FORMAT")
        msg = tr("Invalid signup string");
    else if (reason == "LEVEL_LIMITATION")
        msg = tr("Your level is not enough");
    else
        msg = tr("Unknown warning: %1").arg(reason);

    disconnectFromHost();
    QMessageBox::warning(NULL, tr("Warning"), msg);
}

void Client::askForGeneral(const QVariant &arg)
{
    QStringList generals;
    if (!JsonUtils::tryParse(arg, generals)) return;
    emit generals_got(generals);
    setStatus(ExecDialog);
}

void Client::askForSuit(const QVariant &)
{
    QStringList suits;
    suits << "spade" << "club" << "heart" << "diamond";
    emit suits_got(suits);
    setStatus(ExecDialog);
}

void Client::askForKingdom(const QVariant &)
{
    QStringList kingdoms = Sanguosha->getKingdoms();
    kingdoms.removeOne("god"); // god kingdom does not really exist
    emit kingdoms_got(kingdoms);
    setStatus(ExecDialog);
}

void Client::askForChoice(const QVariant &ask_str)
{
    JsonArray ask = ask_str.value<JsonArray>();
    if (!JsonUtils::isStringArray(ask, 0, 1)) return;
    QString skill_name = ask[0].toString();
    QStringList options = ask[1].toString().split("+");
    emit options_got(skill_name, options);
    setStatus(ExecDialog);
}

void Client::askForCardChosen(const QVariant &ask_str)
{
    JsonArray ask = ask_str.value<JsonArray>();
    if (ask.size() != 6 || !JsonUtils::isStringArray(ask, 0, 2)
        || !JsonUtils::isBool(ask[3]) || !JsonUtils::isNumber(ask[4]))
        return;
    QString player_name = ask[0].toString();
    QString flags = ask[1].toString();
    QString reason = ask[2].toString();
    bool handcard_visible = ask[3].toBool();
    Card::HandlingMethod method = (Card::HandlingMethod)ask[4].toInt();
    ClientPlayer *player = getPlayer(player_name);
    if (player == NULL) return;
    QList<int> disabled_ids;
    JsonUtils::tryParse(ask[5], disabled_ids);
    emit cards_got(player, flags, reason, handcard_visible, method, disabled_ids);
    setStatus(ExecDialog);
}

void Client::setMark(const QVariant &mark_var)
{
    JsonArray mark_str = mark_var.value<JsonArray>();
    if (mark_str.size() != 3) return;
    if (!JsonUtils::isString(mark_str[0]) || !JsonUtils::isString(mark_str[1]) || !JsonUtils::isNumber(mark_str[2])) return;

    QString who = mark_str[0].toString();
    QString mark = mark_str[1].toString();
    int value = mark_str[2].toInt();

    ClientPlayer *player = getPlayer(who);
    player->setMark(mark, value);

    // for all the skills has a ViewAsSkill Effect { RoomScene::detachSkill(const QString &) }
    // this is a DIRTY HACK!!! for we should prevent the ViewAsSkill button been removed temporily by duanchang
    if (mark.startsWith("ViewAsSkill_") && mark.endsWith("Effect") && player == Self && value == 0) {
        QString skill_name = mark.mid(12);
        skill_name.chop(6);

        QString lost_mark = "ViewAsSkill_" + skill_name + "Lost";

        if (!Self->hasSkill(skill_name, true) && Self->getMark(lost_mark) > 0) {
            Self->setMark(lost_mark, 0);
            emit skill_detached(skill_name);
        }
    }
}

void Client::onPlayerChooseSuit()
{
    replyToServer(S_COMMAND_CHOOSE_SUIT, sender()->objectName());
    setStatus(NotActive);
}

void Client::onPlayerChooseKingdom()
{
    replyToServer(S_COMMAND_CHOOSE_KINGDOM, sender()->objectName());
    setStatus(NotActive);
}

void Client::onPlayerDiscardCards(const Card *cards)
{
    if (cards) {
        JsonArray arr;
        foreach(int card_id, cards->getSubcards())
            arr << card_id;
        if (cards->isVirtualCard() && !cards->parent())
            delete cards;
        replyToServer(S_COMMAND_DISCARD_CARD, arr);
    } else {
        replyToServer(S_COMMAND_DISCARD_CARD);
    }

    setStatus(NotActive);
}

void Client::fillAG(const QVariant &cards_str)
{
    JsonArray cards = cards_str.value<JsonArray>();
    if (cards.size() != 2) return;
    QList<int> card_ids, disabled_ids;
    JsonUtils::tryParse(cards[0], card_ids);
    JsonUtils::tryParse(cards[1], disabled_ids);
    emit ag_filled(card_ids, disabled_ids);
}

void Client::takeAG(const QVariant &take_var)
{
    JsonArray take = take_var.value<JsonArray>();
    if (take.size() != 3) return;
    if (!JsonUtils::isNumber(take[1]) || !JsonUtils::isBool(take[2])) return;

    int card_id = take[1].toInt();
    bool move_cards = take[2].toBool();
    const Card *card = Sanguosha->getCard(card_id);

    if (take[0].isNull()) {
        if (move_cards) {
            discarded_list.prepend(card);
            updatePileNum();
        }
        emit ag_taken(NULL, card_id, move_cards);
    } else {
        ClientPlayer *taker = getPlayer(take[0].toString());
        if (move_cards)
            taker->addCard(card, Player::PlaceHand);
        emit ag_taken(taker, card_id, move_cards);
        emit log_received(appendLog("$TakeAG", taker->objectName(), QStringList(), QString::number(card_id)));
    }
}

void Client::clearAG(const QVariant &)
{
    emit ag_cleared();
}

void Client::askForSinglePeach(const QVariant &arg)
{
    JsonArray args = arg.value<JsonArray>();
    if (args.size() != 2 || !JsonUtils::isString(args[0]) || !JsonUtils::isNumber(args[1])) return;

    ClientPlayer *dying = getPlayer(args[0].toString());
    int peaches = args[1].toInt();

    // @todo: anti-cheating of askForSinglePeach is not done yet!!!
    QStringList pattern;
    pattern << "peach";
    if (dying == Self) {
        prompt_doc->setHtml(tr("You are dying, please provide %1 peach(es)(or analeptic) to save yourself").arg(peaches));
        pattern << "analeptic";
    } else {
        QString dying_general = getPlayerName(dying->objectName());
        prompt_doc->setHtml(tr("%1 is dying, please provide %2 peach(es) to save him").arg(dying_general).arg(peaches));
    }
    if (Self->getMark("Global_PreventPeach") > 0) {
        bool has_skill = false;
        foreach (const Skill *skill, Self->getVisibleSkillList(true)) {
            const ViewAsSkill *view_as_skill = ViewAsSkill::parseViewAsSkill(skill);
            if (view_as_skill && view_as_skill->isAvailable(Self, CardUseStruct::CARD_USE_REASON_RESPONSE_USE, pattern.join("+"))) {
                has_skill = true;
                break;
            }
        }
        if (!has_skill) {
            pattern.removeOne("peach");
            if (pattern.isEmpty()) {
                onPlayerResponseCard(NULL);
                return;
            }
        } else {
            Self->setFlags("Client_PreventPeach");
            Self->setCardLimitation("use", "Peach");
        }
    }
    _m_roomState.setCurrentCardUsePattern(pattern.join("+"));
    m_isDiscardActionRefusable = true;
    m_respondingUseFixedTarget = dying;
    setStatus(RespondingUse);
}

void Client::askForCardShow(const QVariant &requestor)
{
    if (!JsonUtils::isString(requestor)) return;
    QString name = Sanguosha->translate(requestor.toString());
    prompt_doc->setHtml(tr("%1 request you to show one hand card").arg(name));

    _m_roomState.setCurrentCardUsePattern(".");
    setStatus(AskForShowOrPindian);
}

void Client::askForAG(const QVariant &arg)
{
    if (!JsonUtils::isBool(arg)) return;
    m_isDiscardActionRefusable = arg.toBool();
    setStatus(AskForAG);
}

void Client::onPlayerChooseAG(int card_id)
{
    replyToServer(S_COMMAND_AMAZING_GRACE, card_id);
    setStatus(NotActive);
}

QList<const ClientPlayer *> Client::getPlayers() const
{
    return players;
}

void Client::alertFocus()
{
    if (Self->getPhase() == Player::Play)
        QApplication::alert(QApplication::focusWidget());
}

void Client::showCard(const QVariant &show_str)
{
    JsonArray show = show_str.value<JsonArray>();
    if (show.size() != 2 || !JsonUtils::isString(show[0]) || !JsonUtils::isNumber(show[1]))
        return;

    QString player_name = show[0].toString();
    int card_id = show[1].toInt();

    ClientPlayer *player = getPlayer(player_name);
    if (player != Self)
        player->addKnownHandCard(Sanguosha->getCard(card_id));

    emit card_shown(player_name, card_id);
}

void Client::attachSkill(const QVariant &skill)
{
    if (!JsonUtils::isString(skill)) return;

    QString skill_name = skill.toString();
    Self->acquireSkill(skill_name);
    emit skill_attached(skill_name);
}

void Client::askForAssign(const QVariant &)
{
    emit assign_asked();
}

void Client::onPlayerAssignRole(const QList<QString> &names, const QList<QString> &roles)
{
    Q_ASSERT(names.size() == roles.size());

    JsonArray reply;
    reply << JsonUtils::toJsonArray(names) << JsonUtils::toJsonArray(roles);

    replyToServer(S_COMMAND_CHOOSE_ROLE, reply);
}

void Client::askForGuanxing(const QVariant &arg)
{
    JsonArray args = arg.value<JsonArray>();
    if (args.isEmpty())
        return;

    JsonArray deck = args[0].value<JsonArray>();
    bool single_side = args[1].toBool();
    QList<int> card_ids;
    JsonUtils::tryParse(deck, card_ids);

    emit guanxing(card_ids, single_side);
    setStatus(AskForGuanxing);
}

void Client::showAllCards(const QVariant &arg)
{
    JsonArray args = arg.value<JsonArray>();
    if (args.size() != 3 || !JsonUtils::isString(args[0]) || !JsonUtils::isBool(args[1]))
        return;

    ClientPlayer *who = getPlayer(args[0].toString());
    QList<int> card_ids;
    if (!JsonUtils::tryParse(args[2], card_ids)) return;

    if (who) who->setCards(card_ids);

    emit gongxin(card_ids, false, QList<int>());
}

void Client::askForGongxin(const QVariant &args)
{
    JsonArray arg = args.value<JsonArray>();
    if (arg.size() != 4 || !JsonUtils::isString(arg[0]) || !JsonUtils::isBool(arg[1]))
        return;

    ClientPlayer *who = getPlayer(arg[0].toString());
    bool enable_heart = arg[1].toBool();
    QList<int> card_ids;
    if (!JsonUtils::tryParse(arg[2], card_ids)) return;
    QList<int> enabled_ids;
    if (!JsonUtils::tryParse(arg[3], enabled_ids)) return;

    who->setCards(card_ids);

    emit gongxin(card_ids, enable_heart, enabled_ids);
    setStatus(AskForGongxin);
}

void Client::onPlayerReplyGongxin(int card_id)
{
    QVariant reply;
    if (card_id != -1)
        reply = card_id;
    replyToServer(S_COMMAND_SKILL_GONGXIN, reply);
    setStatus(NotActive);
}

void Client::askForPindian(const QVariant &ask_str)
{
    JsonArray ask = ask_str.value<JsonArray>();
    if (!JsonUtils::isStringArray(ask, 0, 1)) return;
    QString from = ask[0].toString();
    if (from == Self->objectName())
        prompt_doc->setHtml(tr("Please play a card for pindian"));
    else {
        QString requestor = getPlayerName(from);
        prompt_doc->setHtml(tr("%1 ask for you to play a card to pindian").arg(requestor));
    }
    _m_roomState.setCurrentCardUsePattern(".");
    setStatus(AskForShowOrPindian);
}

void Client::askForYiji(const QVariant &ask_str)
{
    JsonArray ask = ask_str.value<JsonArray>();
    if (ask.size() != 4 && ask.size() != 5) return;

    JsonArray card_list = ask[0].value<JsonArray>();
    int count = ask[2].toInt();
    m_isDiscardActionRefusable = ask[1].toBool();

    if (ask.size() == 5) {
        QString prompt = ask[4].toString();
        QStringList texts = prompt.split(":");
        if (texts.length() < 4) {
            while (texts.length() < 3)
                texts.append(QString());
            texts.append(QString::number(count));
        }
        setPromptList(texts);
    } else {
        prompt_doc->setHtml(tr("Please distribute %1 cards %2 as you wish")
            .arg(count)
            .arg(m_isDiscardActionRefusable ? QString() : tr("to another player")));
    }

    //@todo: use cards directly rather than the QString
    QStringList card_str;
    foreach (const QVariant &card, card_list)
        card_str << QString::number(card.toInt());

    JsonArray players = ask[3].value<JsonArray>();
    QStringList names;
    JsonUtils::tryParse(players, names);

    _m_roomState.setCurrentCardUsePattern(QString("%1=%2=%3").arg(count).arg(card_str.join("+")).arg(names.join("+")));
    setStatus(AskForYiji);
}

void Client::askForPlayerChosen(const QVariant &players)
{
    JsonArray args = players.value<JsonArray>();
    if (args.size() != 4) return;
    if (!JsonUtils::isString(args[1]) || !args[0].canConvert<JsonArray>() || !JsonUtils::isBool(args[3])) return;

    JsonArray choices = args[0].value<JsonArray>();
    if (choices.size() == 0) return;
    skill_name = args[1].toString();
    players_to_choose.clear();
    for (int i = 0; i < choices.size(); i++)
        players_to_choose.push_back(choices[i].toString());
    m_isDiscardActionRefusable = args[3].toBool();

    QString text;
    QString description = Sanguosha->translate(ClientInstance->skill_name);
    QString prompt = args[2].toString();
    if (!prompt.isEmpty()) {
        QStringList texts = prompt.split(":");
        text = setPromptList(texts);
        if (prompt.startsWith("@") && !description.isEmpty() && description != skill_name)
            text.append(tr("<br/> <b>Source</b>: %1<br/>").arg(description));
    } else {
        text = tr("Please choose a player");
        if (!description.isEmpty() && description != skill_name)
            text.append(tr("<br/> <b>Source</b>: %1<br/>").arg(description));
    }
    prompt_doc->setHtml(text);

    setStatus(AskForPlayerChoose);
}

void Client::onPlayerReplyYiji(const Card *card, const Player *to)
{
    if (!card)
        replyToServer(S_COMMAND_SKILL_YIJI);
    else {
        JsonArray req;
        req << JsonUtils::toJsonArray(card->getSubcards());
        req << to->objectName();
        replyToServer(S_COMMAND_SKILL_YIJI, req);
    }

    setStatus(NotActive);
}

void Client::onPlayerReplyGuanxing(const QList<int> &up_cards, const QList<int> &down_cards)
{
    JsonArray decks;
    decks << JsonUtils::toJsonArray(up_cards);
    decks << JsonUtils::toJsonArray(down_cards);

    replyToServer(S_COMMAND_SKILL_GUANXING, decks);

    setStatus(NotActive);
}

void Client::log(const QVariant &log_str)
{
    QStringList log;

    if (!JsonUtils::tryParse(log_str, log) || log.size() != 6)
        emit log_received(QString());
    else {
        if (log.first().contains("#BasaraReveal"))
            Sanguosha->playSystemAudioEffect("choose-item");
        else if (log.first() == "#Zombify") {
            ClientPlayer *from = getPlayer(log.at(1));
            if (from)
                Sanguosha->playSystemAudioEffect(QString("zombify-%1").arg(from->isMale() ? "male" : "female"));
        } else if (log.first() == "#UseLuckCard") {
            ClientPlayer *from = getPlayer(log.at(1));
            if (from && from != Self)
                from->setHandcardNum(0);
        }
        emit log_received(appendLog(log[0], log[1], log[2].isEmpty() ? QStringList() : log[2].split("+"),
                log[3], log[4], log[5]));
    }
}

QString Client::appendLog(const QString &type, const QString &from_general, const QStringList &tos,
    QString card_str, QString arg, QString arg2)
{
    if (Self->hasFlag("marshalling")) return QString();

    if (type == "$AppendSeparator") {
        return tr("<font color='white'>------------------------------</font>");
    }

#define bold(str, color) QString("<font color='%1'><b>%2</b></font>").arg(QColor(color).name()).arg(str);

    QString from;
    if (!from_general.isEmpty()) {
        from = ClientInstance->getPlayerName(from_general);
        from = bold(from, Qt::green);
    }

    QString to;
    if (!tos.isEmpty()) {
        QStringList to_list;
        foreach(QString to, tos)
            to_list << ClientInstance->getPlayerName(to);
        to = to_list.join(", ");
        to = bold(to, Qt::red);
    }

    QString log;

    if (type.startsWith("$")) {
        QString log_name;
        foreach (QString one_card, card_str.split("+")) {
            const Card *card = NULL;
            if (type == "$JudgeResult" || type == "$PasteCard")
                card = Sanguosha->getCard(one_card.toInt());
            else
                card = Sanguosha->getEngineCard(one_card.toInt());
            if (card) {
                if (log_name.isEmpty())
                    log_name = card->getLogName();
                else
                    log_name += ", " + card->getLogName();
            }
        }
        log_name = bold(log_name, Qt::yellow);

        log = Sanguosha->translate(type);
        log.replace("%from", from);
        log.replace("%to", to);
        log.replace("%card", log_name);

        if (!arg2.isEmpty()) {
            arg2 = bold(Sanguosha->translate(arg2), Qt::yellow);
            log.replace("%arg2", arg2);
        }

        if (!arg.isEmpty()) {
            arg = bold(Sanguosha->translate(arg), Qt::yellow);
            log.replace("%arg", arg);
        }

        log = QString("<font color='%2'>%1</font>").arg(log).arg("white");

        return log;
    }

    if (type.startsWith("#UseCard") && !card_str.isEmpty() && !from_general.isEmpty()) {
        const Card *card = Card::Parse(card_str);
        if (card == NULL) return QString();

        QString card_name = card->getLogName();
        card_name = bold(card_name, Qt::yellow);

        QString reason = tr("using");
        if (type.endsWith("_Resp")) reason = tr("playing");
        if (type.endsWith("_Recast")) reason = tr("recasting");

        if (card->isVirtualCard()) {
            QString skill_name = Sanguosha->translate(card->getSkillName());
            skill_name = bold(skill_name, Qt::yellow);
            bool eff = (card->getSkillName(false) != card->getSkillName(true));
            QString meth = eff ? tr("carry out") : tr("use skill");
            QString suffix = eff ? tr("effect") : "";

            QList<int> card_ids = card->getSubcards();
            QStringList subcard_list;
            foreach (int card_id, card_ids) {
                const Card *subcard = Sanguosha->getEngineCard(card_id);
                subcard_list << bold(subcard->getLogName(), Qt::yellow);
            }

            QString subcard_str = subcard_list.join(", ");
            if (card->getTypeId() == Card::TypeSkill && !card->isKindOf("YanxiaoCard")) {
                const SkillCard *skill_card = qobject_cast<const SkillCard *>(card);
                if (subcard_list.isEmpty() || !skill_card->willThrow())
                    log = tr("%from %2 [%1] %3").arg(skill_name).arg(meth).arg(suffix);
                else
                    log = tr("%from %3 [%1] %4, and the cost is %2").arg(skill_name).arg(subcard_str).arg(meth).arg(suffix);
            } else {
                if (subcard_list.isEmpty() || card->getSkillName().contains("guhuo"))
                    log = tr("%from %4 [%1] %5, %3 [%2]").arg(skill_name).arg(card_name).arg(reason).arg(meth).arg(suffix);
                else
                    log = tr("%from %5 [%1] %6 %4 %2 as %3").arg(skill_name).arg(subcard_str).arg(card_name).arg(reason).arg(meth).arg(suffix);
            }

            delete card;
        } else if (card->getSkillName() != QString()) {
            const Card *real = Sanguosha->getEngineCard(card->getEffectiveId());
            QString skill_name = Sanguosha->translate(card->getSkillName());
            skill_name = bold(skill_name, Qt::yellow);

            QString subcard_str = bold(real->getLogName(), Qt::yellow);
            if (card->isKindOf("DelayedTrick"))
                log = tr("%from %5 [%1] %6 %4 %2 as %3").arg(skill_name).arg(subcard_str).arg(card_name).arg(reason).arg(tr("use skill")).arg(QString());
            else
                log = tr("Due to the effect of [%1], %from %4 %2 as %3").arg(skill_name).arg(subcard_str).arg(card_name).arg(reason);
        } else
            log = tr("%from %2 %1").arg(card_name).arg(reason);

        if (!to.isEmpty()) log.append(tr(", target is %to"));
    } else {
        log = Sanguosha->translate(type);
        const Card *card = Card::Parse(card_str);
        if (card) {
            QString card_name = card->getLogName();
            card_name = bold(card_name, Qt::yellow);
            log.replace("%card", card_name);
        }
    }

    log.replace("%from", from);
    log.replace("%to", to);

    if (!arg2.isEmpty()) {
        arg2 = bold(Sanguosha->translate(arg2), Qt::yellow);
        log.replace("%arg2", arg2);
    }

    if (!arg.isEmpty()) {
        arg = bold(Sanguosha->translate(arg), Qt::yellow);
        log.replace("%arg", arg);
    }

    log = QString("<font color='%2'>%1</font>").arg(log).arg("white");
#undef bold
    return log;
    // QString final_log = append(log);
    // if (type == "#Guhuo" || type == "#GuhuoNoTarget")
    //    RoomSceneInstance->setGuhuoLog(final_log);
}


void Client::speak(const QVariant &speak)
{
    if (!speak.canConvert<JsonArray>()) {
        qDebug() << speak;
        return;
    }

    JsonArray args = speak.value<JsonArray>();
    QString who = args[0].toString();
    QString text = QString::fromUtf8(QByteArray::fromBase64(args[1].toString().toLatin1()));

    static const QString prefix("<img width=14 height=14 src='../../../image/system/chatface/");
    static const QString suffix(".png'></img>");
    text = text.replace("<#", prefix).replace("#>", suffix);

    if (who == ".") {
        QString line = tr("<font color='red'>System: %1</font>").arg(text);
        emit line_spoken(QString("<p style=\"margin:3px 2px;\">%1</p>").arg(line));
        return;
    }

    emit player_speak(who, QString("<p style=\"margin:3px 2px;\">%1</p>").arg(text));
    const ClientPlayer *from = getPlayer(who);

    QString title;
    if (from) {
        title = from->getGeneralName();
        title = Sanguosha->translate(title);
        title.append(QString("(%1)").arg(from->screenName()));
    }

    title = QString("<b>%1</b>").arg(title);

    QString line = tr("<font color='%1'>[%2] said: %3 </font>")
        .arg("white").arg(title).arg(text);

    emit line_spoken(QString("<p style=\"margin:3px 2px;\">%1</p>").arg(line));
}

void Client::moveFocus(const QVariant &focus)
{
    Countdown countdown;

    JsonArray args = focus.value<JsonArray>();
    Q_ASSERT(!args.isEmpty());

    QStringList players;
    JsonArray json_players = args[0].value<JsonArray>();
    if (!json_players.isEmpty()) {
        JsonUtils::tryParse(json_players, players);
    } else {
        foreach (const ClientPlayer *player, this->players) {
            if (player->isAlive()) {
                players << player->objectName();
            }
        }
    }

    if (args.size() == 1) {//default countdown
        countdown.type = Countdown::S_COUNTDOWN_USE_SPECIFIED;
        countdown.current = 0;
        countdown.max = ServerInfo.getCommandTimeout(S_COMMAND_UNKNOWN, S_CLIENT_INSTANCE);
    } else // focus[1] is the moveFocus reason, which is unused for now.
        countdown.tryParse(args[2]);
    emit focus_moved(players, countdown);
}

void Client::setEmotion(const QVariant &set_str)
{
    JsonArray set = set_str.value<JsonArray>();
    if (set.size() != 2) return;
    if (!JsonUtils::isStringArray(set, 0, 1)) return;

    QString target_name = set[0].toString();
    QString emotion = set[1].toString();

    emit emotion_set(target_name, emotion);
}

void Client::skillInvoked(const QVariant &arg)
{
    JsonArray args = arg.value<JsonArray>();
    if (!JsonUtils::isStringArray(args, 0, 1)) return;
    emit skill_invoked(args[1].toString(), args[0].toString());
}

void Client::animate(const QVariant &animate_str)
{
    JsonArray animate = animate_str.value<JsonArray>();
    if (animate.size() != 3 || !JsonUtils::isNumber(animate[0])
        || !JsonUtils::isString(animate[1]) || !JsonUtils::isString(animate[2]))
        return;

    QStringList args;
    args << animate[1].toString() << animate[2].toString();
    int name = animate[0].toInt();
    emit animated(name, args);
}

void Client::setFixedDistance(const QVariant &set_str)
{
    JsonArray set = set_str.value<JsonArray>();
    if (set.size() != 4
        || !JsonUtils::isString(set[0])
        || !JsonUtils::isString(set[1])
        || !JsonUtils::isNumber(set[2])
        || !JsonUtils::isBool(set[3])) return;

    ClientPlayer *from = getPlayer(set[0].toString());
    ClientPlayer *to = getPlayer(set[1].toString());
    int distance = set[2].toInt();
    bool isSet = set[3].toBool();

    if (from && to) {
        if (isSet)
            from->setFixedDistance(to, distance);
        else
            from->removeFixedDistance(to, distance);
    }
}

void Client::setAttackRangePair(const QVariant &set_arg)
{
    JsonArray set = set_arg.value<JsonArray>();
    if (!JsonUtils::isString(set[0]) || !JsonUtils::isString(set[1]) || !JsonUtils::isBool(set[2]))
        return;


    ClientPlayer *from = getPlayer(set[0].toString());
    ClientPlayer *to = getPlayer(set[1].toString());
    bool isSet = set[2].toBool();

    if (from && to) {
        if (isSet)
            from->insertAttackRangePair(to);
        else
            from->removeAttackRangePair(to);
    }
}

void Client::updateStateItem(const QVariant &state)
{
    if (!JsonUtils::isString(state)) return;
    emit role_state_changed(state.toString());
}

void Client::setAvailableCards(const QVariant &pile)
{
    QList<int> drawPile;
    if (JsonUtils::tryParse(pile, drawPile))
        available_cards = drawPile;
}

void Client::updateSkill(const QVariant &skill_name)
{
    if (!JsonUtils::isString(skill_name))
        return;

    emit skill_updated(skill_name.toString());
}

REGISTER_UNCREATABLE_QMLTYPE("Sanguosha", 1, 0, Client)
