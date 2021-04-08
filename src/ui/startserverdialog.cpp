#include "startserverdialog.h"
#include "server.h"
#include "settings.h"
#include "engine.h"

StartServerDialog::StartServerDialog(QQuickItem *parent) : QQuickItem(parent)
{

}

void StartServerDialog::createServer()
{
    m_server = new Server(this);
    if (!m_server->listen()) {
        emit messageLogged(tr("Cannot start server!"));
        m_server->deleteLater();
        return;
    }

    m_server->daemonize();
    printServerInfo();
    connect(m_server, &Server::server_message, this, &StartServerDialog::messageLogged);
}

static bool isLanAddress(const QString &address)
{
    if (address.startsWith("192.168.") || address.startsWith("10."))
        return true;
    else if (address.startsWith("172.")) {
        bool ok = false;
        int n = address.split(".").value(1).toInt(&ok);
        if (ok && (n >= 16 && n < 32))
            return true;
    }

    return false;
}

void StartServerDialog::printServerInfo()
{
    QStringList items;
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, addresses) {
        quint32 ipv4 = address.toIPv4Address();
        if (ipv4)
            items << address.toString();
    }

    items.sort();

    foreach (const QString &item, items) {
        if (isLanAddress(item))
            emit messageLogged(tr("Your LAN address: %1, this address is available only for hosts that in the same LAN").arg(item));
        else if (item == "127.0.0.1")
            emit messageLogged(tr("Your loopback address %1, this address is available only for your host").arg(item));
        else if (item.startsWith("5.") || item.startsWith("25."))
            emit messageLogged(tr("Your Hamachi address: %1, the address is available for users that joined the same Hamachi network").arg(item));
        else if (!item.startsWith("169.254."))
            emit messageLogged(tr("Your other address: %1, if this is a public IP, that will be available for all cases").arg(item));
    }

    emit messageLogged(tr("Binding port number is %1").arg(Config.value("ServerPort").toInt()));
    emit messageLogged(tr("Game mode is %1").arg(Sanguosha->getModeName(Config.value("GameMode").toString())));
    emit messageLogged(tr("Player count is %1").arg(Sanguosha->getPlayerCount(Config.value("GameMode").toString())));
    emit messageLogged(Config.value("OperationNoLimit").toBool() ?
        tr("There is no time limit") :
        tr("Operation timeout is %1 seconds").arg(Config.value("OperationTimeout").toInt()));
    emit messageLogged(Config.value("EnableCheat").toBool() ? tr("Cheat is enabled") : tr("Cheat is disabled"));
    if (Config.value("EnableCheat").toBool())
        emit messageLogged(Config.value("FreeChoose").toBool() ? tr("Free choose is enabled") : tr("Free choose is disabled"));

    if (Config.Enable2ndGeneral) {
        QString scheme_str;
        switch (Config.value("MaxHpScheme").toInt()) {
        case 0: scheme_str = QString(tr("Sum - %1")).arg(Config.value("Scheme0Subtraction").toInt()); break;
        case 1: scheme_str = tr("Minimum"); break;
        case 2: scheme_str = tr("Maximum"); break;
        case 3: scheme_str = tr("Average"); break;
        }
        emit messageLogged(tr("Secondary general is enabled, max hp scheme is %1").arg(scheme_str));
    } else
        emit messageLogged(tr("Seconardary general is disabled"));

    if (Config.EnableAI) {
        emit messageLogged(tr("This server is AI enabled, AI delay is %1 milliseconds").arg(Config.value("AIDelay").toInt()));
    } else
        emit messageLogged(tr("This server is AI disabled"));
}
