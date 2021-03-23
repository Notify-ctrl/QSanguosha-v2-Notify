#ifndef ROOMSCENE_H
#define ROOMSCENE_H

#include <QQuickItem>
#include "defines.h"
#include "clientplayer.h"
#include "client.h"
#include "structs.h"

class RoomScene : public QQuickItem
{
    Q_OBJECT
public:
    RoomScene(QQuickItem *parent = nullptr);

signals:
    void addChatter(const QString &chatter);
    void chat(const QString &chatter);
    void addPlayer(ClientPlayer *player);
    void removePlayer(const QString &player_name);
    // == Above is done ===========
    //Signals from C++ to QML
    void loseCards(int moveId, QList<CardsMoveStruct> moves);
    void getCards(int moveId, QList<CardsMoveStruct> moves);
    void keepLoseCardLog(const CardsMoveStruct &move);
    void keepGetCardLog(const CardsMoveStruct &move);
    // choice dialog
    void chooseGeneral(const QStringList &generals);
    void chooseSuit(const QStringList &suits);
    void chooseCard(const ClientPlayer *playerName, const QString &flags, const QString &reason,
        bool handcard_visible, Card::HandlingMethod method, QList<int> disabled_ids);
    void chooseKingdom(const QStringList &kingdoms);
    void chooseOption(const QString &skillName, const QStringList &options);

    void bringToFront(QGraphicsItem *item);
    void arrangeSeats(const QList<const ClientPlayer *> &seats);
    void toggleDiscards();
    void enableTargets(const Card *card);
    void useSelectedCard();
    void updateStatus(Client::Status oldStatus, Client::Status newStatus);
    void killPlayer(const QString &who);
    void revivePlayer(const QString &who);
    void showServerInformation();
    void surrender();
    void saveReplayRecord();
    void makeDamage();
    void makeKilling();
    void makeReviving();
    void doScript();
    void viewGenerals(const QString &reason, const QStringList &names);

    void handleGameEvent(const QVariant &arg);

    void doOkButton();
    void doCancelButton();
    void doDiscardButton();

    void setChatBoxVisibleSlot();

    void addRobot();
    void doAddRobotAction();
    void fillRobots();
    void fillCards(const QList<int> &card_ids, const QList<int> &disabled_ids = QList<int>());
    void updateSkillButtons(bool isPrepare = false);
    void acquireSkill(const ClientPlayer *player, const QString &skill_name);
    void updateSelectedTargets();
    void updateTrustButton();
    void onSkillActivated();
    void onSkillDeactivated();
    void doTimeout();
    void startInXs();
    void hideAvatars();
    void changeHp(const QString &who, int delta, DamageStruct::Nature nature, bool losthp);
    void changeMaxHp(const QString &who, int delta);
    void moveFocus(const QStringList &who, QSanProtocol::Countdown);
    void setEmotion(const QString &who, const QString &emotion);
    void showSkillInvocation(const QString &who, const QString &skill_name);
    void doAnimation(int name, const QStringList &args);
    void showOwnerButtons(bool owner);
    void showPlayerCards();
    void updateRolesBox();
    void updateRoles(const QString &roles);
    void addSkillButton(const Skill *skill);

    void resetPiles();
    void removeLightBox();

    void showCard(const QString &player_name, int card_id);
    void viewDistance();

    void speak();

    void onGameStart();
    void onGameOver();
    void onStandoff();

    void appendChatEdit(QString txt);
    void showBubbleChatBox(const QString &who, const QString &words);

    //animations
    void onEnabledChange();

    void takeAmazingGrace(ClientPlayer *taker, int card_id, bool move_cards);

    void attachSkill(const QString &skill_name);
    void detachSkill(const QString &skill_name);
    void updateSkill(const QString &skill_name);

    void doGongxin(const QList<int> &card_ids, bool enable_heart, QList<int> enabled_ids);

    void startAssign();

    void doPindianAnimation();

    void trust();

    // ======================================
    void moveCards(const QVariant &moves);
    void enableCards(const QVariant &cardIds);
    void setPhotoReady(bool ready);
    void enablePhotos(const QVariant &seats);
    void chooseGeneral(const QVariant &generals, int num);
    void startEmotion(const QString &emotion, int seat);
    void playAudio(const QString &path);
    void showIndicatorLine(int from, const QVariantList &tos);
    void showPrompt(const QString &prompt);
    void hidePrompt();
    void setAcceptEnabled(bool enabled);
    void setRejectEnabled(bool enabled);
    void setFinishEnabled(bool enabled);
    void askToChooseCards(const QVariant &cards);
    void clearPopupBox();
    void askToChoosePlayerCard(const QVariant &handcards, const QVariant &equips, const QVariant &delayedTricks);
    void showCard(int fromSeat, const QVariant &cards);
    void showOptions(const QStringList &options);
    void showArrangeCardBox(const QVariant &cards, const QVariant &capacities, const QVariant &names);
    void showGameOverBox(const QVariant &winners);
    void addLog(const QString &richText);


};

#endif // ROOMSCENE_H
