#include "clientstruct.h"
#include "engine.h"
#include "settings.h"
#include "package.h"

ServerInfoStruct ServerInfo;

time_t ServerInfoStruct::getCommandTimeout(QSanProtocol::CommandType command, QSanProtocol::ProcessInstanceType instance)
{
    time_t timeOut;
    if (OperationTimeout == 0)
        return 0;
    else if (command == QSanProtocol::S_COMMAND_CHOOSE_GENERAL)
        timeOut = OperationTimeout * 1500;
    else if (command == QSanProtocol::S_COMMAND_SKILL_GUANXING)
        timeOut = OperationTimeout * 2000;
    else if (command == QSanProtocol::S_COMMAND_NULLIFICATION)
        timeOut = NullificationCountDown * 1000;
    else
        timeOut = OperationTimeout * 1000;

    if (instance == QSanProtocol::S_SERVER_INSTANCE)
        timeOut += Config.S_SERVER_TIMEOUT_GRACIOUS_PERIOD;
    return timeOut;
}

bool ServerInfoStruct::parse(const QString &_str)
{
    if (_str.isEmpty()) {
        DuringGame = false;
    } else {
        DuringGame = true;

        QStringList str = _str.split(":");

        QString server_name = str.at(0);
        Name = QString::fromUtf8(QByteArray::fromBase64(server_name.toLatin1()));

        GameMode = str.at(1);
        OperationTimeout = str.at(2).toInt();
        NullificationCountDown = str.at(3).toInt();

        QStringList ban_packages = str.at(4).split("+");
        QList<const Package *> packages = Sanguosha->findChildren<const Package *>();
        foreach (const Package *package, packages) {
            if (package->inherits("Scenario"))
                continue;

            QString package_name = package->objectName();
            if (ban_packages.contains(package_name))
                package_name = "!" + package_name;

            Extensions << package_name;
        }

        QString flags = str.at(5);

        RandomSeat = flags.contains("R");
        EnableCheat = flags.contains("C");
        FreeChoose = EnableCheat && flags.contains("F");
        Enable2ndGeneral = flags.contains("S");
        EnableSame = flags.contains("T");
        EnableBasara = flags.contains("B");
        EnableHegemony = flags.contains("H");
        EnableAI = flags.contains("A");
        DisableChat = flags.contains("M");

        if (flags.contains("1"))
            MaxHpScheme = 1;
        else if (flags.contains("2"))
            MaxHpScheme = 2;
        else if (flags.contains("3"))
            MaxHpScheme = 3;
        else {
            MaxHpScheme = 0;
            for (char c = 'a'; c <= 'r'; c++) {
                if (flags.contains(c)) {
                    Scheme0Subtraction = int(c) - int('a') - 5;
                    break;
                }
            }
        }
    }

    return true;
}
