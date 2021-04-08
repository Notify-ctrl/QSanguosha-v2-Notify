#ifndef ROOMSCENE_H
#define ROOMSCENE_H

#include <QQuickItem>
#include "clientplayer.h"
#include "client.h"
#include "structs.h"

class RoomScene : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
public:
    RoomScene(QQuickItem *parent = nullptr);

    Q_INVOKABLE QString _translateMovement(const CardsMoveStruct &move);

signals:
    void addChatter(const QString &chatter);
    void chat(const QString &chatter);
    void addPlayer(ClientPlayer *player);
    void removePlayer(const QString &player_name);
    void returnToStart();
    void chooseGeneral(const QStringList &generals);
    void chooseGeneralDone(const QString &general);
    void updateProperty(QVariantList args);
    void receiveLog(const QString &log_str);
    void addRobot(int num);
    void trust();
    // void moveCards(int moveId, QList<CardsMoveStruct> moves);
    void loseCards(int moveId, QList<CardsMoveStruct> moves);
    void getCards(int moveId, QList<CardsMoveStruct> moves);
    void setEmotion(const QString &who, const QString &emotion);
    void doAnimation(int name, const QStringList &args);
    void changeHp(const QString &who, int delta, int nature, bool losthp);
    void handleGameEvent(QVariantList args);
    void fillCards(const QList<int> &card_ids, const QList<int> &disabled_ids);
    void takeAmazingGrace(ClientPlayer *taker, int card_id, bool move_cards);
    void amazingGraceTaken(int cid);
    void clearPopupBox();
    void showGameOverBox();

    // void moveCards(const QVariant &moves);
    void enableCards(const QVariant &cardIds);
    void setPhotoReady(bool ready);
    void enablePhotos(const QVariant &seats);
    // void startEmotion(const QString &emotion, int seat);
    void playAudio(const QString &path);
    void showPrompt(const QString &prompt);
    void hidePrompt();
    void setAcceptEnabled(bool enabled);
    void setRejectEnabled(bool enabled);
    void setFinishEnabled(bool enabled);
    void askToChooseCards(const QVariant &cards);

    void askToChoosePlayerCard(const QVariant &handcards, const QVariant &equips, const QVariant &delayedTricks);
    void showCard(int fromSeat, const QVariant &cards);
    void showOptions(const QStringList &options);
    void showArrangeCardBox(const QVariant &cards, const QVariant &capacities, const QVariant &names);
    void addLog(const QString &richText);
    void updateStatus(Client::Status oldStatus, Client::Status newStatus);

};

#endif // ROOMSCENE_H
