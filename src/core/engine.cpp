﻿#include "engine.h"
#include "card.h"
#include "client.h"
#include "ai.h"
#include "settings.h"
#include "scenario.h"
#include "lua.hpp"
#include "banpair.h"
#include "audio.h"
#include "protocol.h"
#include "structs.h"
#include "lua-wrapper.h"
#include "room-state.h"
#include "clientstruct.h"
#include "util.h"
#include "exppattern.h"
#include "wrapped-card.h"
#include "room.h"
#include "miniscenarios.h"

#include "guandu-scenario.h"
#include "couple-scenario.h"
#include "boss-mode-scenario.h"
#include "zombie-scenario.h"
#include "fancheng-scenario.h"

Engine *Sanguosha = NULL;

int Engine::getMiniSceneCounts()
{
    return m_miniScenes.size();
}

void Engine::_loadMiniScenarios()
{
    static bool loaded = false;
    if (loaded) return;
    int i = 1;
    while (true) {
        if (!QFile::exists(QString("etc/customScenes/%1.txt").arg(QString::number(i))))
            break;

        QString sceneKey = QString(MiniScene::S_KEY_MINISCENE).arg(QString::number(i));
        m_miniScenes[sceneKey] = new LoadedScenario(QString::number(i));
        i++;
    }
    loaded = true;
}

void Engine::_loadModScenarios()
{
    addScenario(new GuanduScenario());
    addScenario(new CoupleScenario());
    addScenario(new FanchengScenario());
    addScenario(new ZombieScenario());
    addScenario(new ImpasseScenario());
}

void Engine::addPackage(const QString &name)
{
    Package *pack = PackageAdder::packages()[name];
    if (pack)
        addPackage(pack);
    else
        qWarning("Package %s cannot be loaded!", qPrintable(name));
}

struct ManualSkill
{
    ManualSkill(const Skill *skill)
        : skill(skill),
          baseName(skill->objectName().split("_").last())
    {
        static const QString prefixes[] = { "boss", "gd", "jg", "jsp", "kof", "neo", "nos", "ol", "sp", "tw", "vs", "yt", "diy" };

        for (unsigned long int i = 0; i < sizeof(prefixes) / sizeof(QString); ++i) {
            QString prefix = prefixes[i];
            if (baseName.startsWith(prefix))
                baseName.remove(0, prefix.length());
        }

        QTextCodec *codec = QTextCodec::codecForName("GBK");
        translatedBytes = codec->fromUnicode(Sanguosha->translate(skill->objectName()));

        printf("%s:%d", skill->objectName().toLocal8Bit().constData(), translatedBytes.length());
    }

    const Skill *skill;
    QString baseName;
    QByteArray translatedBytes;
    QList<const General *> relatedGenerals;
};

static bool nameLessThan(const ManualSkill *skill1, const ManualSkill *skill2)
{
    return skill1->baseName < skill2->baseName;
}

static bool translatedNameLessThan(const ManualSkill *skill1, const ManualSkill *skill2)
{
    return skill1->translatedBytes < skill2->translatedBytes;
}

class ManualSkillList
{
public:
    ManualSkillList()
    {

    }

    ~ManualSkillList()
    {
        foreach (ManualSkill *manualSkill, m_skills)
            delete manualSkill;
    }

    void insert(const Skill *skill, const General *owner)
    {
        bool exist = false;
        foreach (ManualSkill *manualSkill, m_skills) {
            if (skill == manualSkill->skill) {
                exist = true;
                manualSkill->relatedGenerals << owner;
            }
        }

        if (!exist) {
            ManualSkill *manualSkill = new ManualSkill(skill);
            manualSkill->relatedGenerals << owner;
            m_skills << manualSkill;
        }
    }

    void insert(QList<const Skill *>skills, const General *owner) {
        foreach (const Skill *skill, skills)
            insert(skill, owner);
    }

    void insert(ManualSkill *skill)
    {
        m_skills << skill;
    }

    void clear()
    {
        m_skills.clear();
    }

    bool isEmpty() const
    {
        return m_skills.isEmpty();
    }

    void sortByName()
    {
        std::sort(m_skills.begin(), m_skills.end(), nameLessThan);
    }

    void sortByTranslatedName(QList<ManualSkill *>::iterator begin, QList<ManualSkill *>::iterator end)
    {
        std::sort(begin, end, translatedNameLessThan);
    }

    QList<ManualSkill *>::iterator begin()
    {
        return m_skills.begin();
    }

    QList<ManualSkill *>::iterator end()
    {
        return m_skills.end();
    }

    QString join(const QString &sep)
    {
        QStringList baseNames;
        foreach (ManualSkill *skill, m_skills)
            baseNames << Sanguosha->translate(skill->skill->objectName());

        return baseNames.join(sep);
    }

private:
    QList<ManualSkill *> m_skills;
};

Engine::Engine(bool isManualMode)
{
#ifdef LOGNETWORK
	logFile.setFileName("netmsg.log");
	logFile.open(QIODevice::WriteOnly|QIODevice::Text);
    connect(this, SIGNAL(logNetworkMessage(QString)), this, SLOT(handleNetworkMessage(QString)),Qt::QueuedConnection);
#endif // LOGNETWORK

    Sanguosha = this;

    lua = CreateLuaState();
    if (!DoLuaScript(lua, "lua/config.lua")) exit(1);



    QStringList stringlist_sp_convert = GetConfigFromLuaState(lua, "convert_pairs").toStringList();
    foreach (QString cv_pair, stringlist_sp_convert) {
        QStringList pairs = cv_pair.split("->");
        QStringList cv_to = pairs.at(1).split("|");
        foreach (QString to, cv_to)
            sp_convert_pairs.insert(pairs.at(0), to);
    }

    extra_hidden_generals = GetConfigFromLuaState(lua, "extra_hidden_generals").toStringList();
    removed_hidden_generals = GetConfigFromLuaState(lua, "removed_hidden_generals").toStringList();
    extra_default_lords = GetConfigFromLuaState(lua, "extra_default_lords").toStringList();
    removed_default_lords = GetConfigFromLuaState(lua, "removed_default_lords").toStringList();


    QStringList package_names = GetConfigFromLuaState(lua, "package_names").toStringList();
    foreach (QString name, package_names)
        addPackage(name);

    _loadMiniScenarios();
    _loadModScenarios();
    m_customScene = new CustomScenario;

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));

    if (!DoLuaScript(lua, "lua/sanguosha.lua")) exit(1);

    if (isManualMode) {
        ManualSkillList allSkills;
        foreach (const General *general, getAllGenerals()) {
            allSkills.insert(general->getVisibleSkillList(), general);

            foreach (const QString &skillName, general->getRelatedSkillNames()) {
                const Skill *skill = getSkill(skillName);
                if (skill != NULL && skill->isVisible())
                    allSkills.insert(skill, general);
            }
        }

        allSkills.sortByName();

        QList<ManualSkill *>::iterator j = allSkills.begin();
        QList<ManualSkill *>::iterator i = j;
        for (char c = 'a'; c <= 'z'; ++c) {
            while (j != allSkills.end()) {
                if (!(*j)->baseName.startsWith(c))
                    break;
                else
                    ++j;
            }
            if (j - i > 1)
                allSkills.sortByTranslatedName(i, j);

            i = j;
        }

        QDir dir("manual");
        if (!dir.exists())
            QDir::current().mkdir("manual");


        Config.setValue("AutoSkillTypeColorReplacement", false);
        Config.setValue("AutoSuitReplacement", false);

        QList<ManualSkill *>::iterator iter = allSkills.begin();
        for (char c = 'a'; c <= 'z'; ++c) {
            QChar upper = QChar(c).toUpper();
            QFile file(QString("manual/Chapter%1.lua").arg(upper));
            if (file.open(QFile::WriteOnly | QFile::Truncate)) {
                QTextStream stream(&file);
                stream.setCodec(QTextCodec::codecForName("UTF-8"));

                ManualSkillList list;
                while (iter != allSkills.end()) {
                    if ((*iter)->baseName.startsWith(c)) {
                        list.insert(*iter);
                        ++iter;
                    } else {
                        break;
                    }
                }

                QString info;
                if (list.isEmpty())
                    info = translate("Manual_Empty");
                else
                    info = translate("Manual_Index") + list.join(" ");

                stream << translate("Manual_Head").arg(upper).arg(info)
                          .arg(getVersion())
                       << Qt::endl;

                for (QList<ManualSkill *>::iterator it = list.begin();
                     it < list.end(); ++it) {
                    ManualSkill *skill = *it;
                    QStringList generals;

                    foreach(const General *general, skill->relatedGenerals) {
                        generals << QString("%1-%2")
                                    .arg(translate(general->getPackage()))
                                    .arg(general->getBriefName());
                    }
                    stream << translate("Manual_Skill")
                              .arg(translate(skill->skill->objectName()))
                              .arg(generals.join(" "))
                              .arg(skill->skill->getDescription())
                           << Qt::endl << Qt::endl;
                }

                list.clear();
                file.close();
            }
        }
        return;
    }

    // available game modes
    modes["02p"] = tr("2 players");
    modes["03p"] = tr("3 players");
    modes["04p"] = tr("4 players");
    modes["05p"] = tr("5 players");
    modes["06p"] = tr("6 players");
    modes["06pd"] = tr("6 players (2 renegades)");
    modes["07p"] = tr("7 players");
    modes["08p"] = tr("8 players");
    modes["08pd"] = tr("8 players (2 renegades)");
    modes["08pz"] = tr("8 players (0 renegade)");
    modes["09p"] = tr("9 players");
    modes["10pd"] = tr("10 players");
    modes["10p"] = tr("10 players (1 renegade)");
    modes["10pz"] = tr("10 players (0 renegade)");

    foreach (const Skill *skill, skills.values()) {
        Skill *mutable_skill = const_cast<Skill *>(skill);
        mutable_skill->initMediaSource();
    }
}

lua_State *Engine::getLuaState() const
{
    return lua;
}

void Engine::addTranslationEntry(const char *key, const char *value)
{
    translations.insert(key, QString::fromUtf8(value));
}

Engine::~Engine()
{
    lua_close(lua);
    delete m_customScene;
#ifdef AUDIO_SUPPORT
    Audio::quit();
#endif
}

QStringList Engine::getModScenarioNames() const
{
    return m_scenarios.keys();
}

void Engine::addScenario(Scenario *scenario)
{
    QString key = scenario->objectName();
    m_scenarios[key] = scenario;
    addPackage(scenario);
}

const Scenario *Engine::getScenario(const QString &name) const
{
    if (m_scenarios.contains(name))
        return m_scenarios[name];
    else if (m_miniScenes.contains(name))
        return m_miniScenes[name];
    else if (name == "custom_scenario")
        return m_customScene;
    else return NULL;
}

void Engine::addSkills(const QList<const Skill *> &all_skills)
{
    foreach (const Skill *skill, all_skills) {
        if (!skill) {
            QMessageBox::warning(NULL, "", tr("The engine tries to add an invalid skill"));
            continue;
        }
        if (skills.contains(skill->objectName()))
            QMessageBox::warning(NULL, "", tr("Duplicated skill : %1").arg(skill->objectName()));

        skills.insert(skill->objectName(), skill);

        if (skill->inherits("ProhibitSkill"))
            prohibit_skills << qobject_cast<const ProhibitSkill *>(skill);
        else if (skill->inherits("DistanceSkill"))
            distance_skills << qobject_cast<const DistanceSkill *>(skill);
        else if (skill->inherits("MaxCardsSkill"))
            maxcards_skills << qobject_cast<const MaxCardsSkill *>(skill);
        else if (skill->inherits("TargetModSkill"))
            targetmod_skills << qobject_cast<const TargetModSkill *>(skill);
        else if (skill->inherits("InvaliditySkill"))
            invalidity_skills << qobject_cast<const InvaliditySkill *>(skill);
        else if (skill->inherits("AttackRangeSkill"))
            attack_range_skills << qobject_cast<const AttackRangeSkill *>(skill);
        else if (skill->inherits("TriggerSkill")) {
            const TriggerSkill *trigger_skill = qobject_cast<const TriggerSkill *>(skill);
            if (trigger_skill && trigger_skill->isGlobal())
                global_trigger_skills << trigger_skill;
        }
    }
}

QList<const DistanceSkill *> Engine::getDistanceSkills() const
{
    return distance_skills;
}

QList<const MaxCardsSkill *> Engine::getMaxCardsSkills() const
{
    return maxcards_skills;
}

QList<const TargetModSkill *> Engine::getTargetModSkills() const
{
    return targetmod_skills;
}

QList<const InvaliditySkill *> Engine::getInvaliditySkills() const
{
    return invalidity_skills;
}

QList<const TriggerSkill *> Engine::getGlobalTriggerSkills() const
{
    return global_trigger_skills;
}

QList<const AttackRangeSkill *> Engine::getAttackRangeSkills() const
{
    return attack_range_skills;
}

void Engine::addPackage(Package *package)
{
    if (findChild<const Package *>(package->objectName()))
        return;

    package->setParent(this);
    sp_convert_pairs.unite(package->getConvertPairs());
    patterns.unite(package->getPatterns());
    related_skills.unite(package->getRelatedSkills());

    QList<Card *> all_cards = package->findChildren<Card *>();
    foreach (Card *card, all_cards) {
        card->setId(cards.length());
        cards << card;

        if (card->isKindOf("LuaBasicCard")) {
            const LuaBasicCard *lcard = qobject_cast<const LuaBasicCard *>(card);
            Q_ASSERT(lcard != NULL);
            luaBasicCard_className2objectName.insert(lcard->getClassName(), lcard->objectName());
            if (!luaBasicCards.keys().contains(lcard->getClassName()))
                luaBasicCards.insert(lcard->getClassName(), lcard->clone());
        } else if (card->isKindOf("LuaTrickCard")) {
            const LuaTrickCard *lcard = qobject_cast<const LuaTrickCard *>(card);
            Q_ASSERT(lcard != NULL);
            luaTrickCard_className2objectName.insert(lcard->getClassName(), lcard->objectName());
            if (!luaTrickCards.keys().contains(lcard->getClassName()))
                luaTrickCards.insert(lcard->getClassName(), lcard->clone());
        } else if (card->isKindOf("LuaWeapon")) {
            const LuaWeapon *lcard = qobject_cast<const LuaWeapon *>(card);
            Q_ASSERT(lcard != NULL);
            luaWeapon_className2objectName.insert(lcard->getClassName(), lcard->objectName());
            if (!luaWeapons.keys().contains(lcard->getClassName()))
                luaWeapons.insert(lcard->getClassName(), lcard->clone());
        } else if (card->isKindOf("LuaArmor")) {
            const LuaArmor *lcard = qobject_cast<const LuaArmor *>(card);
            Q_ASSERT(lcard != NULL);
            luaArmor_className2objectName.insert(lcard->getClassName(), lcard->objectName());
            if (!luaArmors.keys().contains(lcard->getClassName()))
                luaArmors.insert(lcard->getClassName(), lcard->clone());
        } else if (card->isKindOf("LuaTreasure")) {
            const LuaTreasure *lcard = qobject_cast<const LuaTreasure *>(card);
            Q_ASSERT(lcard != NULL);
            luaTreasure_className2objectName.insert(lcard->getClassName(), lcard->objectName());
            if (!luaTreasures.keys().contains(lcard->getClassName()))
                luaTreasures.insert(lcard->getClassName(), lcard->clone());
        } else {
            QString class_name = card->metaObject()->className();
            metaobjects.insert(class_name, card->metaObject());
            className2objectName.insert(class_name, card->objectName());
        }
    }

    addSkills(package->getSkills());

    QList<General *> all_generals = package->findChildren<General *>();
    foreach (General *general, all_generals) {
        addSkills(general->findChildren<const Skill *>());
        foreach (QString skill_name, general->getExtraSkillSet()) {
            if (skill_name.startsWith("#")) continue;
            foreach(const Skill *related, getRelatedSkills(skill_name))
                general->addSkill(related->objectName());
        }
        if (sp_convert_pairs.keys().contains(general->objectName())) {
            QStringList to_list(sp_convert_pairs.values(general->objectName()));
            const Skill *skill = new SPConvertSkill(general->objectName(), to_list.join("+"));
            addSkills(QList<const Skill *>() << skill);
            general->addSkill(skill->objectName());
        }
        generals.insert(general->objectName(), general);
        if (isGeneralHidden(general->objectName())) continue;
        if ((general->isLord() && !removed_default_lords.contains(general->objectName()))
            || extra_default_lords.contains(general->objectName()))
            lord_list << general->objectName();
    }

    QList<const QMetaObject *> metas = package->getMetaObjects();
    foreach(const QMetaObject *meta, metas)
        metaobjects.insert(meta->className(), meta);
}

void Engine::addBanPackage(const QString &package_name)
{
    ban_package.insert(package_name);
}

QStringList Engine::getBanPackages() const
{
    if (qApp->arguments().contains("-server"))
        return Config.value("BanPackages").toStringList();
    else
        return ban_package.values();
}

QList<const Package *> Engine::getPackages() const
{
    return findChildren<const Package *>();
}

QString Engine::translate(const QString &to_translate) const
{
    QStringList list = to_translate.split("\\");
    QString res;
    foreach(QString str, list)
        res.append(translations.value(str, str));
    return res;
}

int Engine::getRoleIndex() const
{
    if (ServerInfo.EnableHegemony) {
        return 5;
    } else
        return 1;
}

const CardPattern *Engine::getPattern(const QString &name) const
{
    const CardPattern *ptn = patterns.value(name, NULL);
    if (ptn) return ptn;

    ExpPattern *expptn = new ExpPattern(name);
    patterns.insert(name, expptn);
    return expptn;
}

bool Engine::matchExpPattern(const QString &pattern, const Player *player, const Card *card) const
{
    ExpPattern p(pattern);
    return p.match(player, card);
}

Card::HandlingMethod Engine::getCardHandlingMethod(const QString &method_name) const
{
    if (method_name == "use")
        return Card::MethodUse;
    else if (method_name == "response")
        return Card::MethodResponse;
    else if (method_name == "discard")
        return Card::MethodDiscard;
    else if (method_name == "recast")
        return Card::MethodRecast;
    else if (method_name == "pindian")
        return Card::MethodPindian;
    else {
        Q_ASSERT(false);
        return Card::MethodNone;
    }
}

QList<const Skill *> Engine::getRelatedSkills(const QString &skill_name) const
{
    QList<const Skill *> skills;
    foreach(QString name, related_skills.values(skill_name))
        skills << getSkill(name);

    return skills;
}

const Skill *Engine::getMainSkill(const QString &skill_name) const
{
    const Skill *skill = getSkill(skill_name);
    if (!skill || skill->isVisible() || related_skills.keys().contains(skill_name)) return skill;
    foreach (QString key, related_skills.keys()) {
        foreach(QString name, related_skills.values(key))
            if (name == skill_name) return getSkill(key);
    }
    return skill;
}

const General *Engine::getGeneral(const QString &name) const
{
    return generals.value(name, NULL);
}

int Engine::getGeneralCount(bool include_banned, const QString &kingdom) const
{
    int total = 0;
    QHashIterator<QString, const General *> itor(generals);
    while (itor.hasNext()) {
        bool isBanned = false;
        itor.next();
        const General *general = itor.value();
        if ((!kingdom.isEmpty() && general->getKingdom() != kingdom)
                || isGeneralHidden(general->objectName()))
            continue;

        if (getBanPackages().contains(general->getPackage()))
            isBanned = true;
        else if ((isNormalGameMode(ServerInfo.GameMode)
            || ServerInfo.GameMode.contains("_mini_")
            || ServerInfo.GameMode == "custom_scenario")
            && Config.value("Banlist/Roles").toStringList().contains(general->objectName()))
            isBanned = true;
        else if (ServerInfo.Enable2ndGeneral && BanPair::isBanned(general->objectName()))
            isBanned = true;
        else if (ServerInfo.EnableBasara
            && Config.value("Banlist/Basara").toStringList().contains(general->objectName()))
            isBanned = true;
        else if (ServerInfo.EnableHegemony
            && Config.value("Banlist/Hegemony").toStringList().contains(general->objectName()))
            isBanned = true;
        if (include_banned || !isBanned)
            total++;
    }

    // special case for neo standard package
    if (getBanPackages().contains("standard") && !getBanPackages().contains("nostal_standard")) {
        if (kingdom.isEmpty() || kingdom == "wei")
            ++total; // zhenji
        if (kingdom.isEmpty() || kingdom == "shu")
            ++total; // zhugeliang
        if (kingdom.isEmpty() || kingdom == "wu")
            total += 2; // suanquan && sunshangxiang
    }

    return total;
}

void Engine::registerRoom(QObject *room)
{
    m_mutex.lock();
    m_rooms[QThread::currentThread()] = room;
    m_mutex.unlock();
}

void Engine::unregisterRoom()
{
    m_mutex.lock();
    m_rooms.remove(QThread::currentThread());
    m_mutex.unlock();
}

QObject *Engine::currentRoomObject()
{
    QObject *room = NULL;
    m_mutex.lock();
    room = m_rooms[QThread::currentThread()];
    Q_ASSERT(room);
    m_mutex.unlock();
    return room;
}

Room *Engine::currentRoom()
{
    QObject *roomObject = currentRoomObject();
    Room *room = qobject_cast<Room *>(roomObject);
    Q_ASSERT(room != NULL);
    return room;
}

RoomState *Engine::currentRoomState()
{
    QObject *roomObject = currentRoomObject();
    Room *room = qobject_cast<Room *>(roomObject);
    if (room != NULL) {
        return room->getRoomState();
    } else {
        Client *client = qobject_cast<Client *>(roomObject);
        Q_ASSERT(client != NULL);
        return client->getRoomState();
    }
}

QString Engine::getCurrentCardUsePattern()
{
    return currentRoomState()->getCurrentCardUsePattern();
}

CardUseStruct::CardUseReason Engine::getCurrentCardUseReason()
{
    return currentRoomState()->getCurrentCardUseReason();
}

QString Engine::findConvertFrom(const QString &general_name) const
{
    foreach (QString general, sp_convert_pairs.keys()) {
        if (sp_convert_pairs.values(general).contains(general_name))
            return general;
    }
    return QString();
}

bool Engine::isGeneralHidden(const QString &general_name) const
{
    const General *general = getGeneral(general_name);
    if (!general) return true;
    return (general->isHidden() && !removed_hidden_generals.contains(general_name))
            || extra_hidden_generals.contains(general_name);
}

QVariant Engine::getConfig(const QString &name, QVariant defaultValue)
{
    return Config.value(name, defaultValue);
}

void Engine::setConfig(const QString &key, QVariant value)
{
    Config.setValue(key, value);
}

QStringList Engine::getMiniScenarioNames()
{
    int stage = qMin(Sanguosha->getMiniSceneCounts(), Config.value("MiniSceneStage", 1).toInt());
    QStringList ret;

    for (int i = 1; i <= stage; i++) {
        ret << Sanguosha->translate(QString(MiniScene::S_KEY_MINISCENE).arg(QString::number(i)));
    }

    return ret;
}

QVariant Engine::getServerInfo(const QString &key)
{
    static QVariantMap ServerInfoMap;
    if (ServerInfoMap.isEmpty()) {
        ServerInfoMap.insert("Name", ServerInfo.Name);
        ServerInfoMap.insert("GameMode", ServerInfo.GameMode);
        ServerInfoMap.insert("GameRuleMode", ServerInfo.GameRuleMode);
        ServerInfoMap.insert("OperationTimeout", ServerInfo.OperationTimeout);
        ServerInfoMap.insert("NullificationCountDown", ServerInfo.NullificationCountDown);
        ServerInfoMap.insert("Extensions", ServerInfo.Extensions);
        ServerInfoMap.insert("RandomSeat", ServerInfo.RandomSeat);
        ServerInfoMap.insert("EnableCheat", ServerInfo.EnableCheat);
        ServerInfoMap.insert("FreeChoose", ServerInfo.FreeChoose);
        ServerInfoMap.insert("Enable2ndGeneral", ServerInfo.Enable2ndGeneral);
        ServerInfoMap.insert("EnableSame", ServerInfo.EnableSame);
        ServerInfoMap.insert("EnableBasara", ServerInfo.EnableBasara);
        ServerInfoMap.insert("EnableHegemony", ServerInfo.EnableHegemony);
        ServerInfoMap.insert("EnableAI", ServerInfo.EnableAI);
        ServerInfoMap.insert("DisableChat", ServerInfo.DisableChat);
        ServerInfoMap.insert("MaxHpScheme", ServerInfo.MaxHpScheme);
        ServerInfoMap.insert("Scheme0Subtraction", ServerInfo.Scheme0Subtraction);
        ServerInfoMap.insert("DuringGame", ServerInfo.DuringGame);
    }
    return ServerInfoMap[key];
}

WrappedCard *Engine::getWrappedCard(int cardId)
{
    Card *card = getCard(cardId);
    WrappedCard *wrappedCard = qobject_cast<WrappedCard *>(card);
    Q_ASSERT(wrappedCard != NULL && wrappedCard->getId() == cardId);
    return wrappedCard;
}

Card *Engine::getCard(int cardId)
{
    Card *card = NULL;
    if (cardId < 0 || cardId >= cards.length())
        return NULL;
    QObject *room = currentRoomObject();
    Q_ASSERT(room);
    Room *serverRoom = qobject_cast<Room *>(room);
    if (serverRoom != NULL) {
        card = serverRoom->getCard(cardId);
    } else {
        Client *clientRoom = qobject_cast<Client *>(room);
        Q_ASSERT(clientRoom != NULL);
        card = clientRoom->getCard(cardId);
    }
    Q_ASSERT(card);
    return card;
}

QString Engine::getCard4Qml(int cardId)
{
    Card *card = getCard(cardId);
    QJsonDocument doc;
    QJsonObject obj;
    if (card == NULL) {
        obj.insert("name", "card-back");
        obj.insert("cid", -1);
    } else {
        obj.insert("name", card->objectName());
        obj.insert("cid", card->getId());
        obj.insert("suit", card->getSuitString());
        obj.insert("number", card->getNumber());
        obj.insert("subtype", card->getSubtype());
    }
    doc.setObject(obj);
    return QString(doc.toJson());
}

const Card *Engine::getEngineCard(int cardId) const
{
    if (cardId == Card::S_UNKNOWN_CARD_ID)
        return NULL;
    else if (cardId < 0 || cardId >= cards.length()) {
        // Q_ASSERT(!(cardId < 0 || cardId >= cards.length()));
        return NULL;
    } else {
        Q_ASSERT(cards[cardId] != NULL);
        return cards[cardId];
    }
}

Card *Engine::cloneCard(const Card *card) const
{
    QString name = card->getClassName();
    Card *result = cloneCard(name, card->getSuit(), card->getNumber(), card->getFlags());
    if (result == NULL)
        return NULL;
    result->setId(card->getEffectiveId());
    result->setSkillName(card->getSkillName(false));
    result->setObjectName(card->objectName());
    return result;
}

Card *Engine::cloneCard(const QString &name, Card::Suit suit, int number, const QStringList &flags) const
{
    Card *card = NULL;
    if (luaBasicCard_className2objectName.keys().contains(name)) {
        const LuaBasicCard *lcard = luaBasicCards.value(name, NULL);
        if (!lcard) return NULL;
        card = lcard->clone(suit, number);
    } else if (luaBasicCard_className2objectName.values().contains(name)) {
        QString class_name = luaBasicCard_className2objectName.key(name, name);
        const LuaBasicCard *lcard = luaBasicCards.value(class_name, NULL);
        if (!lcard) return NULL;
        card = lcard->clone(suit, number);
    } else if (luaTrickCard_className2objectName.keys().contains(name)) {
        const LuaTrickCard *lcard = luaTrickCards.value(name, NULL);
        if (!lcard) return NULL;
        card = lcard->clone(suit, number);
    } else if (luaTrickCard_className2objectName.values().contains(name)) {
        QString class_name = luaTrickCard_className2objectName.key(name, name);
        const LuaTrickCard *lcard = luaTrickCards.value(class_name, NULL);
        if (!lcard) return NULL;
        card = lcard->clone(suit, number);
    } else if (luaWeapon_className2objectName.keys().contains(name)) {
        const LuaWeapon *lcard = luaWeapons.value(name, NULL);
        if (!lcard) return NULL;
        card = lcard->clone(suit, number);
    } else if (luaWeapon_className2objectName.values().contains(name)) {
        QString class_name = luaWeapon_className2objectName.key(name, name);
        const LuaWeapon *lcard = luaWeapons.value(class_name, NULL);
        if (!lcard) return NULL;
        card = lcard->clone(suit, number);
    } else if (luaArmor_className2objectName.keys().contains(name)) {
        const LuaArmor *lcard = luaArmors.value(name, NULL);
        if (!lcard) return NULL;
        card = lcard->clone(suit, number);
    } else if (luaArmor_className2objectName.values().contains(name)) {
        QString class_name = luaArmor_className2objectName.key(name, name);
        const LuaArmor *lcard = luaArmors.value(class_name, NULL);
        if (!lcard) return NULL;
        card = lcard->clone(suit, number);
    } else if (luaTreasure_className2objectName.keys().contains(name)) {
        const LuaTreasure *lcard = luaTreasures.value(name, NULL);
        if (!lcard) return NULL;
        card = lcard->clone(suit, number);
    } else if (luaTreasure_className2objectName.values().contains(name)) {
        QString class_name = luaTreasure_className2objectName.key(name, name);
        const LuaTreasure *lcard = luaTreasures.value(class_name, NULL);
        if (!lcard) return NULL;
        card = lcard->clone(suit, number);
    } else {
        const QMetaObject *meta = metaobjects.value(name, NULL);
        if (meta == NULL)
            meta = metaobjects.value(className2objectName.key(name, QString()), NULL);
        if (meta) {
            QObject *card_obj = meta->newInstance(Q_ARG(Card::Suit, suit), Q_ARG(int, number));
            card_obj->setObjectName(className2objectName.value(name, name));
            card = qobject_cast<Card *>(card_obj);
        }
    }
    if (!card) return NULL;
    card->clearFlags();
    if (!flags.isEmpty()) {
        foreach(QString flag, flags)
            card->setFlags(flag);
    }
    return card;
}

SkillCard *Engine::cloneSkillCard(const QString &name) const
{
    const QMetaObject *meta = metaobjects.value(name, NULL);
    if (meta) {
        QObject *card_obj = meta->newInstance();
        SkillCard *card = qobject_cast<SkillCard *>(card_obj);
        return card;
    } else
        return NULL;
}

#ifndef USE_BUILDBOT
QString Engine::getVersionNumber() const
{
    return "20150926";
}
#endif

QString Engine::getVersion() const
{
    QString version_number = getVersionNumber();
    QString mod_name = getMODName();
    if (mod_name == "official")
        return version_number;
    else
        return QString("%1:%2").arg(version_number).arg(mod_name);
}

QString Engine::getVersionName() const
{
    return "V2";
}

QString Engine::getMODName() const
{
    return "official";
}

QStringList Engine::getExtensions() const
{
    QStringList extensions;
    QList<const Package *> packages = findChildren<const Package *>();
    foreach (const Package *package, packages) {
        if (package->inherits("Scenario"))
            continue;

        extensions << package->objectName();
    }
    return extensions;
}

QStringList Engine::getKingdoms() const
{
    static QStringList kingdoms;
    if (kingdoms.isEmpty())
        kingdoms = GetConfigFromLuaState(lua, "kingdoms").toStringList();

    return kingdoms;
}

QColor Engine::getKingdomColor(const QString &kingdom) const
{
    static QMap<QString, QColor> color_map;
    if (color_map.isEmpty()) {
        QVariantMap map = GetValueFromLuaState(lua, "config", "kingdom_colors").toMap();
        QMapIterator<QString, QVariant> itor(map);
        while (itor.hasNext()) {
            itor.next();
            QColor color(itor.value().toString());
            if (!color.isValid()) {
                qWarning("Invalid color for kingdom %s", qPrintable(itor.key()));
                color = QColor(128, 128, 128);
            }
            color_map[itor.key()] = color;
        }
        Q_ASSERT(!color_map.isEmpty());
    }

    return color_map.value(kingdom);
}

QMap<QString, QColor> Engine::getSkillTypeColorMap() const
{
    static QMap<QString, QColor> color_map;
    if (color_map.isEmpty()) {
        QVariantMap map = GetValueFromLuaState(lua, "config", "skill_type_colors").toMap();
        QMapIterator<QString, QVariant> itor(map);
        while (itor.hasNext()) {
            itor.next();
            QColor color(itor.value().toString());
            if (!color.isValid()) {
                qWarning("Invalid color for skill type %s", qPrintable(itor.key()));
                color = QColor(128, 128, 128);
            }
            color_map[itor.key()] = color;
        }
        Q_ASSERT(!color_map.isEmpty());
    }

    return color_map;
}

QStringList Engine::getChattingEasyTexts() const
{
    static QStringList easy_texts;
    if (easy_texts.isEmpty())
        easy_texts = GetConfigFromLuaState(lua, "easy_text").toStringList();

    return easy_texts;
}

QString Engine::getSetupString() const
{
    int timeout = Config.value("OperationNoLimit").toBool() ? 0 : Config.value("OperationTimeout").toInt();
    QString flags;
    if (Config.value("RandomSeat").toBool())
        flags.append("R");
    if (Config.value("EnableCheat").toBool())
        flags.append("C");
    if (Config.value("EnableCheat").toBool() && Config.value("FreeChoose").toBool())
        flags.append("F");
    if (Config.value("Enable2ndGeneral").toBool())
        flags.append("S");
    if (Config.value("EnableAI").toBool())
        flags.append("A");
    if (Config.value("DisableChat").toBool())
        flags.append("M");

    if (Config.value("MaxHpScheme").toInt() == 1)
        flags.append("1");
    else if (Config.value("MaxHpScheme").toInt() == 2)
        flags.append("2");
    else if (Config.value("MaxHpScheme").toInt() == 3)
        flags.append("3");
    else if (Config.value("MaxHpScheme").toInt() == 0) {
        char c = Config.value("Scheme0Subtraction").toInt() + 5 + 'a'; // from -5 to 12
        flags.append(c);
    }

    QString server_name = Config.value("ServerName").toString().toUtf8().toBase64();
    QStringList setup_items;
    QString mode = Config.value("GameMode").toString();
    setup_items << server_name
        << mode
        << QString::number(timeout)
        << QString::number(Config.value("NullificationCountDown").toInt())
        << Sanguosha->getBanPackages().join("+")
        << flags;

    return setup_items.join(":");
}

QMap<QString, QString> Engine::getAvailableModes() const
{
    return modes;
}

QString Engine::getModeName(const QString &mode) const
{
    if (modes.contains(mode))
        return modes.value(mode);
    else
        return tr("%1 [Scenario mode]").arg(translate(mode));
}

int Engine::getPlayerCount(const QString &mode) const
{
    if (modes.contains(mode) || isNormalGameMode(mode)) { // hidden pz settings?
        QRegExp rx("(\\d+)");
        int index = rx.indexIn(mode);
        if (index != -1)
            return rx.capturedTexts().first().toInt();
    } else {
        // scenario mode
        const Scenario *scenario = getScenario(mode);
        Q_ASSERT(scenario);
        return scenario->getPlayerCount();
    }

    return -1;
}

QString Engine::getRoles(const QString &mode) const
{
    int n = getPlayerCount(mode);

    if (modes.contains(mode) || isNormalGameMode(mode)) { // hidden pz settings?
        static const char *table1[] = {
            "",
            "",

            "ZF", // 2
            "ZFN", // 3
            "ZNFF", // 4
            "ZCFFN", // 5
            "ZCFFFN", // 6
            "ZCCFFFN", // 7
            "ZCCFFFFN", // 8
            "ZCCCFFFFN", // 9
            "ZCCCFFFFFN" // 10
        };

        static const char *table2[] = {
            "",
            "",

            "ZF", // 2
            "ZFN", // 3
            "ZNFF", // 4
            "ZCFFN", // 5
            "ZCFFNN", // 6
            "ZCCFFFN", // 7
            "ZCCFFFNN", // 8
            "ZCCCFFFFN", // 9
            "ZCCCFFFFNN" // 10
        };

        const char **table = mode.endsWith("d") ? table2 : table1;
        QString rolechar = table[n];
        if (mode.endsWith("z"))
            rolechar.replace("N", "C");

        return rolechar;
    } else if (mode.startsWith("@")) {
        if (n == 8)
            return "ZCCCNFFF";
        else if (n == 6)
            return "ZCCNFF";
    } else {
        const Scenario *scenario = getScenario(mode);
        if (scenario)
            return scenario->getRoles();
    }
    return QString();
}

QStringList Engine::getRoleList(const QString &mode) const
{
    QString roles = getRoles(mode);

    QStringList role_list;
    for (int i = 0; roles[i] != '\0'; i++) {
        QString role;
        switch (roles[i].toLatin1()) {
        case 'Z': role = "lord"; break;
        case 'C': role = "loyalist"; break;
        case 'N': role = "renegade"; break;
        case 'F': role = "rebel"; break;
        }
        role_list << role;
    }

    return role_list;
}

int Engine::getCardCount() const
{
    return cards.length();
}

QStringList Engine::getLords(bool contain_banned) const
{
    QStringList lords;
    QStringList general_names = getLimitedGeneralNames();

    // add intrinsic lord
    foreach (QString lord, lord_list) {
        if (!general_names.contains(lord))
            continue;
        if (!contain_banned) {
            if (ServerInfo.GameMode.endsWith("p")
                || ServerInfo.GameMode.endsWith("pd")
                || ServerInfo.GameMode.endsWith("pz")
                || ServerInfo.GameMode.contains("_mini_")
                || ServerInfo.GameMode == "custom_scenario")
                if (Config.value("Banlist/Roles", "").toStringList().contains(lord))
                    continue;
            if (Config.value("Enable2ndGeneral").toBool() && BanPair::isBanned(lord))
                continue;
        }
        lords << lord;
    }

    return lords;
}

QStringList Engine::getRandomLords() const
{
    QStringList banlist_ban;

    if (Config.value("GameMode").toString() == "zombie_mode")
        banlist_ban.append(Config.value("Banlist/Zombie").toStringList());
    else if (isNormalGameMode(Config.value("GameMode").toString()))
        banlist_ban.append(Config.value("Banlist/Roles").toStringList());

    QStringList lords;

    foreach (QString alord, getLords()) {
        if (banlist_ban.contains(alord)) continue;
        lords << alord;
    }

    int lord_num = Config.value("LordMaxChoice", -1).toInt();
    if (lord_num != -1 && lord_num < lords.length()) {
        int to_remove = lords.length() - lord_num;
        for (int i = 0; i < to_remove; i++) {
            lords.removeAt(QRandomGenerator::global()->generate() % lords.length());
        }
    }

    QStringList nonlord_list;
    foreach (QString nonlord, generals.keys()) {
        if (isGeneralHidden(nonlord) || lord_list.contains(nonlord)) continue;
        const General *general = generals.value(nonlord);
        if (getBanPackages().contains(general->getPackage()))
            continue;
        if (Config.value("Enable2ndGeneral").toBool() && BanPair::isBanned(general->objectName()))
            continue;
        if (banlist_ban.contains(general->objectName()))
            continue;

        nonlord_list << nonlord;
    }

    qShuffle(nonlord_list);

    int i;
    int extra = Config.value("NonLordMaxChoice", 2).toInt();
    if (lord_num == 0 && extra == 0)
        extra = 1;
    for (i = 0; i < extra; i++) {
        lords << nonlord_list.at(i);
        if (i == nonlord_list.length() - 1) break;
    }

    return lords;
}

QStringList Engine::getLimitedGeneralNames(const QString &kingdom) const
{
    QStringList general_names;
    QHashIterator<QString, const General *> itor(generals);
    while (itor.hasNext()) {
        itor.next();
        const General *gen = itor.value();
        if ((kingdom.isEmpty() || gen->getKingdom() == kingdom)
            && !isGeneralHidden(gen->objectName()) && !getBanPackages().contains(gen->getPackage()))
            general_names << itor.key();
    }

    // special case for neo standard package
    if (getBanPackages().contains("standard") && !getBanPackages().contains("nostal_standard")) {
        if (kingdom.isEmpty() || kingdom == "wei")
            general_names << "zhenji";
        if (kingdom.isEmpty() || kingdom == "shu")
            general_names << "zhugeliang";
        if (kingdom.isEmpty() || kingdom == "wu")
            general_names << "sunquan" << "sunshangxiang";
    }

    return general_names;
}

QStringList Engine::getRandomGenerals(int count, const QSet<QString> &ban_set, const QString &kingdom) const
{
    QT_WARNING_DISABLE_DEPRECATED
    QStringList all_generals = getLimitedGeneralNames(kingdom);
    QSet<QString> general_set = all_generals.toSet();

    Q_ASSERT(all_generals.count() >= count);

    if (isNormalGameMode(ServerInfo.GameMode)
        || ServerInfo.GameMode.contains("_mini_")
        || ServerInfo.GameMode == "custom_scenario") {
        QT_WARNING_DISABLE_DEPRECATED
        general_set.subtract(Config.value("Banlist/Roles", "").toStringList().toSet());
    }

    all_generals = general_set.subtract(ban_set).values();

    // shuffle them
    qShuffle(all_generals);

    QStringList general_list = all_generals.mid(0, count);
    Q_ASSERT(general_list.count() == count);

    return general_list;
}

QList<int> Engine::getRandomCards() const
{
    QList<int> list;
    foreach (Card *card, cards) {
        card->clearFlags();

        QStringList banned_patterns = Config.value("Banlist/Cards").toStringList();
        bool removed = false;
        foreach (QString banned_pattern, banned_patterns) {
            if (matchExpPattern(banned_pattern, NULL, card)) {
                removed = true;
                break;
            }
        }
        if (removed)
            continue;
        if (!getBanPackages().contains(card->getPackage()))
            list << card->getId();
    }

    qShuffle(list);

    return list;
}

QString Engine::getRandomGeneralName() const
{
    return generals.keys().at(QRandomGenerator::global()->generate() % generals.size());
}

void Engine::playSystemAudioEffect(const QString &name, bool superpose) const
{
    playAudioEffect(QString("audio/system/%1.ogg").arg(name), superpose);
}

void Engine::playAudioEffect(const QString &filename, bool superpose) const
{
#ifdef AUDIO_SUPPORT
    if (!Config.value("EnableEffects").toBool())
        return;
    if (filename.isNull())
        return;

    Audio::play(filename, superpose);
#else
    Q_UNUSED(filename)
    Q_UNUSED(superpose)
#endif
}

void Engine::playSkillAudioEffect(const QString &skill_name, int index, bool superpose) const
{
    const Skill *skill = skills.value(skill_name, NULL);
    if (skill)
        skill->playAudioEffect(index, superpose);
}

const Skill *Engine::getSkill(const QString &skill_name) const
{
    return skills.value(skill_name, NULL);
}

const Skill *Engine::getSkill(const EquipCard *equip) const
{
    const Skill *skill;
    if (equip == NULL)
        skill = NULL;
    else
        skill = getSkill(equip->objectName());

    return skill;
}

QStringList Engine::getSkillNames() const
{
    return skills.keys();
}

const TriggerSkill *Engine::getTriggerSkill(const QString &skill_name) const
{
    const Skill *skill = getSkill(skill_name);
    if (skill)
        return qobject_cast<const TriggerSkill *>(skill);
    else
        return NULL;
}

const ViewAsSkill *Engine::getViewAsSkill(const QString &skill_name) const
{
    const Skill *skill = getSkill(skill_name);
    if (skill == NULL)
        return NULL;

    if (skill->inherits("ViewAsSkill"))
        return qobject_cast<const ViewAsSkill *>(skill);
    else if (skill->inherits("TriggerSkill")) {
        const TriggerSkill *trigger_skill = qobject_cast<const TriggerSkill *>(skill);
        return trigger_skill->getViewAsSkill();
    } else
        return NULL;
}

const ProhibitSkill *Engine::isProhibited(const Player *from, const Player *to, const Card *card, const QList<const Player *> &others) const
{
    foreach (const ProhibitSkill *skill, prohibit_skills) {
        if (skill->isProhibited(from, to, card, others))
            return skill;
    }

    return NULL;
}

int Engine::correctDistance(const Player *from, const Player *to) const
{
    int correct = 0;

    foreach (const DistanceSkill *skill, distance_skills) {
        correct += skill->getCorrect(from, to);
    }

    return correct;
}

int Engine::correctMaxCards(const Player *target, bool fixed) const
{
    if (fixed) {
        int max = -1;
        foreach (const MaxCardsSkill *skill, maxcards_skills) {
            int f = skill->getFixed(target);
            if (f > max) max = f;
        }
        return max;
    } else {
        int extra = 0;
        foreach(const MaxCardsSkill *skill, maxcards_skills)
            extra += skill->getExtra(target);
        return extra;
    }
    return 0;
}

int Engine::correctCardTarget(const TargetModSkill::ModType type, const Player *from, const Card *card) const
{
    int x = 0;

    if (type == TargetModSkill::Residue) {
        foreach (const TargetModSkill *skill, targetmod_skills) {
            ExpPattern p(skill->getPattern());
            if (p.match(from, card)) {
                int residue = skill->getResidueNum(from, card);
                if (residue >= 998) return residue;
                x += residue;
            }
        }
    } else if (type == TargetModSkill::DistanceLimit) {
        foreach (const TargetModSkill *skill, targetmod_skills) {
            ExpPattern p(skill->getPattern());
            if (p.match(from, card)) {
                int distance_limit = skill->getDistanceLimit(from, card);
                if (distance_limit >= 998) return distance_limit;
                x += distance_limit;
            }
        }
    } else if (type == TargetModSkill::ExtraTarget) {
        foreach (const TargetModSkill *skill, targetmod_skills) {
            ExpPattern p(skill->getPattern());
            if (p.match(from, card)) {
                x += skill->getExtraTargetNum(from, card);
            }
        }
    }

    return x;
}

bool Engine::correctSkillValidity(const Player *player, const Skill *skill) const
{
    foreach (const InvaliditySkill *is, invalidity_skills) {
        if (!is->isSkillValid(player, skill))
            return false;
    }
    return true;
}

int Engine::correctAttackRange(const Player *target, bool include_weapon, bool fixed) const
{
    int extra = 0;

    foreach (const AttackRangeSkill *skill, attack_range_skills) {
        if (fixed) {
            int f = skill->getFixed(target, include_weapon);
            if (f > extra)
                extra = f;
        } else {
            extra += skill->getExtra(target, include_weapon);
        }
    }

    return extra;
}

#ifdef LOGNETWORK
void Engine::handleNetworkMessage(QString s)
{
    QTextStream out(&logFile);
    out << s << "\n";
}
#endif // LOGNETWORK
