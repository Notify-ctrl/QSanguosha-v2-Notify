#ifndef _SERVER_H
#define _SERVER_H

class Room;
class ServerSocket;
class ClientSocket;

#include "src/pch.h"

class Package;

class Scenario;
class ServerPlayer;

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent);

    friend class BanIpDialog;

    void broadcast(const QString &msg);
    bool listen();
    void daemonize();
    Room *createNewRoom();
    void signupPlayer(ServerPlayer *player);

private:
    ServerSocket *server;
    Room *current;
    QSet<Room *> rooms;
    QHash<QString, ServerPlayer *> players;
    QSet<QString> addresses;
    QMultiHash<QString, QString> name2objname;
    bool created_successfully;
	int playerCount;

private slots:
    void processNewConnection(ClientSocket *socket);
    void processRequest(const char *request);
    void cleanup();
    void gameOver();

signals:
    void server_message(const QString &);
    void newPlayer(ServerPlayer *player);
};

#endif

