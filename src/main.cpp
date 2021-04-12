#include <cstring>

#include <QQmlApplicationEngine>
#include "settings.h"
#include "banpair.h"
#include "server.h"
#include "audio.h"
#include "serverplayer.h"
#include "engine.h"
#include "qmlrouter.h"

QQmlApplicationEngine *main_window;

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "-server") == 0) {
        new QCoreApplication(argc, argv);
    } else if (argc > 1 && strcmp(argv[1], "-manual") == 0) {
        new QCoreApplication(argc, argv);
        Sanguosha = new Engine(true);
        return 0;
    } else {
        new QApplication(argc, argv);
    }

    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");

#ifdef Q_OS_MAC
#ifdef QT_NO_DEBUG
    QDir::setCurrent(qApp->applicationDirPath());
#endif
#endif

#ifdef Q_OS_LINUX
    QDir dir(QString("lua"));
    if (dir.exists() && (dir.exists(QString("config.lua")))) {
        // things look good and use current dir
    } else {
        QDir::setCurrent(qApp->applicationFilePath().replace("games", "share"));
    }
#endif

    // initialize random seed for later use
    // qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    // QRandomGenerator::global()->seed(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    QTranslator qt_translator, translator;
    qt_translator.load("qt_zh_CN.qm");
    translator.load("sanguosha.qm");

    qApp->installTranslator(&qt_translator);
    qApp->installTranslator(&translator);

    Sanguosha = new Engine;
    Config.init();
    qApp->setFont(Config.AppFont);
    BanPair::loadBanPairs();

    if (qApp->arguments().contains("-server")) {
        Server *server = new Server(qApp);
        printf("Server is starting on port %u\n", Config.ServerPort);

        if (server->listen())
            printf("Starting successfully\n");
        else {
            delete server;
            printf("Starting failed!\n");
        }

        return qApp->exec();
    }

#ifdef AUDIO_SUPPORT
    Audio::init();
#endif

    Router = new QmlRouter;

    main_window = new QQmlApplicationEngine;

    Sanguosha->setParent(main_window);
    main_window->rootContext()->setContextProperty("Router", Router);
    main_window->rootContext()->setContextProperty("Sanguosha", Sanguosha);
    main_window->load(QUrl(QStringLiteral("script/main.qml")));
    if (main_window->rootObjects().isEmpty())
        return -1;

    foreach (QString arg, qApp->arguments()) {
        if (arg.startsWith("-connect:")) {
            arg.remove("-connect:");
            Config.HostAddress = arg;
            Config.setValue("HostAddress", arg);

            // main_window->startConnection();
            break;
        }
    }

    return qApp->exec();
}

