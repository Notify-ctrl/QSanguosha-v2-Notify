#ifndef _GAME_RULE_H
#define _GAME_RULE_H

#include "skill.h"

static QVariant _dummy_variant;

class GameRule : public TriggerSkill
{
    Q_OBJECT

public:
    GameRule(QObject *parent);
    virtual bool triggerable(const ServerPlayer *target) const;
    virtual int getPriority(TriggerEvent triggerEvent) const;
    virtual bool trigger(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data = _dummy_variant) const;

private:
    void onPhaseProceed(ServerPlayer *player) const;
    void rewardAndPunish(ServerPlayer *killer, ServerPlayer *victim) const;
    QString getWinner(ServerPlayer *victim) const;
};

#endif

