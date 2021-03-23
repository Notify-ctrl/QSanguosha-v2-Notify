#include "generalselector.h"
#include "engine.h"
#include "serverplayer.h"
#include "util.h"
#include "room.h"

static GeneralSelector *Selector;

GeneralSelector *GeneralSelector::getInstance()
{
    if (Selector == NULL) {
        Selector = new GeneralSelector;
        //@todo: this setParent is illegitimate in QT and is equivalent to calling
        // setParent(NULL). So taking it off at the moment until we figure out
        // a way to do it.
        //Selector->setParent(Sanguosha);
    }

    return Selector;
}

GeneralSelector::GeneralSelector()
{
    loadFirstGeneralTable();
    loadSecondGeneralTable();
}

QString GeneralSelector::selectFirst(ServerPlayer *player, const QStringList &candidates)
{
    QMap<QString, qreal> values;
    QString role = player->getRole();
    ServerPlayer *lord = player->getRoom()->getLord();
    foreach (QString candidate, candidates) {
        qreal value = 5.0;
        const General *general = Sanguosha->getGeneral(candidate);
        if (role == "loyalist" && lord && (general->getKingdom() == lord->getKingdom() || general->getKingdom() == "god"))
            value *= 1.04;
        if (role == "rebel" && lord && lord->getGeneral() && lord->getGeneral()->hasSkill("xueyi")
            && general->getKingdom() == "qun")
            value *= 0.8;
        if (role != "loyalist" && lord && lord->getGeneral() && lord->getGeneral()->hasSkill("shichou")
            && general->getKingdom() == "shu")
            value *= 0.1;
        if (role == "rebel" && lord && lord->getGeneral() && lord->getGeneral()->hasSkill("guiming")
            && general->getKingdom() == "wu")
            value *= 0.5;
        QString key = QString("_:%1:%2").arg(candidate).arg(role);
        value *= qPow(1.1, first_general_table.value(key, 0.0));
        if (lord) {
            QString key2 = QString("%1:%2:%3").arg(lord->getGeneralName()).arg(candidate).arg(role);
            value *= qPow(1.1, first_general_table.value(key2, 0.0));
        }
        values.insert(candidate, value);
    }

    QStringList _candidates = candidates;
    QStringList choice_list;
    while (!_candidates.isEmpty() && choice_list.length() < 6) {
        qreal max = -1;
        QString choice = QString();
        foreach (QString candidate, _candidates) {
            qreal value = values.value(candidate, 5.0);
            if (value > max) {
                max = value;
                choice = candidate;
            }
        }
        choice_list << choice;
        _candidates.removeOne(choice);
    }

    QString max_general;
    int rnd = QRandomGenerator::global()->generate() % 100;
    int total = choice_list.length();
    int prob[6] = { 70, 85, 92, 95, 97, 99 };
    for (int i = 0; i < 6; i++) {
        if (rnd <= prob[i] || total <= i + 1) {
            max_general = choice_list.at(i).split(":").at(0);
            break;
        }
    }

    Q_ASSERT(!max_general.isEmpty());
    return max_general;
}

QString GeneralSelector::selectSecond(ServerPlayer *player, const QStringList &candidates)
{
    QString first = player->getGeneralName();

    int max = -1;
    QString max_general;

    foreach (QString candidate, candidates) {
        QString key = QString("%1+%2").arg(first).arg(candidate);
        int value = second_general_table.value(key, 0);
        if (value == 0) {
            key = QString("%1+%2").arg(candidate).arg(first);
            value = second_general_table.value(key, 50);
        }

        if (value > max) {
            max = value;
            max_general = candidate;
        }
    }

    Q_ASSERT(!max_general.isEmpty());

    return max_general;
}

QString GeneralSelector::selectHighest(const QHash<QString, int> &table, const QStringList &candidates, int default_value)
{
    int max = -1;
    QString max_general;

    foreach (QString candidate, candidates) {
        int value = table.value(candidate, default_value);

        if (value > max) {
            max = value;
            max_general = candidate;
        }
    }

    Q_ASSERT(!max_general.isEmpty());

    return max_general;
}

void GeneralSelector::loadFirstGeneralTable()
{
    loadFirstGeneralTable("loyalist");
    loadFirstGeneralTable("rebel");
    loadFirstGeneralTable("renegade");
}

void GeneralSelector::loadFirstGeneralTable(const QString &role)
{
    QFile file(QString("etc/%1.txt").arg(role));
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString lord_name, name;
            stream >> lord_name >> name;
            qreal value;
            stream >> value;

            QString key = QString("%1:%2:%3").arg(lord_name).arg(name).arg(role);
            first_general_table.insert(key, value);
        }

        file.close();
    }
}

void GeneralSelector::loadSecondGeneralTable()
{
    QRegExp rx("(\\w+)\\s+(\\w+)\\s+(\\d+)");
    QFile file("etc/double-generals.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (!rx.exactMatch(line))
                continue;

            QStringList texts = rx.capturedTexts();
            QString first = texts.at(1);
            QString second = texts.at(2);
            int value = texts.at(3).toInt();

            QString key = QString("%1+%2").arg(first).arg(second);
            second_general_table.insert(key, value);
        }

        file.close();
    }
}
