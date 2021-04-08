#ifndef PCCONSOLESTARTDIALOG_H
#define PCCONSOLESTARTDIALOG_H

#include <QQuickItem>
#include "server.h"
#include "settings.h"
#include "startgamedialog.h"

class PcConsoleStartDialog : public StartGameDialog
{
    Q_OBJECT
    QML_ELEMENT
public:
    PcConsoleStartDialog(QQuickItem *parent = 0);

    Q_INVOKABLE void consoleStart();

private:
    Server *m_server;
};

#endif // PCCONSOLESTARTDIALOG_H
