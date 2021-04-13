#ifndef QMLROUTER_H
#define QMLROUTER_H

#include <QObject>
#include "card.h"
#include "engine.h"
#include "client.h"
#include "clientplayer.h"
#include "json.h"

// Class for communication between c++ and qml

class QmlRouter : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_ANONYMOUS
public:
    explicit QmlRouter(QObject *parent = nullptr);

    Q_INVOKABLE bool card_isAvailable(int card, QString player_name);

    Q_INVOKABLE bool roomscene_card_idenabled(int id, int index);
    Q_INVOKABLE QString roomscene_enable_targets(int id, QStringList selected_targets);
    Q_INVOKABLE QStringList roomscene_update_targets_enablity(int id, QStringList selected_targets);
    Q_INVOKABLE QString roomscene_update_selected_targets(int id, QString player_name, bool selected, QStringList targets);
    Q_INVOKABLE void roomscene_use_card(int id, QStringList selected_targets);

private:
    const Card *qml_getCard(QVariant data);
    QString enable_targets(const Card *, QStringList);
    QStringList updateTargetsEnablity(const Card *, QStringList);
    QString updateSelectedTargets(const Card *, QString, bool, QStringList);
    void useCard(const Card *, QStringList);
};

extern QmlRouter *Router;

#endif // QMLROUTER_H
