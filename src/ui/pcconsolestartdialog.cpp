#include "pcconsolestartdialog.h"

PcConsoleStartDialog::PcConsoleStartDialog(QQuickItem *parent) : StartGameDialog(parent)
{

}

void PcConsoleStartDialog::consoleStart()
{
    Config.HostAddress = "127.0.0.1";
    m_server = new Server(qApp);
    if (!m_server->listen()) {
        errorGet(tr("Cannot start server!"));
        m_server->deleteLater();
        return;
    }

    m_server->daemonize();
    connectToServer();
}
