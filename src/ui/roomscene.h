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
    void clearPopupBox();
    void askToChoosePlayerCard(const QVariant &handcards, const QVariant &equips, const QVariant &delayedTricks);
    void showCard(int fromSeat, const QVariant &cards);
    void showOptions(const QStringList &options);
    void showArrangeCardBox(const QVariant &cards, const QVariant &capacities, const QVariant &names);
    void showGameOverBox(const QVariant &winners);
    void addLog(const QString &richText);

};

#endif // ROOMSCENE_H
