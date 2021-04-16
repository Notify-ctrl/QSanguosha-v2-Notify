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
    Q_INVOKABLE QString get_skill_details(QString skill_name);
    Q_INVOKABLE bool vs_view_filter(QString skill_name, QList<int> ids, int id);
    Q_INVOKABLE bool vs_can_view_as(QString skill_name, QList<int> ids);
    Q_INVOKABLE void update_discard_skill();
    Q_INVOKABLE QString update_response_skill();
    Q_INVOKABLE QStringList roomscene_get_enable_skills(QStringList skill_names, int newStatus);
    Q_INVOKABLE QString roomscene_enable_targets(int id, QStringList selected_targets);
    Q_INVOKABLE QString roomscene_enable_targets(QString json_data, QStringList selected_targets);
    Q_INVOKABLE QStringList roomscene_update_targets_enablity(int id, QStringList selected_targets);
    Q_INVOKABLE QStringList roomscene_update_targets_enablity(QString json_data, QStringList selected_targets);
    Q_INVOKABLE QString roomscene_update_selected_targets(int id, QString player_name, bool selected, QStringList targets);
    Q_INVOKABLE QString roomscene_update_selected_targets(QString json_data, QString player_name, bool selected, QStringList targets);
    Q_INVOKABLE void roomscene_use_card(int id, QStringList selected_targets);
    Q_INVOKABLE void roomscene_use_card(QString json_data, QStringList selected_targets);
    Q_INVOKABLE void roomscene_finish();
    Q_INVOKABLE void roomscene_discard(QString json_data) {
        ClientInstance->onPlayerDiscardCards(qml_getCard(json_data));
    }
    Q_INVOKABLE void roomscene_invoke_skill(bool invoke) {
        ClientInstance->onPlayerInvokeSkill(invoke);
    }
    Q_INVOKABLE void on_player_response_card(int id, QStringList targets);
    Q_INVOKABLE void on_player_response_card(QString json_data, QStringList targets);

private:
    const Card *qml_getCard(QString);
    QString enable_targets(const Card *, QStringList);
    QStringList updateTargetsEnablity(const Card *, QStringList);
    QString updateSelectedTargets(const Card *, QString, bool, QStringList);
    void useCard(const Card *, QStringList);
    const ViewAsSkill *qml_getViewAsSkill(QString);
};

extern QmlRouter *Router;

#endif // QMLROUTER_H
