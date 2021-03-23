#ifndef STARTSERVERDIALOG_H
#define STARTSERVERDIALOG_H

#include <QQuickItem>
#include "defines.h"

class Server;

class StartServerDialog : public QQuickItem
{
    Q_OBJECT

public:
    StartServerDialog(QQuickItem *parent = 0);

    Q_INVOKABLE void createServer();

signals:
    void messageLogged(const QString &message);

private:
    Server *m_server;

    void printServerInfo();
};

#endif // STARTSERVERDIALOG_H
