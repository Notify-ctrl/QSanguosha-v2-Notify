#ifndef _CLIENT_PLAYER_H
#define _CLIENT_PLAYER_H

#include "player.h"
//#include "clientstruct.h"

class Client;
class QTextDocument;

class ClientPlayer : public Player
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Don't do like this.")
    Q_PROPERTY(int handcard READ getHandcardNum WRITE setHandcardNum)
    Q_PROPERTY(QString mark_doc READ getMarkDoc NOTIFY mark_doc_changed)

public:
    explicit ClientPlayer(Client *client);
    QList<const Card *> getHandcards() const;
    void setCards(const QList<int> &card_ids);
    QString getMarkDoc() const;
    void changePile(const QString &name, bool add, QList<int> card_ids);
    QString getDeathPixmapPath() const;
    void setHandcardNum(int n);
    QString getGameMode() const;

    void setFlags(const QString &flag);
    int aliveCount() const;
    int getHandcardNum() const;
    void removeCard(const Card *card, Place place);
    void addCard(const Card *card, Place place);
    void addKnownHandCard(const Card *card);
    bool isLastHandCard(const Card *card, bool contain = false) const;
    void setMark(const QString &mark, int value);

private:
    int handcard_num;
    QList<const Card *> known_cards;
    QString mark_doc;

signals:
    void pile_changed(const QString &name);
    void drank_changed();
    void action_taken();
    void duanchang_invoked();
    void mark_doc_changed();
};

extern ClientPlayer *Self;

Q_DECLARE_METATYPE(ClientPlayer *)

#endif

