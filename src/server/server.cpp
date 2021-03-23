#include "server.h"
#include "settings.h"
#include "room.h"
#include "engine.h"
#include "nativesocket.h"
#include "scenario.h"
#include "miniscenarios.h"
#include "json.h"
#include "gamerule.h"
#include "clientstruct.h"
#include "defines.h"

using namespace QSanProtocol;

Server::Server(QObject *parent)
    : QObject(parent), created_successfully(true)
{
    server = new NativeServerSocket;
    server->setParent(this);
	playerCount = 0;

    //synchronize ServerInfo on the server side to avoid ambiguous usage of Config and ServerInfo
    ServerInfo.parse(Sanguosha->getSetupString());

    current = NULL;
    if (!createNewRoom()) created_successfully = false;

    connect(server, SIGNAL(new_connection(ClientSocket *)), this, SLOT(processNewConnection(ClientSocket *)));
}

void Server::broadcast(const QString &msg)
{
    QString to_sent = msg.toUtf8().toBase64();
    JsonArray arg;
    arg << "." << to_sent;

    Packet packet(S_SRC_ROOM | S_TYPE_NOTIFICATION | S_DEST_CLIENT, S_COMMAND_SPEAK);
    packet.setMessageBody(arg);
    foreach(Room *room, rooms)
        room->broadcastInvoke(&packet);
}

bool Server::listen()
{
    return created_successfully && server->listen();
}

void Server::daemonize()
{
    server->daemonize();
}

Room *Server::createNewRoom()
{
    Room *new_room = new Room(this, Config.value("GameMode").toString());
    if (!new_room->getLuaState()) {
        delete new_room;
        return NULL;
    }
    current = new_room;
    rooms.insert(current);

    connect(current, SIGNAL(room_message(QString)), this, SIGNAL(server_message(QString)));
    connect(current, SIGNAL(game_over(QString)), this, SLOT(gameOver()));

    return current;
}

void Server::processNewConnection(ClientSocket *socket)
{
    QString addr = socket->peerAddress();

    if (Config.value("BannedIP").toStringList().contains(addr)) {
        socket->disconnectFromHost();
        emit server_message(tr("Forbid the connection of address %1").arg(addr));
        return;
    }

    if (Config.value("ForbidSIMC").toBool()) {
		if (addresses.contains(addr)) {
			socket->disconnectFromHost();
			emit server_message(tr("Forbid the connection of address %1").arg(addr));
			return;
		}
		else
			addresses.insert(addr);
	}

	connect(socket, SIGNAL(disconnected()), this, SLOT(cleanup()));
    
    Packet packet(S_SRC_ROOM | S_TYPE_NOTIFICATION | S_DEST_CLIENT, S_COMMAND_CHECK_VERSION);
    packet.setMessageBody((Sanguosha->getVersion()));
    socket->send((packet.toString()));

    Packet packet2(S_SRC_ROOM | S_TYPE_NOTIFICATION | S_DEST_CLIENT, S_COMMAND_SETUP);
	QString s = Sanguosha->getSetupString();
	s.append(":"+QString::number(playerCount));
    packet2.setMessageBody(s);
    socket->send((packet2.toString()));
	playerCount++;

    emit server_message(tr("%1 connected").arg(socket->peerName()));

    connect(socket, SIGNAL(message_got(const char *)), this, SLOT(processRequest(const char *)));
    socket->timerSignup.start(30000);
}

void Server::processRequest(const char *request)
{
    ClientSocket *socket = qobject_cast<ClientSocket *>(sender());
    socket->disconnect(this, SLOT(processRequest(const char *)));
    socket->timerSignup.stop();

    Packet signup;
    if (!signup.parse(request) || signup.getCommandType() != S_COMMAND_SIGNUP) {
        emit server_message(tr("Invalid signup string: %1").arg(request));
        QSanProtocol::Packet packet(S_SRC_ROOM | S_TYPE_NOTIFICATION | S_DEST_CLIENT, S_COMMAND_WARN);
        packet.setMessageBody("INVALID_FORMAT");
        socket->send(packet.toString());
        socket->disconnectFromHost();
        return;
    }

    const JsonArray &body = signup.getMessageBody().value<JsonArray>();
    bool reconnection_enabled = body[0].toBool();
    QString screen_name = QString::fromUtf8(QByteArray::fromBase64(body[1].toString().toLatin1()));
    QString avatar = body[2].toString();

    if (reconnection_enabled) {
        foreach (QString objname, name2objname.values(screen_name)) {
            ServerPlayer *player = players.value(objname);
            if (player && player->getState() == "offline" && !player->getRoom()->isFinished()) {
                player->getRoom()->reconnect(player, socket);
                return;
            }
        }
    }

    if (current == NULL || current->isFull() || current->isFinished()) {
        if (!createNewRoom()) return;
    }

    ServerPlayer *player = current->addSocket(socket);
    current->signup(player, screen_name, avatar, false);
    emit newPlayer(player);
}

void Server::cleanup()
{
	playerCount--;
    const ClientSocket *socket = qobject_cast<const ClientSocket *>(sender());
    if (Config.value("ForbidSIMC").toBool())
        addresses.remove(socket->peerAddress());
}

void Server::signupPlayer(ServerPlayer *player)
{
    name2objname.insert(player->screenName(), player->objectName());
    players.insert(player->objectName(), player);
}

void Server::gameOver()
{
    Room *room = qobject_cast<Room *>(sender());
    rooms.remove(room);

    foreach(ServerPlayer *player, room->findChildren<ServerPlayer *>())
    {
        name2objname.remove(player->screenName(), player->objectName());
        players.remove(player->objectName());
    }
}
