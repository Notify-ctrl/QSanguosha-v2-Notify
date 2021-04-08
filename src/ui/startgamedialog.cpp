#include "startgamedialog.h"
#include "client.h"
#include "engine.h"

StartGameDialog::StartGameDialog(QQuickItem *parent) : QQuickItem(parent)
{

}

void StartGameDialog::connectToServer()
{
    Client *client = new Client(qApp);

    connect(client, &Client::version_checked, this, &StartGameDialog::checkVersion);
    connect(client, &Client::error_message, this, &StartGameDialog::errorGet);
}

void StartGameDialog::checkVersion(const QString &server_version, const QString &server_mod)
{
    QString client_mod = Sanguosha->getMODName();
    if (client_mod != server_mod) {
        emit errorGet(tr("Client MOD name is not same as the server!"));
        return;
    }

    Client *client = qobject_cast<Client *>(sender());
    QString client_version = Sanguosha->getVersionNumber();

    if (server_version == client_version) {
        client->signup();
        connect(client, &Client::server_connected, this, &StartGameDialog::enterRoom);
        return;
    }

    client->disconnectFromHost();

    static QString link = "http://github.com/Mogara/QSanguosha-v2";
    QString text = tr("Server version is %1, client version is %2 <br/>").arg(server_version).arg(client_version);
    if (server_version > client_version)
        text.append(tr("Your client version is older than the server's, please update it <br/>"));
    else
        text.append(tr("The server version is older than your client version, please ask the server to update<br/>"));

    text.append(tr("Download link : <a href='%1'>%1</a> <br/>").arg(link));
    emit errorGet(text);
}

void StartGameDialog::networkError(const QString &error_msg)
{
    emit errorGet(error_msg);
}
