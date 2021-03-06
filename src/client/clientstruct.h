#ifndef _CLIENT_STRUCT_H
#define _CLIENT_STRUCT_H

#include "protocol.h"

struct ServerInfoStruct
{
    bool parse(const QString &str);
    //Get the timeout allowance for a command. Server countdown is more lenient than the client.
    //@param command: type of command
    //@return countdown for command in milliseconds.
    time_t getCommandTimeout(QSanProtocol::CommandType command, QSanProtocol::ProcessInstanceType instance);

    QString Name;
    QString GameMode;
    QString GameRuleMode;
    int OperationTimeout;
    int NullificationCountDown;
    QStringList Extensions;
    bool RandomSeat;
    bool EnableCheat;
    bool FreeChoose;
    bool Enable2ndGeneral;
    bool EnableSame;
    bool EnableBasara;
    bool EnableHegemony;
    bool EnableAI;
    bool DisableChat;
    int MaxHpScheme;
    int Scheme0Subtraction;

    bool DuringGame;
};

extern ServerInfoStruct ServerInfo;

#endif

