#ifndef ROOMSCENE_H
#define ROOMSCENE_H

#include <QQuickItem>
#include "defines.h"

class RoomScene : public QQuickItem
{
    Q_OBJECT
public:
    RoomScene();

signals:
    //Signals from C++ to QML
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
